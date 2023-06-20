/* Copyright (C) 2012 Atsushi Togo */
/* All rights reserved. */

/* This file is part of spglib. */

/* Redistribution and use in source and binary forms, with or without */
/* modification, are permitted provided that the following conditions */
/* are met: */

/* * Redistributions of source code must retain the above copyright */
/*   notice, this list of conditions and the following disclaimer. */

/* * Redistributions in binary form must reproduce the above copyright */
/*   notice, this list of conditions and the following disclaimer in */
/*   the documentation and/or other materials provided with the */
/*   distribution. */

/* * Neither the name of the spglib project nor the names of its */
/*   contributors may be used to endorse or promote products derived */
/*   from this software without specific prior written permission. */

/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS */
/* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT */
/* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS */
/* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE */
/* COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, */
/* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, */
/* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; */
/* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER */
/* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT */
/* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN */
/* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE */
/* POSSIBILITY OF SUCH DAMAGE. */

#include "spin.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "cell.h"
#include "debug.h"
#include "mathfunc.h"
#include "primitive.h"
#include "symmetry.h"

static MagneticSymmetry *get_operations(const Symmetry *sym_nonspin,
                                        const Cell *cell,
                                        int with_time_reversal, int is_axial,
                                        double symprec, double mag_symprec);
static int *get_symmetry_permutations(const MagneticSymmetry *magnetic_symmetry,
                                      const Cell *cell, int with_time_reversal,
                                      int is_axial, double symprec,
                                      double mag_symprec);
static int *get_orbits(const int *permutations, int num_sym, int num_atoms);
static int get_operation_sign_on_scalar(double spin_j, double spin_k,
                                        const double rot_cart[3][3],
                                        int with_time_reversal, int is_axial,
                                        double mag_symprec);
static int get_operation_sign_on_vector(int j, int k, const double *vectors,
                                        const double rot_cart[3][3],
                                        int with_time_reversal, int is_axial,
                                        double mag_symprec);
static void apply_symmetry_to_position(double pos_dst[3],
                                       const double pos_src[3],
                                       const int rot[3][3],
                                       const double trans[3]);
static double apply_symmetry_to_site_scalar(double src,
                                            const double rot_cart[3][3],
                                            int timerev, int with_time_reversal,
                                            int is_axial);
static void apply_symmetry_to_site_vector(double dst[3], int idx,
                                          const double *tensors,
                                          const double rot_cart[3][3],
                                          int timerev, int with_time_reversal,
                                          int is_axial);
void set_rotations_in_cartesian(double (*rotations_cart)[3][3],
                                const double lattice[3][3],
                                const MagneticSymmetry *magnetic_symmetry);
static int is_zero(double a, double mag_symprec);
static int is_zero_d3(const double a[3], double mag_symprec);

/******************************************************************************/

