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

#include "cell.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "mathfunc.h"

#define INCREASE_RATE 2.0
#define REDUCE_RATE 0.95
#define NUM_ATTEMPT 100

static Cell *trim_cell(int *mapping_table, const double trimmed_lattice[3][3],
                       const Cell *cell, const double symprec);
static void set_positions_and_tensors(Cell *trimmed_cell,
                                      const VecDBL *position,
                                      const SiteTensorType tensor_rank,
                                      const double *tensors,
                                      const int *mapping_table,
                                      const int *overlap_table);
static VecDBL *translate_atoms_in_trimmed_lattice(const Cell *cell,
                                                  const double prim_lat[3][3]);
static int *get_overlap_table(const VecDBL *position, const int cell_size,
                              const int *cell_types, const Cell *trimmed_cell,
                              const double symprec);

// @brief Allocate Cell. NULL is returned if failed
// @param size number of atoms
// @param tensor_rank rank of site tensors for magnetic symmetry. Set -1 if not
// used.
Cell *cel_alloc_cell(const int size, const SiteTensorType tensor_rank) {
    Cell *cell;

    cell = NULL;

    if (size < 1) {
        return NULL;
    }

    if ((cell = (Cell *)malloc(sizeof(Cell))) == NULL) {
        goto fail;
    }

    if ((cell->lattice = (double(*)[3])malloc(sizeof(double[3]) * 3)) == NULL) {
        goto fail;
    }

    cell->size = size;

    cell->aperiodic_axis = -1;

    if ((cell->types = (int *)malloc(sizeof(int) * size)) == NULL) {
        goto fail;
    }
    if ((cell->position = (double(*)[3])malloc(sizeof(double[3]) * size)) ==
        NULL) {
        goto fail;
    }

    cell->tensor_rank = tensor_rank;
    if (tensor_rank == COLLINEAR) {
        // Collinear case
        if ((cell->tensors = (double *)malloc(sizeof(double) * size)) == NULL) {
            goto fail;
        }
    }
    if (tensor_rank == NONCOLLINEAR) {
        // Non-collinear case
        if ((cell->tensors = (double *)malloc(sizeof(double) * size * 3)) ==
            NULL) {
            goto fail;
        }
    }

    return cell;

fail:
    warning_print("spglib: Memory could not be allocated.");
    cel_free_cell(cell);
    cell = NULL;
    return NULL;
}

void cel_free_cell(Cell *cell) {
    if (cell != NULL) {
        if (cell->lattice != NULL) {
            free(cell->lattice);
            cell->lattice = NULL;
        }
        if (cell->position != NULL) {
            free(cell->position);
            cell->position = NULL;
        }
        if (cell->types != NULL) {
            free(cell->types);
            cell->types = NULL;
        }
        if ((cell->tensor_rank != NOSPIN) && (cell->tensors != NULL)) {
            // When cell->tensor_rank==NOSPIN, cell->tensors is already NULL.
            free(cell->tensors);
            cell->tensors = NULL;
        }
        free(cell);
    }
}

void cel_set_cell(Cell *cell, const double lattice[3][3],
                  const double position[][3], const int types[]) {
    int i, j;
    mat_copy_matrix_d3(cell->lattice, lattice);
    for (i = 0; i < cell->size; i++) {
        for (j = 0; j < 3; j++) {
            cell->position[i][j] = position[i][j] - mat_Nint(position[i][j]);
        }
        cell->types[i] = types[i];
    }
}

/* aperiodic_axis = -1 for none; = 0 1 2 for a b c */
void cel_set_layer_cell(Cell *cell, const double lattice[3][3],
                        const double position[][3], const int types[],
                        const int aperiodic_axis) {
    int i, j;
    mat_copy_matrix_d3(cell->lattice, lattice);
    for (i = 0; i < cell->size; i++) {
        for (j = 0; j < 3; j++) {
            if (j != aperiodic_axis) {
                cell->position[i][j] =
                    position[i][j] - mat_Nint(position[i][j]);
            } else {
                cell->position[i][j] = position[i][j];
            }
        }
        cell->types[i] = types[i];
    }
    cell->aperiodic_axis = aperiodic_axis;
}

