/* Copyright (C) 2008 Atsushi Togo */
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

#include "primitive.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "cell.h"
#include "debug.h"
#include "delaunay.h"
#include "mathfunc.h"
#include "symmetry.h"

#define REDUCE_RATE 0.95
#define NUM_ATTEMPT 20

static Primitive *get_primitive(Cell const *cell, double const symprec,
                                double const angle_tolerance);
static Cell *get_cell_with_smallest_lattice(Cell const *cell,
                                            double const symprec);
static Cell *get_primitive_cell(int *mapping_table, Cell const *cell,
                                VecDBL const *pure_trans, double const symprec,
                                double const angle_tolerance);
static int get_primitive_lattice_vectors(double prim_lattice[3][3],
                                         Cell const *cell,
                                         VecDBL const *pure_trans,
                                         double const symprec,
                                         double const angle_tolerance);
static int find_primitive_lattice_vectors(double prim_lattice[3][3],
                                          VecDBL const *vectors,
                                          Cell const *cell,
                                          double const symprec);
static VecDBL *get_translation_candidates(VecDBL const *pure_trans);
static VecDBL *collect_pure_translations(Symmetry const *symmetry);
static int get_primitive_in_translation_space(double t_mat[3][3],
                                              VecDBL const *pure_trans,
                                              int const symmetry_size,
                                              double const symprec);
static Symmetry *collect_primitive_symmetry(Symmetry const *symmetry,
                                            int const primsym_size);

/* return NULL if failed */
Primitive *prm_alloc_primitive(int const size) {
    Primitive *primitive;
    int i;

    primitive = NULL;

    if ((primitive = (Primitive *)malloc(sizeof(Primitive))) == NULL) {
        warning_memory("primitive");
        return NULL;
    }

    primitive->cell = NULL;
    primitive->mapping_table = NULL;
    primitive->size = size;
    primitive->tolerance = 0;
    primitive->angle_tolerance = -1.0;
    primitive->orig_lattice = NULL;

    if (size > 0) {
        if ((primitive->mapping_table = (int *)malloc(sizeof(int) * size)) ==
            NULL) {
            warning_memory("primitive->mapping_table");
            free(primitive);
            primitive = NULL;
            return NULL;
        }
    }

    for (i = 0; i < size; i++) {
        primitive->mapping_table[i] = -1;
    }

    return primitive;
}

void prm_free_primitive(Primitive *primitive) {
    if (primitive != NULL) {
        if (primitive->mapping_table != NULL) {
            free(primitive->mapping_table);
            primitive->mapping_table = NULL;
        }

        if (primitive->cell != NULL) {
            cel_free_cell(primitive->cell);
            primitive->cell = NULL;
        }

        if (primitive->orig_lattice != NULL) {
            free(primitive->orig_lattice);
            primitive->orig_lattice = NULL;
        }

        free(primitive);
    }
}

/* Return NULL if failed */
Primitive *prm_get_primitive(Cell const *cell, double const symprec,
                             double const angle_tolerance) {
    return get_primitive(cell, symprec, angle_tolerance);
}

/* Return primitive cell from given pure translations. */
/* If failed, return 0. */
int prm_get_primitive_with_pure_trans(Primitive *primitive, Cell const *cell,
                                      VecDBL const *pure_trans,
                                      double const symprec,
                                      double const angle_tolerance) {
    int i;

    if (pure_trans->size == 1) {
        if ((primitive->cell = get_cell_with_smallest_lattice(cell, symprec)) ==
            NULL) {
            return 0;
        }
        for (i = 0; i < cell->size; i++) {
            primitive->mapping_table[i] = i;
        }
    } else {
        if ((primitive->cell =
                 get_primitive_cell(primitive->mapping_table, cell, pure_trans,
                                    symprec, angle_tolerance)) == NULL) {
            return 0;
        }
    }

    primitive->tolerance = symprec;
    primitive->angle_tolerance = angle_tolerance;
    if ((primitive->orig_lattice =
             (double(*)[3])malloc(sizeof(double[3]) * 3)) == NULL) {
        warning_memory("primitive->orig_lattice");
        return 0;
    }
    mat_copy_matrix_d3(primitive->orig_lattice, cell->lattice);

    return 1;
}

