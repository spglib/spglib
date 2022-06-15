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

#include <stdio.h>
#include <stdlib.h>

#include "cell.h"
#include "debug.h"
#include "mathfunc.h"
#include "primitive.h"
#include "symmetry.h"

static MagneticSymmetry *get_operations(
    const Symmetry *sym_nonspin, const Cell *cell, const double *tensors,
    const int tensor_rank, const int is_magnetic, const int is_axial,
    const double symprec, const double mag_symprec);
static int *get_symmetry_permutations(const MagneticSymmetry *magnetic_symmetry,
                                      const Cell *cell, const double *tensors,
                                      const int tensor_rank,
                                      const int is_magnetic, const int is_axial,
                                      const double symprec,
                                      const double mag_symprec);
static int *get_orbits(const int *permutations, const int num_sym,
                       const int num_atoms);
static int get_operation_sign_on_scalar(
    const double spin_j, const double spin_k, SPGCONST double rot_cart[3][3],
    const int is_magnetic, const int is_axial, const double mag_symprec);
static int get_operation_sign_on_vector(const int j, const int k,
                                        const double *vectors,
                                        SPGCONST double rot_cart[3][3],
                                        const int is_magnetic,
                                        const int is_axial,
                                        const double mag_symprec);
static void apply_symmetry_to_position(double pos_dst[3],
                                       SPGCONST double pos_src[3],
                                       SPGCONST int rot[3][3],
                                       SPGCONST double trans[3]);
static void apply_symmetry_to_site_scalar(double *dst, const double src,
                                          SPGCONST double rot_cart[3][3],
                                          const int timerev,
                                          const int is_magnetic,
                                          const int is_axial);
static void apply_symmetry_to_site_vector(double *dst, const int idx,
                                          const double *tensors,
                                          SPGCONST double rot_cart[3][3],
                                          const int timerev,
                                          const int is_magnetic,
                                          const int is_axial);
static int is_zero(const double a, const double mag_symprec);
static int is_zero_d3(const double a[3], const double mag_symprec);

/******************************************************************************/