void cel_set_cell_with_tensors(Cell *cell, const double lattice[3][3],
                               const double position[][3], const int types[],
                               const double *tensors) {
    int i, j;

    cel_set_cell(cell, lattice, position, types);
    for (i = 0; i < cell->size; i++) {
        if (cell->tensor_rank == COLLINEAR) {
            cell->tensors[i] = tensors[i];
        } else if (cell->tensor_rank == NONCOLLINEAR) {
            for (j = 0; j < 3; j++) {
                cell->tensors[i * 3 + j] = tensors[i * 3 + j];
            }
        }
    }
}

Cell *cel_copy_cell(const Cell *cell) {
    Cell *cell_new;

    cell_new = NULL;

    if ((cell_new = cel_alloc_cell(cell->size, cell->tensor_rank)) == NULL) {
        return NULL;
    }

    if (cell->aperiodic_axis != -1) {
        cel_set_layer_cell(cell_new, cell->lattice, cell->position, cell->types,
                           cell->aperiodic_axis);
    } else if (cell->tensor_rank == NOSPIN) {
        cel_set_cell(cell_new, cell->lattice, cell->position, cell->types);
    } else {
        cel_set_cell_with_tensors(cell_new, cell->lattice, cell->position,
                                  cell->types, cell->tensors);
    }

    return cell_new;
}

int cel_is_overlap(const double a[3], const double b[3],
                   const double lattice[3][3], const double symprec) {
    int i;
    double v_diff[3];

    for (i = 0; i < 3; i++) {
        v_diff[i] = a[i] - b[i];
        v_diff[i] -= mat_Nint(v_diff[i]);
    }

    mat_multiply_matrix_vector_d3(v_diff, lattice, v_diff);
    if (sqrt(mat_norm_squared_d3(v_diff)) < symprec) {
        return 1;
    } else {
        return 0;
    }
}

int cel_is_overlap_with_same_type(const double a[3], const double b[3],
                                  const int type_a, const int type_b,
                                  const double lattice[3][3],
                                  const double symprec) {
    if (type_a == type_b) {
        return cel_is_overlap(a, b, lattice, symprec);
    } else {
        return 0;
    }
}

/* 1: At least one overlap of a pair of atoms was found. */
/* 0: No overlap of atoms was found. */
int cel_any_overlap(const Cell *cell, const double symprec) {
    int i, j;

    for (i = 0; i < cell->size; i++) {
        for (j = i + 1; j < cell->size; j++) {
            if (cel_is_overlap(cell->position[i], cell->position[j],
                               cell->lattice, symprec)) {
                return 1;
            }
        }
    }
    return 0;
}

/* 1: At least one overlap of a pair of atoms with same type was found. */
/* 0: No overlap of atoms was found. */
int cel_any_overlap_with_same_type(const Cell *cell, const double symprec) {
    int i, j;

    for (i = 0; i < cell->size; i++) {
        for (j = i + 1; j < cell->size; j++) {
            if (cel_is_overlap_with_same_type(
                    cell->position[i], cell->position[j], cell->types[i],
                    cell->types[j], cell->lattice, symprec)) {
                return 1;
            }
        }
    }
    return 0;
}

/* Modified from cel_is_overlap */
/* Periodic boundary condition only applied on 2 directions */
int cel_layer_is_overlap(const double a[3], const double b[3],
                         const double lattice[3][3], const int periodic_axes[2],
                         const double symprec) {
    double v_diff[3];

    v_diff[0] = a[0] - b[0];
    v_diff[1] = a[1] - b[1];
    v_diff[2] = a[2] - b[2];

    v_diff[periodic_axes[0]] -= mat_Nint(v_diff[periodic_axes[0]]);
    v_diff[periodic_axes[1]] -= mat_Nint(v_diff[periodic_axes[1]]);

    mat_multiply_matrix_vector_d3(v_diff, lattice, v_diff);
    if (sqrt(mat_norm_squared_d3(v_diff)) < symprec) {
        return 1;
    } else {
        return 0;
    }
}