// t_mat transforms primitive cell to conventional:
// (a_p, b_p, c_p) t_mat = (a_c, b_c, c_c)
// If failed, return NULL
Symmetry *prm_get_primitive_symmetry(double t_mat[3][3],
                                     Symmetry const *symmetry,
                                     double const symprec) {
    int i, primsym_size;
    VecDBL *pure_trans;
    Symmetry *prim_symmetry;
    double t_mat_inv[3][3], tmp_mat[3][3];

    pure_trans = NULL;
    prim_symmetry = NULL;

    if ((pure_trans = collect_pure_translations(symmetry)) == NULL) {
        return NULL;
    }
    primsym_size = symmetry->size / pure_trans->size;

    /* t_mat: T=(Lp^-1.L) where L is identity matrix. */
    if (get_primitive_in_translation_space(t_mat_inv, pure_trans,
                                           symmetry->size, symprec) == 0) {
        mat_free_VecDBL(pure_trans);
        pure_trans = NULL;
        return NULL;
    }

    mat_free_VecDBL(pure_trans);
    pure_trans = NULL;

    if (!mat_inverse_matrix_d3(t_mat, t_mat_inv, symprec)) {
        return NULL;
    }

    /* Collect operations for primitive cell from operations in 'symmetry' */
    if ((prim_symmetry = collect_primitive_symmetry(symmetry, primsym_size)) ==
        NULL) {
        return NULL;
    }

    /* Overwrite prim_symmetry by (T, 0) (R, t) (T, 0)^-1 = (T R T^-1, T t) */
    for (i = 0; i < prim_symmetry->size; i++) {
        mat_multiply_matrix_di3(tmp_mat, t_mat, prim_symmetry->rot[i]);
        mat_multiply_matrix_d3(tmp_mat, tmp_mat, t_mat_inv);
        mat_cast_matrix_3d_to_3i(prim_symmetry->rot[i], tmp_mat);
        mat_multiply_matrix_vector_d3(prim_symmetry->trans[i], t_mat,
                                      prim_symmetry->trans[i]);
    }

    for (i = 0; i < prim_symmetry->size; i++) {
        debug_print("--- %d ---\n", i + 1);
        for (int j = 0; j < 3; j++) {
            debug_print("%d %d %d\n", prim_symmetry->rot[i][j][0],
                        prim_symmetry->rot[i][j][1],
                        prim_symmetry->rot[i][j][2]);
        }
        debug_print("%f %f %f\n", prim_symmetry->trans[i][0],
                    prim_symmetry->trans[i][1], prim_symmetry->trans[i][2]);
    }

    return prim_symmetry;
}

int prm_get_primitive_lattice_vectors(double prim_lattice[3][3],
                                      Cell const *cell,
                                      VecDBL const *pure_trans,
                                      double const symprec,
                                      double const angle_tolerance) {
    return get_primitive_lattice_vectors(prim_lattice, cell, pure_trans,
                                         symprec, angle_tolerance);
}

/*=======*/
/* local */
/*=======*/

/* Return NULL if failed */
static Primitive *get_primitive(Cell const *cell, double const symprec,
                                double const angle_tolerance) {
    int attempt;
    double tolerance;
    Primitive *primitive;
    VecDBL *pure_trans;

    debug_print("get_primitive (tolerance = %f):\n", symprec);

    primitive = NULL;
    pure_trans = NULL;

    if ((primitive = prm_alloc_primitive(cell->size)) == NULL) {
        goto notfound;
    }

    tolerance = symprec;
    for (attempt = 0; attempt < NUM_ATTEMPT; attempt++) {
        debug_print("get_primitive (attempt = %d):\n", attempt);
        if ((pure_trans = sym_get_pure_translation(cell, tolerance)) != NULL) {
            if (prm_get_primitive_with_pure_trans(primitive, cell, pure_trans,
                                                  tolerance, angle_tolerance)) {
                goto found;
            }
        }

        mat_free_VecDBL(pure_trans);
        pure_trans = NULL;

        tolerance *= REDUCE_RATE;
        debug_print("spglib: Reduce tolerance to %f ", tolerance);
    }

    prm_free_primitive(primitive);
    primitive = NULL;

notfound:
    return NULL;

found:
    mat_free_VecDBL(pure_trans);
    pure_trans = NULL;
    return primitive;
}