/* `permutations`: such that the p-th operation in `magnetic_symmetry` maps */
/* site-`i` to site-`permutations[p * cell->size + i]`. */
/* Return NULL if failed */
MagneticSymmetry *spn_get_operations_with_site_tensors(
    int **equivalent_atoms, int **permutations, double prim_lattice[3][3],
    const Symmetry *sym_nonspin, const Cell *cell, const double *tensors,
    const int tensor_rank, const int is_magnetic, const double symprec,
    const double angle_tolerance) {
    int i, multi, is_axial;
    double mag_symprec;
    MagneticSymmetry *magnetic_symmetry;
    VecDBL *pure_trans;

    int identity[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

    magnetic_symmetry = NULL;
    pure_trans = NULL;

    /* TODO(shinohara): allow to select this flag from input */
    is_axial = (tensor_rank == 1) ? is_magnetic : 0;

    /* TODO(shinohara): allow to choose different tolerance for positions and
     * magmoms */
    mag_symprec = symprec;

    if ((magnetic_symmetry = get_operations(sym_nonspin, cell, tensors,
                                            tensor_rank, is_magnetic, is_axial,
                                            symprec, mag_symprec)) == NULL) {
        goto err;
    }

    /* equivalent atoms */
    if ((*permutations = get_symmetry_permutations(
             magnetic_symmetry, cell, tensors, tensor_rank, is_magnetic,
             is_axial, symprec, mag_symprec)) == NULL) {
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
    const MagneticSymmetry *sym_msg) {
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
/******************************************************************************/
/* Local functions                                                            */
/******************************************************************************/

/* Return NULL if failed */
/* returned MagneticSymmetry.timerev is NULL if is_magnetic==false. */
/* is_axial: If true, tensors with tensor_rank==1 do not change by */
/*           spatial inversion */
static MagneticSymmetry *get_operations(
    const Symmetry *sym_nonspin, const Cell *cell, const double *tensors,
    const int tensor_rank, const int is_magnetic, const int is_axial,
    const double symprec, const double mag_symprec) {
    MagneticSymmetry *magnetic_symmetry;
    int i, j, k, sign, num_sym, found, determined, max_size;
    double pos[3];
    MatINT *rotations;
    VecDBL *trans;
    int *spin_flips;
    double(*rotations_cart)[3][3];
    double inv_lat[3][3];

    rotations_cart = NULL;

    /* Site tensors in cartesian */
    if ((rotations_cart = (double(*)[3][3])malloc(sizeof(double[3][3]) *
                                                  sym_nonspin->size)) == NULL) {
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
        /* When is_magnetic=true, found becomes true if (rot, trans) */
        /* in family space group. */
        /* When is_magnetic=false, found becomes true if (rot, trans) */
        /* in maximal space subgroup. */
        found = 1;

        /* Set sign as undetermined */
        determined = 0;
        sign = 0;
        for (j = 0; j < cell->size; j++) {
            /* Find atom-k overlapped with atom-j by opration-i */
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
                /* Unreachable here! */
                return NULL;
            }

            /* Skip if relevant tensors are zeros because they have nothing to
             * do with magnetic symmetry search! */
            if (tensor_rank == 0) {
                if (is_zero(tensors[j], mag_symprec) &&
                    is_zero(tensors[k], mag_symprec)) {
                    continue;
                }
            }
            if (tensor_rank == 1) {
                if (is_zero(tensors[j * 3], mag_symprec) &&
                    is_zero(tensors[j * 3 + 1], mag_symprec) &&
                    is_zero(tensors[j * 3 + 2], mag_symprec) &&
                    is_zero(tensors[k * 3], mag_symprec) &&
                    is_zero(tensors[k * 3 + 1], mag_symprec) &&
                    is_zero(tensors[k * 3 + 2], mag_symprec)) {
                    continue;
                }
            }

            if (!determined) {
                /* Determine sign */
                if (tensor_rank == 0) {
                    sign = get_operation_sign_on_scalar(
                        tensors[j], tensors[k], rotations_cart[i], is_magnetic,
                        is_axial, mag_symprec);
                }
                if (tensor_rank == 1) {
                    sign = get_operation_sign_on_vector(
                        j, k, tensors, rotations_cart[i], is_magnetic, is_axial,
                        mag_symprec);
                }
                determined = 1;

                if (sign == 0 || (!is_magnetic && sign != 1)) {
                    /* When is_magnetic=false, only sign=1 operation is accepted
                     */
                    found = 0;
                    break;
                }
            } else {
                /* Check if `sign` is consistent */
                if (tensor_rank == 0) {
                    if (get_operation_sign_on_scalar(
                            tensors[j], tensors[k], rotations_cart[i],
                            is_magnetic, is_axial, mag_symprec) != sign) {
                        found = 0;
                        break;
                    }
                }
                if (tensor_rank == 1) {
                    if (get_operation_sign_on_vector(
                            j, k, tensors, rotations_cart[i], is_magnetic,
                            is_axial, mag_symprec) != sign) {
                        found = 0;
                        break;
                    }
                }
            }
        }
        if (found) {
            /* (is_magnetic, determined, sign) */
            /* (true,        true,       1/-1) -> accept */
            /* (true,        false,      0)    -> take both sign 1/-1 */
            /* (false,       true,       1)    -> accept */
            /* (false,       true,       -1)   -> not occurred */
            /* (false,       false,      0)    -> accept */
            if (determined) {
                /* (is_magnetic, determined, sign) */
                /* (true,        true,       1/-1) -> accept */
                /* (false,       true,       1)    -> accept */
                mat_copy_matrix_i3(rotations->mat[num_sym],
                                   sym_nonspin->rot[i]);
                mat_copy_vector_d3(trans->vec[num_sym], sym_nonspin->trans[i]);
                if (is_magnetic) {
                    spin_flips[num_sym] = sign;
                }
                num_sym++;
            } else if (is_magnetic) {
                /* (is_magnetic, determined, sign) */
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
                /* (is_magnetic, determined, sign) */
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
        if (is_magnetic) {
            magnetic_symmetry->timerev[i] = (1 - spin_flips[i]) / 2;
        } else {
            /* fill as ordinary operation */
            magnetic_symmetry->timerev[i] = 0;
        }
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

/* Return permutation tables `permutations` such that the p-th operation */
/* in `magnetic_symmetry` maps site-`i` to site-`permutations[p * cell->size +
 * i]`. */
static int *get_symmetry_permutations(const MagneticSymmetry *magnetic_symmetry,
                                      const Cell *cell, const double *tensors,
                                      const int tensor_rank,
                                      const int is_magnetic, const int is_axial,
                                      const double symprec,
                                      const double mag_symprec) {
    int p, i, j;
    int *permutations;
    double scalar;
    double pos[3], vector[3], diff[3];
    double(*rotations_cart)[3][3];
    double inv_lat[3][3];

    rotations_cart = NULL;
    permutations = NULL;

    if ((permutations = (int *)malloc(sizeof(int) * magnetic_symmetry->size *
                                      cell->size)) == NULL) {
        return NULL;
    }

    /* Site tensors in cartesian */
    if ((rotations_cart = (double(*)[3][3])malloc(
             sizeof(double[3][3]) * magnetic_symmetry->size)) == NULL) {
        free(permutations);
        permutations = NULL;
        return NULL;
    }
    mat_inverse_matrix_d3(inv_lat, cell->lattice, 0);
    for (i = 0; i < magnetic_symmetry->size; i++) {
        /* rot_cart = lattice @ rot @ lattice^-1 */
        mat_multiply_matrix_id3(rotations_cart[i], magnetic_symmetry->rot[i],
                                inv_lat);
        mat_multiply_matrix_d3(rotations_cart[i], cell->lattice,
                               rotations_cart[i]);
    }

    for (p = 0; p < magnetic_symmetry->size; p++) {
        for (i = 0; i < cell->size; i++) {
            permutations[p * cell->size + i] = -1;

            /* Apply operation on site-i */
            apply_symmetry_to_position(pos, cell->position[i],
                                       magnetic_symmetry->rot[p],
                                       magnetic_symmetry->trans[p]);
            if (tensor_rank == 0) {
                apply_symmetry_to_site_scalar(
                    &scalar, tensors[i], rotations_cart[p],
                    magnetic_symmetry->timerev[p], is_magnetic, is_axial);
            } else if (tensor_rank == 1) {
                apply_symmetry_to_site_vector(
                    vector, i, tensors, rotations_cart[p],
                    magnetic_symmetry->timerev[p], is_magnetic, is_axial);
            }

            for (j = 0; j < cell->size; j++) {
                if (!cel_is_overlap_with_same_type(
                        pos, cell->position[j], cell->types[i], cell->types[j],
                        cell->lattice, symprec)) {
                    continue;
                }
                if (tensor_rank == 0 &&
                    !is_zero(tensors[j] - scalar, mag_symprec)) {
                    continue;
                }
                if (tensor_rank == 1) {
                    diff[0] = tensors[3 * j] - vector[0];
                    diff[1] = tensors[3 * j + 1] - vector[1];
                    diff[2] = tensors[3 * j + 2] - vector[2];
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

/* Return equivalent_atoms */
static int *get_orbits(const int *permutations, const int num_sym,
                       const int num_atoms) {
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

/* Return sign in {-1, 1} such that `spin_j == sign * spin_k` */
/* If spin_j and spin_k are not the same dimension, return 0 */
static int get_operation_sign_on_scalar(
    const double spin_j, const double spin_k, SPGCONST double rot_cart[3][3],
    const int is_magnetic, const int is_axial, const double mag_symprec) {
    int timerev;
    double spin_k_ops;

    for (timerev = 0; timerev <= 1; timerev += 1) {
        apply_symmetry_to_site_scalar(&spin_k_ops, spin_k, rot_cart, timerev,
                                      is_magnetic, is_axial);
        if (is_zero(spin_j - spin_k_ops, mag_symprec)) {
            return 1 - 2 * timerev; /* Spin-flip */
        }
    }

    return 0;
}

/* Work in Cartesian coordinates. */
/* Return sign in {-1, 1} such that `v_j == sign * v_k` */
/* If v_j and v_k are not transformed each other, return 0 */
/* is_axial: Non-collinear magnetic moment m' = |detR|Rm */
/* !is_axial: Usual vector: v' = Rv */
static int get_operation_sign_on_vector(const int j, const int k,
                                        const double *vectors,
                                        SPGCONST double rot_cart[3][3],
                                        const int is_magnetic,
                                        const int is_axial,
                                        const double mag_symprec) {
    int i, timerev;
    double vec_k[3], vec_k_ops[3], diff[3];

    for (i = 0; i < 3; i++) {
        vec_k[i] = vectors[3 * k + i];
    }

    for (timerev = 0; timerev <= 1; timerev++) {
        apply_symmetry_to_site_vector(vec_k_ops, k, vectors, rot_cart, timerev,
                                      is_magnetic, is_axial);
        for (i = 0; i < 3; i++) {
            diff[i] = vectors[3 * j + i] - vec_k_ops[i];
        }
        if (is_zero_d3(diff, mag_symprec)) {
            return 1 - 2 * timerev; /* Spin-flip */
        }
    }

    return 0;
}

/* Apply `idx`th operation in `magnetic_symmetry` to position. */
static void apply_symmetry_to_position(double pos_dst[3],
                                       SPGCONST double pos_src[3],
                                       SPGCONST int rot[3][3],
                                       SPGCONST double trans[3]) {
    int k;
    mat_multiply_matrix_vector_id3(pos_dst, rot, pos_src);
    for (k = 0; k < 3; k++) {
        pos_dst[k] += trans[k];
    }
}

/* tensor_rank=0 case */
static void apply_symmetry_to_site_scalar(double *dst, const double src,
                                          SPGCONST double rot_cart[3][3],
                                          const int timerev,
                                          const int is_magnetic,
                                          const int is_axial) {
    double det;

    *dst = (is_magnetic && timerev) ? -src : src;

    if (is_axial) {
        det = mat_get_determinant_d3(rot_cart);
        *dst *= det;
    }
}

/* tensor_rank=1 case */
static void apply_symmetry_to_site_vector(double *dst, const int idx,
                                          const double *tensors,
                                          SPGCONST double rot_cart[3][3],
                                          const int timerev,
                                          const int is_magnetic,
                                          const int is_axial) {
    int k;
    double det;
    double vec[3];

    for (k = 0; k < 3; k++) {
        vec[k] = tensors[3 * idx + k];
    }

    det = mat_get_determinant_d3(rot_cart);
    mat_multiply_matrix_vector_d3(dst, rot_cart, vec);
    for (k = 0; k < 3; k++) {
        if (is_magnetic && timerev) {
            dst[k] *= -1;
        }
        if (is_axial) {
            dst[k] *= det;
        }
    }
}

static int is_zero(const double a, const double mag_symprec) {
    return mat_Dabs(a) < mag_symprec;
}

static int is_zero_d3(const double a[3], const double mag_symprec) {
    int i;
    for (i = 0; i < 3; i++) {
        if (!is_zero(a[i], mag_symprec)) {
            return 0;
        }
    }
    return 1;
}