int cel_layer_is_overlap_with_same_type(const double a[3], const double b[3],
                                        const int type_a, const int type_b,
                                        const double lattice[3][3],
                                        const int periodic_axes[2],
                                        const double symprec) {
    if (type_a == type_b) {
        return cel_layer_is_overlap(a, b, lattice, periodic_axes, symprec);
    } else {
        return 0;
    }
}

/* 1: At least one overlap of a pair of atoms with same type was found. */
/* 0: No overlap of atoms was found. */
int cel_layer_any_overlap_with_same_type(const Cell *cell,
                                         const int periodic_axes[2],
                                         const double symprec) {
    int i, j;

    for (i = 0; i < cell->size; i++) {
        for (j = i + 1; j < cell->size; j++) {
            if (cel_layer_is_overlap_with_same_type(
                    cell->position[i], cell->position[j], cell->types[i],
                    cell->types[j], cell->lattice, periodic_axes, symprec)) {
                return 1;
            }
        }
    }
    return 0;
}

/// @param[out] mapping_table array (cell->size, ), maps atom-`i` in cell to
/// non-overlapped atom-`mapping_table[i]`
/// @param[in] trimmed_lattice
/// @param[in] cell
/// @param[in] symprec
/// @return trimmed_cell
Cell *cel_trim_cell(int *mapping_table, const double trimmed_lattice[3][3],
                    const Cell *cell, const double symprec) {
    return trim_cell(mapping_table, trimmed_lattice, cell, symprec);
}

/* Return NULL if failed */
static Cell *trim_cell(int *mapping_table, const double trimmed_lattice[3][3],
                       const Cell *cell, const double symprec) {
    int i, index_atom, ratio;
    Cell *trimmed_cell;
    VecDBL *position;
    int *overlap_table;
    int tmp_mat_int[3][3];
    double tmp_mat[3][3];

    debug_print("trim_cell\n");

    position = NULL;
    overlap_table = NULL;
    trimmed_cell = NULL;

    ratio = abs(mat_Nint(mat_get_determinant_d3(cell->lattice) /
                         mat_get_determinant_d3(trimmed_lattice)));

    mat_inverse_matrix_d3(tmp_mat, trimmed_lattice, symprec);
    mat_multiply_matrix_d3(tmp_mat, tmp_mat, cell->lattice);
    mat_cast_matrix_3d_to_3i(tmp_mat_int, tmp_mat);
    if (abs(mat_get_determinant_i3(tmp_mat_int)) != ratio) {
        warning_print(
            "spglib: Determinant of change of basis matrix "
            "has to be same as volume ratio (line %d, %s).\n",
            __LINE__, __FILE__);
        goto err;
    }

    /* Check if cell->size is dividable by ratio */
    if ((cell->size / ratio) * ratio != cell->size) {
        warning_print("spglib: atom number ratio is inconsistent.\n");
        warning_print(" (line %d, %s).\n", __LINE__, __FILE__);
        goto err;
    }

    if ((trimmed_cell =
             cel_alloc_cell(cell->size / ratio, cell->tensor_rank)) == NULL) {
        goto err;
    }

    if ((position = translate_atoms_in_trimmed_lattice(
             cell, trimmed_lattice)) == NULL) {
        warning_print("spglib: translate_atoms_in_trimmed_lattice failed.\n");
        warning_print(" (line %d, %s).\n", __LINE__, __FILE__);
        cel_free_cell(trimmed_cell);
        trimmed_cell = NULL;
        goto err;
    }

    mat_copy_matrix_d3(trimmed_cell->lattice, trimmed_lattice);
    /* aperiodic axis of trimmed_cell is directly copied from cell */
    trimmed_cell->aperiodic_axis = cell->aperiodic_axis;

    if ((overlap_table = get_overlap_table(position, cell->size, cell->types,
                                           trimmed_cell, symprec)) == NULL) {
        warning_print("spglib: get_overlap_table failed.\n");
        warning_print(" (line %d, %s).\n", __LINE__, __FILE__);
        mat_free_VecDBL(position);
        position = NULL;
        cel_free_cell(trimmed_cell);
        trimmed_cell = NULL;
        goto err;
    }

    index_atom = 0;
    for (i = 0; i < cell->size; i++) {
        if (overlap_table[i] == i) {
            mapping_table[i] = index_atom;
            trimmed_cell->types[index_atom] = cell->types[i];
            index_atom++;
        } else {
            mapping_table[i] = mapping_table[overlap_table[i]];
        }
    }

    set_positions_and_tensors(trimmed_cell, position, cell->tensor_rank,
                              cell->tensors, mapping_table, overlap_table);

    mat_free_VecDBL(position);
    position = NULL;
    free(overlap_table);

    return trimmed_cell;

err:
    return NULL;
}