/* Return NULL if failed */
static Cell *get_cell_with_smallest_lattice(Cell const *cell,
                                            double const symprec) {
    int i, j, aperiodic_axis;
    double min_lat[3][3], trans_mat[3][3], inv_lat[3][3];
    Cell *smallest_cell;

    debug_print("get_cell_with_smallest_lattice:\n");

    smallest_cell = NULL;

    aperiodic_axis = cell->aperiodic_axis;
    if ((aperiodic_axis == -1 &&
         !del_delaunay_reduce(min_lat, cell->lattice, symprec)) ||
        (aperiodic_axis != -1 &&
         !del_layer_delaunay_reduce(min_lat, cell->lattice, aperiodic_axis,
                                    symprec))) {
        return NULL;
    }

    mat_inverse_matrix_d3(inv_lat, min_lat, 0);
    mat_multiply_matrix_d3(trans_mat, inv_lat, cell->lattice);

    if ((smallest_cell = cel_alloc_cell(cell->size, cell->tensor_rank)) ==
        NULL) {
        return NULL;
    }

    mat_copy_matrix_d3(smallest_cell->lattice, min_lat);
    for (i = 0; i < cell->size; i++) {
        smallest_cell->types[i] = cell->types[i];
        mat_multiply_matrix_vector_d3(smallest_cell->position[i], trans_mat,
                                      cell->position[i]);
        for (j = 0; j < 3; j++) {
            if (j == aperiodic_axis) {
                smallest_cell->aperiodic_axis = j;
            } else {
                smallest_cell->position[i][j] =
                    mat_Dmod1(smallest_cell->position[i][j]);
            }
        }
    }

    return smallest_cell;
}

/* Return NULL if failed */
static Cell *get_primitive_cell(int *mapping_table, Cell const *cell,
                                VecDBL const *pure_trans, double const symprec,
                                double const angle_tolerance) {
    int multi;
    double prim_lattice[3][3];
    Cell *primitive_cell;

    debug_print("get_primitive_cell:\n");

    primitive_cell = NULL;

    /* Primitive lattice vectors are searched. */
    /* To be consistent, sometimes tolerance is decreased iteratively. */
    /* The decreased tolerance is stored in 'static double tolerance'. */
    multi = get_primitive_lattice_vectors(prim_lattice, cell, pure_trans,
                                          symprec, angle_tolerance);
    if (!multi) {
        goto not_found;
    }

    /* Fit atoms into new primitive cell */
    if ((primitive_cell = cel_trim_cell(mapping_table, prim_lattice, cell,
                                        symprec)) == NULL) {
        goto not_found;
    }

    /* found */
    return primitive_cell;

not_found:
    debug_print("spglib: Primitive cell could not be found\n");
    return NULL;
}

/* Return 0 if failed */
static int get_primitive_lattice_vectors(double prim_lattice[3][3],
                                         Cell const *cell,
                                         VecDBL const *pure_trans,
                                         double const symprec,
                                         double const angle_tolerance) {
    int i, multi, attempt;
    double tolerance;
    VecDBL *vectors, *pure_trans_reduced, *tmp_vec;

    vectors = NULL;
    pure_trans_reduced = NULL;
    tmp_vec = NULL;

    tolerance = symprec;

    if ((pure_trans_reduced = mat_alloc_VecDBL(pure_trans->size)) == NULL) {
        goto fail;
    }

    for (i = 0; i < pure_trans->size; i++) {
        mat_copy_vector_d3(pure_trans_reduced->vec[i], pure_trans->vec[i]);
    }

    for (attempt = 0; attempt < NUM_ATTEMPT; attempt++) {
        multi = pure_trans_reduced->size;

        if ((vectors = get_translation_candidates(pure_trans_reduced)) ==
            NULL) {
            mat_free_VecDBL(pure_trans_reduced);
            pure_trans_reduced = NULL;
            goto fail;
        }

        /* Lattice of primitive cell is found among pure translation vectors */
        if (find_primitive_lattice_vectors(prim_lattice, vectors, cell,
                                           tolerance)) {
            mat_free_VecDBL(vectors);
            vectors = NULL;
            mat_free_VecDBL(pure_trans_reduced);
            pure_trans_reduced = NULL;

            if ((cell->aperiodic_axis == -1 &&
                 !del_delaunay_reduce(prim_lattice, prim_lattice, symprec)) ||
                (cell->aperiodic_axis != -1 &&
                 !del_layer_delaunay_reduce(prim_lattice, prim_lattice,
                                            cell->aperiodic_axis, symprec))) {
                goto fail;
            }

            goto found;

        } else {
            if ((tmp_vec = mat_alloc_VecDBL(multi)) == NULL) {
                mat_free_VecDBL(vectors);
                vectors = NULL;
                mat_free_VecDBL(pure_trans_reduced);
                pure_trans_reduced = NULL;
                goto fail;
            }

            for (i = 0; i < multi; i++) {
                mat_copy_vector_d3(tmp_vec->vec[i], pure_trans_reduced->vec[i]);
            }
            mat_free_VecDBL(pure_trans_reduced);
            pure_trans_reduced = NULL;

            pure_trans_reduced = sym_reduce_pure_translation(
                cell, tmp_vec, tolerance, angle_tolerance);

            mat_free_VecDBL(tmp_vec);
            tmp_vec = NULL;
            mat_free_VecDBL(vectors);
            vectors = NULL;

            if (pure_trans_reduced == NULL) {
                goto fail;
            }

            debug_print("spglib: Tolerance is reduced to %f (%d), ", tolerance,
                        attempt);
            debug_print("num_pure_trans = %d\n", pure_trans_reduced->size);

            tolerance *= REDUCE_RATE;
        }
    }

    mat_free_VecDBL(pure_trans_reduced);
    pure_trans_reduced = NULL;

fail:
    return 0;

found:
    return multi;
}

