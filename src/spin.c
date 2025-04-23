// Copyright (C) 2012 Atsushi Togo
// This file is part of spglib.
// SPDX-License-Identifier: BSD-3-Clause

#include "spin.h"

#include <stdio.h>
#include <stdlib.h>

#include "cell.h"
#include "debug.h"
#include "mathfunc.h"
#include "primitive.h"
#include "symmetry.h"

static MagneticSymmetry *get_operations(
    Symmetry const *sym_nonspin, Cell const *cell, int const with_time_reversal,
    int const is_axial, double const symprec, double const mag_symprec);
static int *get_symmetry_permutations(MagneticSymmetry const *magnetic_symmetry,
                                      Cell const *cell,
                                      int const with_time_reversal,
                                      int const is_axial, double const symprec,
                                      double const mag_symprec);
static int *get_orbits(int const *permutations, int const num_sym,
                       int const num_atoms);
static int get_operation_sign_on_scalar(
    double const spin_j, double const spin_k, double const rot_cart[3][3],
    int const with_time_reversal, int const is_axial, double const mag_symprec);
static int get_operation_sign_on_vector(int const j, int const k,
                                        double const *vectors,
                                        double const rot_cart[3][3],
                                        int const with_time_reversal,
                                        int const is_axial,
                                        double const mag_symprec);
static void apply_symmetry_to_position(double pos_dst[3],
                                       double const pos_src[3],
                                       int const rot[3][3],
                                       double const trans[3]);
static double apply_symmetry_to_site_scalar(double const src,
                                            double const rot_cart[3][3],
                                            int const timerev,
                                            int const with_time_reversal,
                                            int const is_axial);
static void apply_symmetry_to_site_vector(double dst[3], int const idx,
                                          double const *tensors,
                                          double const rot_cart[3][3],
                                          int const timerev,
                                          int const with_time_reversal,
                                          int const is_axial);
void set_rotations_in_cartesian(double (*rotations_cart)[3][3],
                                double const lattice[3][3],
                                MagneticSymmetry const *magnetic_symmetry);
static int is_zero(double const a, double const mag_symprec);
static int is_zero_d3(double const a[3], double const mag_symprec);

/******************************************************************************/

/* doc was moved to spin.h. */
MagneticSymmetry *spn_get_operations_with_site_tensors(
    int **equivalent_atoms, int **permutations, double prim_lattice[3][3],
    Symmetry const *sym_nonspin, Cell const *cell, int const with_time_reversal,
    int const is_axial, double const symprec, double const angle_tolerance,
    double const mag_symprec_) {
    int multi;
    double mag_symprec;
    MagneticSymmetry *magnetic_symmetry;
    VecDBL *pure_trans;

    magnetic_symmetry = NULL;
    pure_trans = NULL;

    // TODO: More robust way to guess mag_symprec
    if (mag_symprec_ < 0) {
        mag_symprec = symprec;
    } else {
        mag_symprec = mag_symprec_;
    }

    if ((magnetic_symmetry =
             get_operations(sym_nonspin, cell, with_time_reversal, is_axial,
                            symprec, mag_symprec)) == NULL) {
        goto err;
    }

    /* equivalent atoms */
    if ((*permutations = get_symmetry_permutations(
             magnetic_symmetry, cell, with_time_reversal, is_axial, symprec,
             mag_symprec)) == NULL) {
        goto err;
    }
    if ((*equivalent_atoms = get_orbits(*permutations, magnetic_symmetry->size,
                                        cell->size)) == NULL) {
        goto err;
    }

    if ((pure_trans = spn_collect_pure_translations_from_magnetic_symmetry(
             magnetic_symmetry)) == NULL)
        goto err;

    multi = prm_get_primitive_lattice_vectors(prim_lattice, cell, pure_trans,
                                              symprec, angle_tolerance);

    /* By definition, change of number of pure translations would */
    /* not be allowed. */
    if (multi != pure_trans->size) goto err;

    mat_free_VecDBL(pure_trans);
    pure_trans = NULL;

    return magnetic_symmetry;

err:
    if (pure_trans != NULL) {
        mat_free_VecDBL(pure_trans);
        pure_trans = NULL;
    }
    return NULL;
}