/// @brief Set trimmed_cell->position and trimmed_cell->tensors by averaging
/// over overlapped atoms
static void set_positions_and_tensors(Cell *trimmed_cell,
                                      const VecDBL *position,
                                      const SiteTensorType tensor_rank,
                                      const double *tensors,
                                      const int *mapping_table,
                                      const int *overlap_table) {
    int i, j, k, l, multi, atom_idx;

    for (i = 0; i < trimmed_cell->size; i++) {
        for (j = 0; j < 3; j++) {
            trimmed_cell->position[i][j] = 0;
        }

        if (tensor_rank == COLLINEAR) {
            trimmed_cell->tensors[i] = 0;
        } else if (tensor_rank == NONCOLLINEAR) {
            for (j = 0; j < 3; j++) {
                trimmed_cell->tensors[3 * i + j] = 0;
            }
        }
    }

    /* Positions of overlapped atoms are averaged. */
    for (i = 0; i < position->size; i++) {
        j = mapping_table[i];
        k = overlap_table[i];
        for (l = 0; l < 3; l++) {
            /* boundary treatment */
            /* One is at right and one is at left or vice versa. */
            if (mat_Dabs(position->vec[k][l] - position->vec[i][l]) > 0.5) {
                if (position->vec[i][l] < position->vec[k][l]) {
                    trimmed_cell->position[j][l] += position->vec[i][l] + 1;
                } else {
                    trimmed_cell->position[j][l] += position->vec[i][l] - 1;
                }
            } else {
                trimmed_cell->position[j][l] += position->vec[i][l];
            }
        }
    }

    /* Site tensors of overlapped atoms are averaged. */
    for (i = 0; i < position->size; i++) {
        atom_idx = mapping_table[i];
        if (tensor_rank == COLLINEAR) {
            trimmed_cell->tensors[atom_idx] += tensors[i];
        } else if (tensor_rank == NONCOLLINEAR) {
            for (j = 0; j < 3; j++) {
                trimmed_cell->tensors[3 * atom_idx + j] += tensors[3 * i + j];
            }
        }
    }

    multi = position->size / trimmed_cell->size;
    for (i = 0; i < trimmed_cell->size; i++) {
        for (j = 0; j < 3; j++) {
            trimmed_cell->position[i][j] /= multi;
            if (j != trimmed_cell->aperiodic_axis) {
                trimmed_cell->position[i][j] =
                    mat_Dmod1(trimmed_cell->position[i][j]);
            }
        }

        if (tensor_rank == COLLINEAR) {
            trimmed_cell->tensors[i] /= multi;
        } else if (tensor_rank == NONCOLLINEAR) {
            for (j = 0; j < 3; j++) {
                trimmed_cell->tensors[3 * i + j] /= multi;
            }
        }
    }
}