/* doc was moved to spin.h. */
MagneticSymmetry *spn_get_operations_with_site_tensors(
    int **equivalent_atoms, int **permutations, double prim_lattice[3][3],
    const Symmetry *sym_nonspin, const Cell *cell, const int with_time_reversal,
    const int is_axial, const double symprec, const double angle_tolerance,
    const double mag_symprec_) {
    // TODO: More robust way to guess mag_symprec
    double mag_symprec = (mag_symprec_ < 0) ? symprec : mag_symprec_;

    MagneticSymmetry *magnetic_symmetry = get_operations(
        sym_nonspin, cell, with_time_reversal, is_axial, symprec, mag_symprec);
    if (magnetic_symmetry == NULL) {
        return NULL;
    }

    /* equivalent atoms */
    *permutations =
        get_symmetry_permutations(magnetic_symmetry, cell, with_time_reversal,
                                  is_axial, symprec, mag_symprec);
    if (*permutations == NULL) {
        sym_free_magnetic_symmetry(magnetic_symmetry);
        magnetic_symmetry = NULL;
        return NULL;
    }
    *equivalent_atoms =
        get_orbits(*permutations, magnetic_symmetry->size, cell->size);
    if (*equivalent_atoms == NULL) {
        free(*permutations);
        permutations = NULL;
        sym_free_magnetic_symmetry(magnetic_symmetry);
        magnetic_symmetry = NULL;
        return NULL;
    }

    VecDBL *pure_trans =
        spn_collect_pure_translations_from_magnetic_symmetry(magnetic_symmetry);
    if (pure_trans == NULL) {
        free(*equivalent_atoms);
        equivalent_atoms = NULL;
        free(*permutations);
        permutations = NULL;
        sym_free_magnetic_symmetry(magnetic_symmetry);
        magnetic_symmetry = NULL;
        return NULL;
    }

    int multi = prm_get_primitive_lattice_vectors(
        prim_lattice, cell, pure_trans, symprec, angle_tolerance);

    /* By definition, change of number of pure translations would */
    /* not be allowed. */
    if (multi != pure_trans->size) {
        mat_free_VecDBL(pure_trans);
        pure_trans = NULL;
        free(*equivalent_atoms);
        equivalent_atoms = NULL;
        free(*permutations);
        permutations = NULL;
        sym_free_magnetic_symmetry(magnetic_symmetry);
        magnetic_symmetry = NULL;
        return NULL;
    }

    mat_free_VecDBL(pure_trans);
    pure_trans = NULL;

    return magnetic_symmetry;
}