/* Return 0 if failed */
static int find_primitive_lattice_vectors(double prim_lattice[3][3],
                                          VecDBL const *vectors,
                                          Cell const *cell,
                                          double const symprec) {
    int i, j, k, size, aperiodic_axis;
    double initial_volume, volume;
    double relative_lattice[3][3], min_vectors[3][3], tmp_lattice[3][3];
    double inv_mat_dbl[3][3];
    int inv_mat_int[3][3];

    debug_print("find_primitive_lattice_vectors:\n");

    size = vectors->size;
    initial_volume = fabs(mat_get_determinant_d3(cell->lattice));
    aperiodic_axis = cell->aperiodic_axis;

    /* check volumes of all possible lattices, find smallest volume */
    if (aperiodic_axis == -1) {
        for (i = 0; i < size; i++) {
            for (j = i + 1; j < size; j++) {
                for (k = j + 1; k < size; k++) {
                    mat_multiply_matrix_vector_d3(tmp_lattice[0], cell->lattice,
                                                  vectors->vec[i]);
                    mat_multiply_matrix_vector_d3(tmp_lattice[1], cell->lattice,
                                                  vectors->vec[j]);
                    mat_multiply_matrix_vector_d3(tmp_lattice[2], cell->lattice,
                                                  vectors->vec[k]);
                    volume = fabs(mat_get_determinant_d3(tmp_lattice));
                    if (volume > symprec) {
                        if ((int)round(initial_volume / volume) == size - 2) {
                            mat_copy_vector_d3(min_vectors[0], vectors->vec[i]);
                            mat_copy_vector_d3(min_vectors[1], vectors->vec[j]);
                            mat_copy_vector_d3(min_vectors[2], vectors->vec[k]);
                            goto ret;
                        }
                    }
                }
            }
        }
    } else {
        /* for layer, first move aperiodic axis to c */
        k = size + aperiodic_axis - 3;

        for (i = 0; i < size; i++) {
            for (j = i + 1; j < size; j++) {
                if (i != k && j != k) {
                    mat_multiply_matrix_vector_d3(tmp_lattice[0], cell->lattice,
                                                  vectors->vec[i]);
                    mat_multiply_matrix_vector_d3(tmp_lattice[1], cell->lattice,
                                                  vectors->vec[j]);
                    mat_multiply_matrix_vector_d3(tmp_lattice[2], cell->lattice,
                                                  vectors->vec[k]);
                    volume = fabs(mat_get_determinant_d3(tmp_lattice));
                    if (volume > symprec) {
                        if ((int)round(initial_volume / volume) == size - 2) {
                            mat_copy_vector_d3(min_vectors[0], vectors->vec[i]);
                            mat_copy_vector_d3(min_vectors[1], vectors->vec[j]);
                            /* move aperiodic axis back */
                            if (aperiodic_axis == 2) {
                                mat_copy_vector_d3(min_vectors[2],
                                                   vectors->vec[k]);
                            } else {
                                mat_copy_vector_d3(min_vectors[2],
                                                   min_vectors[aperiodic_axis]);
                                mat_copy_vector_d3(min_vectors[aperiodic_axis],
                                                   vectors->vec[k]);
                            }
                            goto ret;
                        }
                    }
                }
            }
        }
    }

    /* Not found */
    debug_print("spglib: Primitive lattice vectors could not be found\n");
    return 0;

    /* Found */
ret:
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            relative_lattice[j][i] = min_vectors[i][j];
        }
    }

    mat_inverse_matrix_d3(inv_mat_dbl, relative_lattice, 0);
    mat_cast_matrix_3d_to_3i(inv_mat_int, inv_mat_dbl);
    if (abs(mat_get_determinant_i3(inv_mat_int)) == size - 2) {
        mat_cast_matrix_3i_to_3d(inv_mat_dbl, inv_mat_int);
        mat_inverse_matrix_d3(relative_lattice, inv_mat_dbl, 0);
    } else {
        warning_print("spglib: Primitive lattice cleaning is incomplete\n");
    }
    mat_multiply_matrix_d3(prim_lattice, cell->lattice, relative_lattice);

    return 1;
}