VecDBL *spn_collect_pure_translations_from_magnetic_symmetry(
    MagneticSymmetry const *sym_msg) {
    int i, num_pure_trans;
    VecDBL *pure_trans;
    VecDBL *ret_pure_trans;
    static int identity[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    num_pure_trans = 0;
    pure_trans = NULL;
    ret_pure_trans = NULL;

    if ((pure_trans = mat_alloc_VecDBL(sym_msg->size)) == NULL) return NULL;

    for (i = 0; i < sym_msg->size; i++) {
        /* Take translation with rot=identity and timerev=false */
        /* time reversal should be considered for type-IV magnetic space group
         */
        if (mat_check_identity_matrix_i3(identity, sym_msg->rot[i]) &&
            !sym_msg->timerev[i]) {
            mat_copy_vector_d3(pure_trans->vec[num_pure_trans],
                               sym_msg->trans[i]);
            num_pure_trans++;
        }
    }

    /* Shrink allocated size of VecDBL */
    if ((ret_pure_trans = mat_alloc_VecDBL(num_pure_trans)) == NULL) {
        mat_free_VecDBL(pure_trans);
        pure_trans = NULL;
        return NULL;
    }

    for (i = 0; i < num_pure_trans; i++) {
        mat_copy_vector_d3(ret_pure_trans->vec[i], pure_trans->vec[i]);
    }

    mat_free_VecDBL(pure_trans);
    pure_trans = NULL;

    return ret_pure_trans;
}

/* Apply special position operator to `cell`. */
Cell *spn_get_idealized_cell(int const *permutations, Cell const *cell,
                             MagneticSymmetry const *magnetic_symmetry,
                             int const with_time_reversal, int const is_axial) {
    int i, j, s, p;
    Cell *exact_cell;
    double scalar_tmp, scalar_res;
    double pos_tmp[3], pos_res[3], vector_tmp[3], vector_res[3];
    double (*rotations_cart)[3][3];
    int *inv_perm;

    scalar_res = 0;
    exact_cell = NULL;
    rotations_cart = NULL;
    inv_perm = NULL;

    if ((inv_perm = (int *)malloc(sizeof(int) * cell->size)) == NULL) {
        return NULL;
    }

    if ((exact_cell = cel_alloc_cell(cell->size, cell->tensor_rank)) == NULL) {
        return NULL;
    }
    mat_copy_matrix_d3(exact_cell->lattice, cell->lattice);
    exact_cell->aperiodic_axis = cell->aperiodic_axis;
    exact_cell->size = cell->size;

    if ((rotations_cart = (double (*)[3][3])malloc(
             sizeof(double[3][3]) * magnetic_symmetry->size)) == NULL) {
        return NULL;
    }
    set_rotations_in_cartesian(rotations_cart, cell->lattice,
                               magnetic_symmetry);

    for (i = 0; i < cell->size; i++) {
        exact_cell->types[i] = cell->types[i];

        /* Initialize correction for site-i */
        for (s = 0; s < 3; s++) {
            pos_res[s] = 0;
        }
        if (cell->tensor_rank == COLLINEAR) {
            scalar_res = 0;
        } else if (cell->tensor_rank == NONCOLLINEAR) {
            for (s = 0; s < 3; s++) {
                vector_res[s] = 0;
            }
        }

        for (p = 0; p < magnetic_symmetry->size; p++) {
            /* Inverse of the p-th permutation */
            for (j = 0; j < cell->size; j++) {
                inv_perm[permutations[p * cell->size + j]] = j;
            }

            j = inv_perm[i]; /* p-th operation maps site-j to site-i */

            apply_symmetry_to_position(pos_tmp, cell->position[j],
                                       magnetic_symmetry->rot[p],
                                       magnetic_symmetry->trans[p]);
            for (s = 0; s < 3; s++) {
                /* To minimize rounding error, subtract by the original
                 * position[i][s]. */
                /* `pos_tmp[s] - cell->position[i][s]` should be close to
                 * integer. */
                pos_res[s] += pos_tmp[s] - cell->position[i][s] -
                              mat_Nint(pos_tmp[s] - cell->position[i][s]);
            }

            if (cell->tensor_rank == COLLINEAR) {
                scalar_tmp = apply_symmetry_to_site_scalar(
                    cell->tensors[j], rotations_cart[p],
                    magnetic_symmetry->timerev[p], with_time_reversal,
                    is_axial);
                scalar_res += scalar_tmp - cell->tensors[i];
            } else if (cell->tensor_rank == NONCOLLINEAR) {
                apply_symmetry_to_site_vector(vector_tmp, j, cell->tensors,
                                              rotations_cart[p],
                                              magnetic_symmetry->timerev[p],
                                              with_time_reversal, is_axial);
                for (s = 0; s < 3; s++) {
                    vector_res[s] += vector_tmp[s] - cell->tensors[3 * i + s];
                }
            }
        }

        for (s = 0; s < 3; s++) {
            exact_cell->position[i][s] =
                cell->position[i][s] + pos_res[s] / magnetic_symmetry->size;
        }
        debug_print("Idealize position\n");
        debug_print_vector_d3(cell->position[i]);
        debug_print_vector_d3(exact_cell->position[i]);

        debug_print("Idealize site tensor\n");
        if (cell->tensor_rank == COLLINEAR) {
            exact_cell->tensors[i] =
                cell->tensors[i] + scalar_res / magnetic_symmetry->size;
            debug_print("%f\n", cell->tensors[i]);
            debug_print("%f\n", exact_cell->tensors[i]);
        } else if (cell->tensor_rank == NONCOLLINEAR) {
            for (s = 0; s < 3; s++) {
                exact_cell->tensors[3 * i + s] =
                    cell->tensors[3 * i + s] +
                    vector_res[s] / magnetic_symmetry->size;
            }
        }
    }

    free(rotations_cart);
    rotations_cart = NULL;
    free(inv_perm);
    inv_perm = NULL;

    return exact_cell;
}

/* If failed, return NULL. */
double *spn_alloc_site_tensors(int const num_atoms, int const tensor_rank) {
    double *ret;
    if (tensor_rank == 0) {
        if ((ret = (double *)malloc(sizeof(double) * num_atoms)) == NULL) {
            return NULL;
        }
    } else if (tensor_rank == 1) {
        if ((ret = (double *)malloc(sizeof(double) * num_atoms * 3)) == NULL) {
            return NULL;
        }
    } else {
        /* NotImplemented */
        return NULL;
    }
    return ret;
}

/******************************************************************************/
/* Local functions                                                            */
/******************************************************************************/

/* Return NULL if failed */
/* returned MagneticSymmetry.timerev is NULL if with_time_reversal==false. */
/* is_axial: If true, tensors with tensor_rank==1 do not change by */
/*           spatial inversion */
static MagneticSymmetry *get_operations(
    Symmetry const *sym_nonspin, Cell const *cell, int const with_time_reversal,
    int const is_axial, double const symprec, double const mag_symprec) {
    MagneticSymmetry *magnetic_symmetry;
    int i, j, k, sign, num_sym, found, determined, max_size;
    double pos[3];
    MatINT *rotations;
    VecDBL *trans;
    int *spin_flips;
    double (*rotations_cart)[3][3];
    double inv_lat[3][3];

    rotations_cart = NULL;

    /* Site tensors in cartesian */
    if ((rotations_cart = (double (*)[3][3])malloc(
             sizeof(double[3][3]) * sym_nonspin->size)) == NULL) {
        goto err;
    }
    mat_inverse_matrix_d3(inv_lat, cell->lattice, 0);
    for (i = 0; i < sym_nonspin->size; i++) {
        /* rot_cart = lattice @ rot @ lattice^-1 */
        mat_multiply_matrix_id3(rotations_cart[i], sym_nonspin->rot[i],
                                inv_lat);
        mat_multiply_matrix_d3(rotations_cart[i], cell->lattice,
                               rotations_cart[i]);
    }

    /* Need to reserve two times of nonspin symmetries for type-II magnetic
     * space group */
    max_size = 2 * sym_nonspin->size;
    rotations = mat_alloc_MatINT(max_size);
    trans = mat_alloc_VecDBL(max_size);
    if ((spin_flips = (int *)malloc(sizeof(int) * max_size)) == NULL) {
        goto err;
    }

    num_sym = 0;

    for (i = 0; i < sym_nonspin->size; i++) {
        /* When with_time_reversal=true, found becomes true if (rot, trans) */
        /* in family space group. */
        /* When with_time_reversal=false, found becomes true if (rot, trans) */
        /* in maximal space subgroup. */
        found = 1;

        /* Set sign as undetermined */
        determined = 0;
        sign = 0;
        for (j = 0; j < cell->size; j++) {
            /* Find atom-k overlapped with atom-j by operation-i */
            apply_symmetry_to_position(pos, cell->position[j],
                                       sym_nonspin->rot[i],
                                       sym_nonspin->trans[i]);
            for (k = 0; k < cell->size; k++) {
                if (cel_is_overlap_with_same_type(
                        cell->position[k], pos, cell->types[k], cell->types[j],
                        cell->lattice, symprec)) {
                    /* Break because cel_is_overlap_with_same_type == true */
                    /* for only one atom. */
                    break;
                }
            }
            if (k == cell->size) {
                // Unreachable here in theory, but we rarely fail to overlap
                // atoms possibly due to too high symprec. In that case, skip
                // the symmetry operation.
                debug_print("Failed to overlap atom-%d by operation-%d\n", j,
                            i);
                found = 0;
                break;
            }

            // Skip if relevant tensors are zeros because they have nothing to
            // do with magnetic symmetry search!
            // If spins `m` and `-m` are equal up to `mag_symprec`,
            // m - (-m) = 2m < mag_symprec
            // Thus, we need to check `m` and 0 up to `0.5 * mag_symprec`!
            if (cell->tensor_rank == COLLINEAR) {
                if (is_zero(cell->tensors[j], 0.5 * mag_symprec) &&
                    is_zero(cell->tensors[k], 0.5 * mag_symprec)) {
                    continue;
                }
            }
            if (cell->tensor_rank == NONCOLLINEAR) {
                if (is_zero(cell->tensors[j * 3], 0.5 * mag_symprec) &&
                    is_zero(cell->tensors[j * 3 + 1], 0.5 * mag_symprec) &&
                    is_zero(cell->tensors[j * 3 + 2], 0.5 * mag_symprec) &&
                    is_zero(cell->tensors[k * 3], 0.5 * mag_symprec) &&
                    is_zero(cell->tensors[k * 3 + 1], 0.5 * mag_symprec) &&
                    is_zero(cell->tensors[k * 3 + 2], 0.5 * mag_symprec)) {
                    continue;
                }
            }

            if (!determined) {
                /* Determine sign */
                if (cell->tensor_rank == COLLINEAR) {
                    sign = get_operation_sign_on_scalar(
                        cell->tensors[j], cell->tensors[k], rotations_cart[i],
                        with_time_reversal, is_axial, mag_symprec);
                }
                if (cell->tensor_rank == NONCOLLINEAR) {
                    sign = get_operation_sign_on_vector(
                        j, k, cell->tensors, rotations_cart[i],
                        with_time_reversal, is_axial, mag_symprec);
                }
                determined = 1;

                if (sign == 0 || (!with_time_reversal && sign != 1)) {
                    /* When with_time_reversal=false, only sign=1 operation is
                     * accepted
                     */
                    found = 0;
                    break;
                }
            } else {
                /* Check if `sign` is consistent */
                if (cell->tensor_rank == COLLINEAR) {
                    if (get_operation_sign_on_scalar(
                            cell->tensors[j], cell->tensors[k],
                            rotations_cart[i], with_time_reversal, is_axial,
                            mag_symprec) != sign) {
                        found = 0;
                        break;
                    }
                }
                if (cell->tensor_rank == NONCOLLINEAR) {
                    if (get_operation_sign_on_vector(
                            j, k, cell->tensors, rotations_cart[i],
                            with_time_reversal, is_axial,
                            mag_symprec) != sign) {
                        found = 0;
                        break;
                    }
                }
            }
        }
        if (found) {
            /* (with_time_reversal, determined, sign) */
            /* (true,        true,       1/-1) -> accept */
            /* (true,        false,      0)    -> take both sign 1/-1 */
            /* (false,       true,       1)    -> accept */
            /* (false,       true,       -1)   -> not occurred */
            /* (false,       false,      0)    -> accept */
            if (determined) {
                /* (with_time_reversal, determined, sign) */
                /* (true,        true,       1/-1) -> accept */
                /* (false,       true,       1)    -> accept */
                mat_copy_matrix_i3(rotations->mat[num_sym],
                                   sym_nonspin->rot[i]);
                mat_copy_vector_d3(trans->vec[num_sym], sym_nonspin->trans[i]);
                if (with_time_reversal) {
                    spin_flips[num_sym] = sign;
                }
                num_sym++;
            } else if (with_time_reversal) {
                /* (with_time_reversal, determined, sign) */
                /* (true,        false,      0)    -> take both sign 1/-1 */

                /* sign=1 */
                mat_copy_matrix_i3(rotations->mat[num_sym],
                                   sym_nonspin->rot[i]);
                mat_copy_vector_d3(trans->vec[num_sym], sym_nonspin->trans[i]);
                spin_flips[num_sym] = 1;
                num_sym++;

                /* sign=-1 */
                mat_copy_matrix_i3(rotations->mat[num_sym],
                                   sym_nonspin->rot[i]);
                mat_copy_vector_d3(trans->vec[num_sym], sym_nonspin->trans[i]);
                spin_flips[num_sym] = -1;
                num_sym++;
            } else {
                /* (with_time_reversal, determined, sign) */
                /* (false,       false,      0) */
                mat_copy_matrix_i3(rotations->mat[num_sym],
                                   sym_nonspin->rot[i]);
                mat_copy_vector_d3(trans->vec[num_sym], sym_nonspin->trans[i]);
                num_sym++;
            }
        }
    }

    magnetic_symmetry = sym_alloc_magnetic_symmetry(num_sym);
    for (i = 0; i < num_sym; i++) {
        mat_copy_matrix_i3(magnetic_symmetry->rot[i], rotations->mat[i]);
        mat_copy_vector_d3(magnetic_symmetry->trans[i], trans->vec[i]);
        if (with_time_reversal) {
            magnetic_symmetry->timerev[i] = (1 - spin_flips[i]) / 2;
        } else {
            /* fill as ordinary operation */
            magnetic_symmetry->timerev[i] = 0;
        }
        debug_print("-- %d -- \n", i);
        debug_print_matrix_i3(magnetic_symmetry->rot[i]);
        debug_print_vector_d3(magnetic_symmetry->trans[i]);
        debug_print("timerev %d\n", magnetic_symmetry->timerev[i]);
    }

    free(rotations_cart);
    rotations_cart = NULL;
    mat_free_MatINT(rotations);
    rotations = NULL;
    mat_free_VecDBL(trans);
    trans = NULL;
    free(spin_flips);
    spin_flips = NULL;

    return magnetic_symmetry;

err:
    if (rotations_cart != NULL) {
        free(rotations_cart);
        rotations_cart = NULL;
    }
    return NULL;
}

// Return permutation tables `permutations` such that the p-th operation
// in `magnetic_symmetry` maps site-`i` to site-`permutations[p * cell->size +
// * i]`. If failed, return NULL.
static int *get_symmetry_permutations(MagneticSymmetry const *magnetic_symmetry,
                                      Cell const *cell,
                                      int const with_time_reversal,
                                      int const is_axial, double const symprec,
                                      double const mag_symprec) {
    int p, i, j;
    int *permutations;
    double scalar;
    double pos[3], vector[3], diff[3];
    double (*rotations_cart)[3][3];

    rotations_cart = NULL;
    permutations = NULL;

    if ((permutations = (int *)malloc(sizeof(int) * magnetic_symmetry->size *
                                      cell->size)) == NULL) {
        return NULL;
    }

    /* Site tensors in cartesian */
    if ((rotations_cart = (double (*)[3][3])malloc(
             sizeof(double[3][3]) * magnetic_symmetry->size)) == NULL) {
        free(permutations);
        permutations = NULL;
        return NULL;
    }
    set_rotations_in_cartesian(rotations_cart, cell->lattice,
                               magnetic_symmetry);

    for (p = 0; p < magnetic_symmetry->size; p++) {
        for (i = 0; i < cell->size; i++) {
            permutations[p * cell->size + i] = -1;

            /* Apply operation on site-i */
            apply_symmetry_to_position(pos, cell->position[i],
                                       magnetic_symmetry->rot[p],
                                       magnetic_symmetry->trans[p]);
            if (cell->tensor_rank == COLLINEAR) {
                scalar = apply_symmetry_to_site_scalar(
                    cell->tensors[i], rotations_cart[p],
                    magnetic_symmetry->timerev[p], with_time_reversal,
                    is_axial);
            } else if (cell->tensor_rank == NONCOLLINEAR) {
                apply_symmetry_to_site_vector(vector, i, cell->tensors,
                                              rotations_cart[p],
                                              magnetic_symmetry->timerev[p],
                                              with_time_reversal, is_axial);
            }

            for (j = 0; j < cell->size; j++) {
                if (!cel_is_overlap_with_same_type(
                        pos, cell->position[j], cell->types[i], cell->types[j],
                        cell->lattice, symprec)) {
                    continue;
                }
                debug_print("Try to overlap site-%d (%f) with site-%d (%f)\n",
                            i, scalar, j, cell->tensors[j]);
                if (cell->tensor_rank == COLLINEAR &&
                    !is_zero(cell->tensors[j] - scalar, mag_symprec)) {
                    continue;
                }
                if (cell->tensor_rank == NONCOLLINEAR) {
                    diff[0] = cell->tensors[3 * j] - vector[0];
                    diff[1] = cell->tensors[3 * j + 1] - vector[1];
                    diff[2] = cell->tensors[3 * j + 2] - vector[2];
                    if (!is_zero_d3(diff, mag_symprec)) {
                        continue;
                    }
                }

                /* Now, operation-p maps site-i to site-j */
                permutations[p * cell->size + i] = j;
                break;
            }

            if (permutations[p * cell->size + i] == -1) {
                debug_print("Failed to map site-%d by operation-%d\n", i, p);
                return NULL; /* Unreachable */
            }
        }

        debug_print("Operation %d\n", p);
        for (i = 0; i < cell->size; i++) {
            debug_print(" %d", permutations[p * cell->size + i]);
        }
        debug_print("\n");
    }

    free(rotations_cart);
    rotations_cart = NULL;

    return permutations;
}

// Return equivalent_atoms. If failed, return NULL.
static int *get_orbits(int const *permutations, int const num_sym,
                       int const num_atoms) {
    int s, i;
    int *equivalent_atoms;

    if ((equivalent_atoms = (int *)malloc(sizeof(int) * num_atoms)) == NULL) {
        return NULL;
    }

    for (i = 0; i < num_atoms; i++) {
        equivalent_atoms[i] = -1;
    }

    for (i = 0; i < num_atoms; i++) {
        if (equivalent_atoms[i] != -1) {
            continue;
        }

        equivalent_atoms[i] = i;
        for (s = 0; s < num_sym; s++) {
            equivalent_atoms[permutations[s * num_atoms + i]] = i;
        }
    }

    return equivalent_atoms;
}

/* Return sign in {-1, 1} such that `sign * spin'_j == spin_k` */
/* If spin_j and spin_k are not the same dimension, return 0 */
static int get_operation_sign_on_scalar(double const spin_j,
                                        double const spin_k,
                                        double const rot_cart[3][3],
                                        int const with_time_reversal,
                                        int const is_axial,
                                        double const mag_symprec) {
    int timerev;
    double spin_j_ops;

    for (timerev = 0; timerev <= 1; timerev += 1) {
        spin_j_ops = apply_symmetry_to_site_scalar(
            spin_j, rot_cart, timerev, with_time_reversal, is_axial);
        if (is_zero(spin_k - spin_j_ops, mag_symprec)) {
            return 1 - 2 * timerev; /* Spin-flip */
        }
    }

    return 0;
}

/* Work in Cartesian coordinates. */
/* Return sign in {-1, 1} such that `sign * v'_j == sign * v_k` */
/* If v_j and v_k are not transformed each other, return 0 */
/* is_axial: Non-collinear magnetic moment v'_j = |detR|R v_j */
/* !is_axial: Usual vector: v'_j = R v_j */
static int get_operation_sign_on_vector(int const j, int const k,
                                        double const *vectors,
                                        double const rot_cart[3][3],
                                        int const with_time_reversal,
                                        int const is_axial,
                                        double const mag_symprec) {
    int s, timerev;
    double vec_j_ops[3], diff[3];

    for (timerev = 0; timerev <= 1; timerev++) {
        apply_symmetry_to_site_vector(vec_j_ops, j, vectors, rot_cart, timerev,
                                      with_time_reversal, is_axial);
        for (s = 0; s < 3; s++) {
            diff[s] = vectors[3 * k + s] - vec_j_ops[s];
        }
        if (is_zero_d3(diff, mag_symprec)) {
            return 1 - 2 * timerev; /* Spin-flip */
        }
    }

    return 0;
}

/* Apply `idx`th operation in `magnetic_symmetry` to position. */
static void apply_symmetry_to_position(double pos_dst[3],
                                       double const pos_src[3],
                                       int const rot[3][3],
                                       double const trans[3]) {
    int k;
    mat_multiply_matrix_vector_id3(pos_dst, rot, pos_src);
    for (k = 0; k < 3; k++) {
        pos_dst[k] += trans[k];
    }
}

/* tensor_rank=0 case */
static double apply_symmetry_to_site_scalar(double const src,
                                            double const rot_cart[3][3],
                                            int const timerev,
                                            int const with_time_reversal,
                                            int const is_axial) {
    double det, dst;

    dst = (with_time_reversal && timerev) ? -src : src;

    if (is_axial) {
        det = mat_get_determinant_d3(rot_cart);
        dst *= det;
    }
    return dst;
}

/* tensor_rank=1 case */
static void apply_symmetry_to_site_vector(double dst[3], int const idx,
                                          double const *tensors,
                                          double const rot_cart[3][3],
                                          int const timerev,
                                          int const with_time_reversal,
                                          int const is_axial) {
    int s;
    double det;
    double vec[3];

    for (s = 0; s < 3; s++) {
        vec[s] = tensors[3 * idx + s];
    }

    det = mat_get_determinant_d3(rot_cart);
    mat_multiply_matrix_vector_d3(dst, rot_cart, vec);
    for (s = 0; s < 3; s++) {
        if (with_time_reversal && timerev) {
            dst[s] *= -1;
        }
        if (is_axial) {
            dst[s] *= det;
        }
    }
}

void set_rotations_in_cartesian(double (*rotations_cart)[3][3],
                                double const lattice[3][3],
                                MagneticSymmetry const *magnetic_symmetry) {
    int i;
    double inv_lat[3][3];

    mat_inverse_matrix_d3(inv_lat, lattice, 0);
    for (i = 0; i < magnetic_symmetry->size; i++) {
        /* rot_cart = lattice @ rot @ lattice^-1 */
        mat_multiply_matrix_id3(rotations_cart[i], magnetic_symmetry->rot[i],
                                inv_lat);
        mat_multiply_matrix_d3(rotations_cart[i], lattice, rotations_cart[i]);
    }
}

static int is_zero(double const a, double const mag_symprec) {
    return mat_Dabs(a) < mag_symprec;
}

static int is_zero_d3(double const a[3], double const mag_symprec) {
    int i;
    for (i = 0; i < 3; i++) {
        if (!is_zero(a[i], mag_symprec)) {
            return 0;
        }
    }
    return 1;
}