/* Return NULL if failed */
static VecDBL *translate_atoms_in_trimmed_lattice(
    const Cell *cell, const double trimmed_lattice[3][3]) {
    int i, j;
    double tmp_matrix[3][3], axis_inv[3][3];
    VecDBL *position;

    position = NULL;

    if ((position = mat_alloc_VecDBL(cell->size)) == NULL) {
        return NULL;
    }

    mat_inverse_matrix_d3(tmp_matrix, trimmed_lattice, 0);
    mat_multiply_matrix_d3(axis_inv, tmp_matrix, cell->lattice);

    /* Send atoms into the trimmed cell */
    for (i = 0; i < cell->size; i++) {
        mat_multiply_matrix_vector_d3(position->vec[i], axis_inv,
                                      cell->position[i]);
        for (j = 0; j < 3; j++) {
            if (j != cell->aperiodic_axis) {
                position->vec[i][j] = mat_Dmod1(position->vec[i][j]);
            }
        }
    }

    return position;
}

/* Return NULL if failed */
static int *get_overlap_table(const VecDBL *position, const int cell_size,
                              const int *cell_types, const Cell *trimmed_cell,
                              const double symprec) {
    int i, j, attempt, num_overlap, ratio, lattice_rank;
    double trim_tolerance;
    int *overlap_table;
    int periodic_axes[3];
    /* use periodic_axes to avoid for loop in cel_layer_is_overlap */
    lattice_rank = 0;
    for (i = 0; i < 3; i++) {
        if (i != trimmed_cell->aperiodic_axis) {
            periodic_axes[lattice_rank] = i;
            lattice_rank++;
        }
    }

    trim_tolerance = symprec;

    ratio = cell_size / trimmed_cell->size;

    if ((overlap_table = (int *)malloc(sizeof(int) * cell_size)) == NULL) {
        return NULL;
    }

    for (attempt = 0; attempt < NUM_ATTEMPT; attempt++) {
        for (i = 0; i < cell_size; i++) {
            overlap_table[i] = i;
            for (j = 0; j < cell_size; j++) {
                if (cell_types[i] == cell_types[j]) {
                    if ((lattice_rank == 3 &&
                         cel_is_overlap(position->vec[i], position->vec[j],
                                        trimmed_cell->lattice,
                                        trim_tolerance)) ||
                        (lattice_rank == 2 &&
                         cel_layer_is_overlap(position->vec[i],
                                              position->vec[j],
                                              trimmed_cell->lattice,
                                              periodic_axes, trim_tolerance))) {
                        if (overlap_table[j] == j) {
                            overlap_table[i] = j;
                            break;
                        }
                    }
                }
            }
        }

        for (i = 0; i < cell_size; i++) {
            if (overlap_table[i] != i) {
                continue;
            }

            num_overlap = 0;
            for (j = 0; j < cell_size; j++) {
                if (i == overlap_table[j]) {
                    num_overlap++;
                }
            }

            if (num_overlap == ratio) {
                continue;
            }

            if (num_overlap < ratio) {
                trim_tolerance *= INCREASE_RATE;
                warning_print("spglib: Increase tolerance to %f ",
                              trim_tolerance);
                warning_print("(line %d, %s).\n", __LINE__, __FILE__);
                goto cont;
            }
            if (num_overlap > ratio) {
                trim_tolerance *= REDUCE_RATE;
                warning_print("spglib: Reduce tolerance to %f ",
                              trim_tolerance);
                warning_print("(%d) ", attempt);
                warning_print("(line %d, %s).\n", __LINE__, __FILE__);
                goto cont;
            }
        }

        goto found;

    cont:;
    }

    warning_print("spglib: Could not trim cell well ");
    warning_print("(line %d, %s).\n", __LINE__, __FILE__);
    free(overlap_table);
    overlap_table = NULL;

found:
    return overlap_table;
}