VecDBL *spn_collect_pure_translations_from_magnetic_symmetry(
    const MagneticSymmetry *sym_msg) {
    static int identity[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

    VecDBL *pure_trans = mat_alloc_VecDBL(sym_msg->size);
    if (pure_trans == NULL) return NULL;

    int num_pure_trans = 0;
    for (int i = 0; i < sym_msg->size; i++) {
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
    VecDBL *ret_pure_trans = mat_alloc_VecDBL(num_pure_trans);
    if (ret_pure_trans == NULL) {
        mat_free_VecDBL(pure_trans);
        pure_trans = NULL;
        return NULL;
    }

    for (int i = 0; i < num_pure_trans; i++) {
        mat_copy_vector_d3(ret_pure_trans->vec[i], pure_trans->vec[i]);
    }

    mat_free_VecDBL(pure_trans);
    pure_trans = NULL;

    return ret_pure_trans;
}

/* Apply special position operator to `cell`. */
Cell *spn_get_idealized_cell(const int *permutations, const Cell *cell,
                             const MagneticSymmetry *magnetic_symmetry,
                             const int with_time_reversal, const int is_axial) {
    double pos_tmp[3], pos_res[3], vector_tmp[3], vector_res[3];
    int *inv_perm = (int *)malloc(sizeof(int) * cell->size);
    if (inv_perm == NULL) {
        return NULL;
    }

    Cell *exact_cell = cel_alloc_cell(cell->size, cell->tensor_rank);
    if (cell == NULL) {
        free(inv_perm);
        inv_perm = NULL;
        return NULL;
    }
    mat_copy_matrix_d3(exact_cell->lattice, cell->lattice);
    exact_cell->aperiodic_axis = cell->aperiodic_axis;
    exact_cell->size = cell->size;

    double(*rotations_cart)[3][3] =
        (double(*)[3][3])malloc(sizeof(double[3][3]) * magnetic_symmetry->size);
    if (rotations_cart == NULL) {
        cel_free_cell(exact_cell);
        exact_cell = NULL;
        free(inv_perm);
        inv_perm = NULL;
        return NULL;
    }
    set_rotations_in_cartesian(rotations_cart, cell->lattice,
                               magnetic_symmetry);

    double scalar_res = 0;
    for (int i = 0; i < cell->size; i++) {
        exact_cell->types[i] = cell->types[i];

        /* Initialize correction for site-i */
        for (int s = 0; s < 3; s++) {
            pos_res[s] = 0;
        }
        if (cell->tensor_rank == COLLINEAR) {
            scalar_res = 0;
        } else if (cell->tensor_rank == NONCOLLINEAR) {
            for (int s = 0; s < 3; s++) {
                vector_res[s] = 0;
            }
        }

        for (int p = 0; p < magnetic_symmetry->size; p++) {
            /* Inverse of the p-th permutation */
            for (int j = 0; j < cell->size; j++) {
                inv_perm[permutations[p * cell->size + j]] = j;
            }

            int j = inv_perm[i]; /* p-th operation maps site-j to site-i */

            apply_symmetry_to_position(pos_tmp, cell->position[j],
                                       magnetic_symmetry->rot[p],
                                       magnetic_symmetry->trans[p]);
            for (int s = 0; s < 3; s++) {
                /* To minimize rounding error, subtract by the original
                 * position[i][s]. */
                /* `pos_tmp[s] - cell->position[i][s]` should be close to
                 * integer. */
                pos_res[s] += pos_tmp[s] - cell->position[i][s] -
                              mat_Nint(pos_tmp[s] - cell->position[i][s]);
            }

            if (cell->tensor_rank == COLLINEAR) {
                double scalar_tmp = apply_symmetry_to_site_scalar(
                    cell->tensors[j], rotations_cart[p],
                    magnetic_symmetry->timerev[p], with_time_reversal,
                    is_axial);
                scalar_res += scalar_tmp - cell->tensors[i];
            } else if (cell->tensor_rank == NONCOLLINEAR) {
                apply_symmetry_to_site_vector(vector_tmp, j, cell->tensors,
                                              rotations_cart[p],
                                              magnetic_symmetry->timerev[p],
                                              with_time_reversal, is_axial);
                for (int s = 0; s < 3; s++) {
                    vector_res[s] += vector_tmp[s] - cell->tensors[3 * i + s];
                }
            }
        }

        for (int s = 0; s < 3; s++) {
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
            for (int s = 0; s < 3; s++) {
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
double *spn_alloc_site_tensors(const int num_atoms, const int tensor_rank) {
    switch (tensor_rank) {
        case 0:
            return (double *)malloc(sizeof(double) * num_atoms);
        case 1:
            return (double *)malloc(sizeof(double) * num_atoms * 3);
        default:
            warning_print("Not implemented tensor rank %d", tensor_rank);
            return NULL;
    }
}

/******************************************************************************/
/* Local functions                                                            */
/******************************************************************************/

/* Return NULL if failed */
/* returned MagneticSymmetry.timerev is NULL if with_time_reversal==false. */
/* is_axial: If true, tensors with tensor_rank==1 do not change by */
/*           spatial inversion */
static MagneticSymmetry *get_operations(
    const Symmetry *sym_nonspin, const Cell *cell, const int with_time_reversal,
    const int is_axial, const double symprec, const double mag_symprec) {
    double pos[3];
    double inv_lat[3][3];

    /* Site tensors in cartesian */
    double(*rotations_cart)[3][3] =
        (double(*)[3][3])malloc(sizeof(double[3][3]) * sym_nonspin->size);
    if (rotations_cart == NULL) {
        return NULL;
    }
    mat_inverse_matrix_d3(inv_lat, cell->lattice, 0);
    for (int i = 0; i < sym_nonspin->size; i++) {
        /* rot_cart = lattice @ rot @ lattice^-1 */
        mat_multiply_matrix_id3(rotations_cart[i], sym_nonspin->rot[i],
                                inv_lat);
        mat_multiply_matrix_d3(rotations_cart[i], cell->lattice,
                               rotations_cart[i]);
    }

    /* Need to reserve two times of nonspin symmetries for type-II magnetic
     * space group */
    int max_size = 2 * sym_nonspin->size;
    MatINT *rotations = mat_alloc_MatINT(max_size);
    if (rotations == NULL) {
        free(rotations_cart);
        rotations_cart = NULL;
        return NULL;
    }
    VecDBL *trans = mat_alloc_VecDBL(max_size);
    if (trans == NULL) {
        mat_free_MatINT(rotations);
        rotations = NULL;
        free(rotations_cart);
        rotations_cart = NULL;
        return NULL;
    }
    int *spin_flips = (int *)malloc(sizeof(int) * max_size);
    if (spin_flips == NULL) {
        mat_free_VecDBL(trans);
        trans = NULL;
        mat_free_MatINT(rotations);
        rotations = NULL;
        free(rotations_cart);
        rotations_cart = NULL;
        return NULL;
    }

    int num_sym = 0;
    for (int i = 0; i < sym_nonspin->size; i++) {
        /* When with_time_reversal=true, found becomes true if (rot, trans) */
        /* in family space group. */
        /* When with_time_reversal=false, found becomes true if (rot, trans) */
        /* in maximal space subgroup. */
        int found = 1;

        /* Set sign as undetermined */
        int determined = 0;
        int sign = 0;
        for (int j = 0; j < cell->size; j++) {
            /* Find atom-k overlapped with atom-j by operation-i */
            apply_symmetry_to_position(pos, cell->position[j],
                                       sym_nonspin->rot[i],
                                       sym_nonspin->trans[i]);
            int k = 0;
            for (; k < cell->size; k++) {
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

    free(rotations_cart);
    rotations_cart = NULL;
    MagneticSymmetry *magnetic_symmetry = sym_alloc_magnetic_symmetry(num_sym);
    if (magnetic_symmetry == NULL) {
        free(spin_flips);
        spin_flips = NULL;
        mat_free_VecDBL(trans);
        trans = NULL;
        mat_free_MatINT(rotations);
        rotations = NULL;
        return NULL;
    }
    for (int i = 0; i < num_sym; i++) {
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

    mat_free_MatINT(rotations);
    rotations = NULL;
    mat_free_VecDBL(trans);
    trans = NULL;
    free(spin_flips);
    spin_flips = NULL;

    return magnetic_symmetry;
}

// Return permutation tables `permutations` such that the p-th operation
// in `magnetic_symmetry` maps site-`i` to site-`permutations[p * cell->size +
// * i]`. If failed, return NULL.
static int *get_symmetry_permutations(const MagneticSymmetry *magnetic_symmetry,
                                      const Cell *cell,
                                      const int with_time_reversal,
                                      const int is_axial, const double symprec,
                                      const double mag_symprec) {
    double pos[3], vector[3], diff[3];

    int *permutations =
        (int *)malloc(sizeof(int) * magnetic_symmetry->size * cell->size);
    if (permutations == NULL) {
        return NULL;
    }

    /* Site tensors in cartesian */
    double(*rotations_cart)[3][3] =
        (double(*)[3][3])malloc(sizeof(double[3][3]) * magnetic_symmetry->size);
    if (rotations_cart == NULL) {
        free(permutations);
        permutations = NULL;
        return NULL;
    }
    set_rotations_in_cartesian(rotations_cart, cell->lattice,
                               magnetic_symmetry);

    for (int p = 0; p < magnetic_symmetry->size; p++) {
        for (int i = 0; i < cell->size; i++) {
            double scalar = 0;
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

            for (int j = 0; j < cell->size; j++) {
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
                    diff[0] = cell->tensors[(ptrdiff_t)(3 * j)] - vector[0];
                    diff[1] = cell->tensors[(ptrdiff_t)(3 * j + 1)] - vector[1];
                    diff[2] = cell->tensors[(ptrdiff_t)(3 * j + 2)] - vector[2];
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
                free(rotations_cart);
                rotations_cart = NULL;
                free(permutations);
                permutations = NULL;
                return NULL; /* Unreachable */
            }
        }

        debug_print("Operation %d\n", p);
        for (int i = 0; i < cell->size; i++) {
            debug_print(" %d", permutations[p * cell->size + i]);
        }
        debug_print("\n");
    }

    free(rotations_cart);
    rotations_cart = NULL;

    return permutations;
}

// Return equivalent_atoms. If failed, return NULL.
static int *get_orbits(const int *permutations, const int num_sym,
                       const int num_atoms) {
    int *equivalent_atoms = (int *)malloc(sizeof(int) * num_atoms);
    if (equivalent_atoms == NULL) {
        return NULL;
    }

    for (int i = 0; i < num_atoms; i++) {
        equivalent_atoms[i] = -1;
    }

    for (int i = 0; i < num_atoms; i++) {
        if (equivalent_atoms[i] != -1) {
            continue;
        }

        equivalent_atoms[i] = i;
        for (int s = 0; s < num_sym; s++) {
            equivalent_atoms[permutations[s * num_atoms + i]] = i;
        }
    }

    return equivalent_atoms;
}

/* Return sign in {-1, 1} such that `sign * spin'_j == spin_k` */
/* If spin_j and spin_k are not the same dimension, return 0 */
static int get_operation_sign_on_scalar(const double spin_j,
                                        const double spin_k,
                                        const double rot_cart[3][3],
                                        const int with_time_reversal,
                                        const int is_axial,
                                        const double mag_symprec) {
    for (int timerev = 0; timerev <= 1; timerev += 1) {
        double spin_j_ops = apply_symmetry_to_site_scalar(
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
static int get_operation_sign_on_vector(const int j, const int k,
                                        const double *vectors,
                                        const double rot_cart[3][3],
                                        const int with_time_reversal,
                                        const int is_axial,
                                        const double mag_symprec) {
    double vec_j_ops[3], diff[3];

    for (int timerev = 0; timerev <= 1; timerev++) {
        apply_symmetry_to_site_vector(vec_j_ops, j, vectors, rot_cart, timerev,
                                      with_time_reversal, is_axial);
        for (int s = 0; s < 3; s++) {
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
                                       const double pos_src[3],
                                       const int rot[3][3],
                                       const double trans[3]) {
    mat_multiply_matrix_vector_id3(pos_dst, rot, pos_src);
    for (int k = 0; k < 3; k++) {
        pos_dst[k] += trans[k];
    }
}

/* tensor_rank=0 case */
static double apply_symmetry_to_site_scalar(const double src,
                                            const double rot_cart[3][3],
                                            const int timerev,
                                            const int with_time_reversal,
                                            const int is_axial) {
    double dst = (with_time_reversal && timerev) ? -src : src;

    if (is_axial) {
        return dst * mat_get_determinant_d3(rot_cart);
    }
    return dst;
}

/* tensor_rank=1 case */
static void apply_symmetry_to_site_vector(double dst[3], const int idx,
                                          const double *tensors,
                                          const double rot_cart[3][3],
                                          const int timerev,
                                          const int with_time_reversal,
                                          const int is_axial) {
    double vec[3];

    for (int s = 0; s < 3; s++) {
        vec[s] = tensors[3 * idx + s];
    }

    double det = mat_get_determinant_d3(rot_cart);
    mat_multiply_matrix_vector_d3(dst, rot_cart, vec);
    for (int s = 0; s < 3; s++) {
        if (with_time_reversal && timerev) {
            dst[s] *= -1;
        }
        if (is_axial) {
            dst[s] *= det;
        }
    }
}

void set_rotations_in_cartesian(double (*rotations_cart)[3][3],
                                const double lattice[3][3],
                                const MagneticSymmetry *magnetic_symmetry) {
    double inv_lat[3][3];

    mat_inverse_matrix_d3(inv_lat, lattice, 0);
    for (int i = 0; i < magnetic_symmetry->size; i++) {
        /* rot_cart = lattice @ rot @ lattice^-1 */
        mat_multiply_matrix_id3(rotations_cart[i], magnetic_symmetry->rot[i],
                                inv_lat);
        mat_multiply_matrix_d3(rotations_cart[i], lattice, rotations_cart[i]);
    }
}

static int is_zero(const double a, const double mag_symprec) {
    return mat_Dabs(a) < mag_symprec;
}

static int is_zero_d3(const double a[3], const double mag_symprec) {
    for (int i = 0; i < 3; i++) {
        if (!is_zero(a[i], mag_symprec)) {
            return 0;
        }
    }
    return 1;
}