static VecDBL *get_translation_candidates(VecDBL const *pure_trans) {
    int i, j, multi;
    VecDBL *vectors;

    vectors = NULL;
    multi = pure_trans->size;

    if ((vectors = mat_alloc_VecDBL(multi + 2)) == NULL) {
        return NULL;
    }

    /* store pure translations in original cell */
    /* as trial primitive lattice vectors */
    for (i = 0; i < multi - 1; i++) {
        mat_copy_vector_d3(vectors->vec[i], pure_trans->vec[i + 1]);
    }

    /* store lattice translations of original cell */
    /* as trial primitive lattice vectors */
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (i == j) {
                vectors->vec[i + multi - 1][j] = 1;
            } else {
                vectors->vec[i + multi - 1][j] = 0;
            }
        }
    }

    return vectors;
}

static VecDBL *collect_pure_translations(Symmetry const *symmetry) {
    int i, num_pure_trans;
    VecDBL *pure_trans;
    VecDBL *ret_pure_trans;
    static int identity[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    num_pure_trans = 0;
    pure_trans = NULL;
    ret_pure_trans = NULL;

    if ((pure_trans = mat_alloc_VecDBL(symmetry->size)) == NULL) {
        return NULL;
    }

    for (i = 0; i < symmetry->size; i++) {
        if (mat_check_identity_matrix_i3(symmetry->rot[i], identity)) {
            mat_copy_vector_d3(pure_trans->vec[num_pure_trans],
                               symmetry->trans[i]);
            num_pure_trans++;
        }
    }

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

static int get_primitive_in_translation_space(double t_mat_inv[3][3],
                                              VecDBL const *pure_trans,
                                              int const symmetry_size,
                                              double const symprec) {
    int i, j, primsym_size;
    Primitive *primitive;
    Cell *cell;

    cell = NULL;
    primitive = NULL;

    if ((cell = cel_alloc_cell(pure_trans->size, NOSPIN)) == NULL) {
        return 0;
    }

    primsym_size = symmetry_size / pure_trans->size;
    if (symmetry_size != primsym_size * pure_trans->size) {
        cel_free_cell(cell);
        cell = NULL;
        return 0;
    }

    for (i = 0; i < pure_trans->size; i++) {
        cell->types[i] = 1;
        for (j = 0; j < 3; j++) {
            cell->position[i][j] = pure_trans->vec[i][j];
        }
    }

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (i == j) {
                cell->lattice[i][j] = 1;
            } else {
                cell->lattice[i][j] = 0;
            }
        }
    }

    primitive = get_primitive(cell, symprec, -1.0);
    cel_free_cell(cell);
    cell = NULL;

    if (primitive->cell->size != 1) {
        prm_free_primitive(primitive);
        primitive = NULL;
        return 0;
    }

    mat_copy_matrix_d3(t_mat_inv, primitive->cell->lattice);
    prm_free_primitive(primitive);
    primitive = NULL;

    return 1;
}

static Symmetry *collect_primitive_symmetry(Symmetry const *symmetry,
                                            int const primsym_size) {
    int i, j, num_psym, is_found;
    Symmetry *prim_symmetry;

    prim_symmetry = NULL;

    prim_symmetry = sym_alloc_symmetry(primsym_size);
    num_psym = 1;
    mat_copy_matrix_i3(prim_symmetry->rot[0], symmetry->rot[0]);
    mat_copy_vector_d3(prim_symmetry->trans[0], symmetry->trans[0]);
    for (i = 1; i < symmetry->size; i++) {
        is_found = 1;
        for (j = 0; j < num_psym; j++) {
            if (mat_check_identity_matrix_i3(prim_symmetry->rot[j],
                                             symmetry->rot[i])) {
                is_found = 0;
                break;
            }
        }
        if (is_found) {
            if (num_psym == primsym_size) {
                sym_free_symmetry(prim_symmetry);
                prim_symmetry = NULL;
                break;
            }
            mat_copy_matrix_i3(prim_symmetry->rot[num_psym], symmetry->rot[i]);
            mat_copy_vector_d3(prim_symmetry->trans[num_psym],
                               symmetry->trans[i]);
            num_psym++;
        }
    }

    if (prim_symmetry == NULL) {
        return NULL;
    }

    if (num_psym != primsym_size) {
        sym_free_symmetry(prim_symmetry);
        prim_symmetry = NULL;
        return NULL;
    }

    return prim_symmetry;
}
