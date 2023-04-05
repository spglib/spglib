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

#include "kpoint.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "kgrid.h"
#include "mathfunc.h"

#ifdef KPTWARNING
#include <stdio.h>
#define warning_print(...) fprintf(stderr, __VA_ARGS__)
#else
#define warning_print(...)
#endif

#define KPT_NUM_BZ_SEARCH_SPACE 125

static int bz_search_space[KPT_NUM_BZ_SEARCH_SPACE][3] = {
    {0, 0, 0},   {0, 0, 1},   {0, 0, 2},   {0, 0, -2},   {0, 0, -1},
    {0, 1, 0},   {0, 1, 1},   {0, 1, 2},   {0, 1, -2},   {0, 1, -1},
    {0, 2, 0},   {0, 2, 1},   {0, 2, 2},   {0, 2, -2},   {0, 2, -1},
    {0, -2, 0},  {0, -2, 1},  {0, -2, 2},  {0, -2, -2},  {0, -2, -1},
    {0, -1, 0},  {0, -1, 1},  {0, -1, 2},  {0, -1, -2},  {0, -1, -1},
    {1, 0, 0},   {1, 0, 1},   {1, 0, 2},   {1, 0, -2},   {1, 0, -1},
    {1, 1, 0},   {1, 1, 1},   {1, 1, 2},   {1, 1, -2},   {1, 1, -1},
    {1, 2, 0},   {1, 2, 1},   {1, 2, 2},   {1, 2, -2},   {1, 2, -1},
    {1, -2, 0},  {1, -2, 1},  {1, -2, 2},  {1, -2, -2},  {1, -2, -1},
    {1, -1, 0},  {1, -1, 1},  {1, -1, 2},  {1, -1, -2},  {1, -1, -1},
    {2, 0, 0},   {2, 0, 1},   {2, 0, 2},   {2, 0, -2},   {2, 0, -1},
    {2, 1, 0},   {2, 1, 1},   {2, 1, 2},   {2, 1, -2},   {2, 1, -1},
    {2, 2, 0},   {2, 2, 1},   {2, 2, 2},   {2, 2, -2},   {2, 2, -1},
    {2, -2, 0},  {2, -2, 1},  {2, -2, 2},  {2, -2, -2},  {2, -2, -1},
    {2, -1, 0},  {2, -1, 1},  {2, -1, 2},  {2, -1, -2},  {2, -1, -1},
    {-2, 0, 0},  {-2, 0, 1},  {-2, 0, 2},  {-2, 0, -2},  {-2, 0, -1},
    {-2, 1, 0},  {-2, 1, 1},  {-2, 1, 2},  {-2, 1, -2},  {-2, 1, -1},
    {-2, 2, 0},  {-2, 2, 1},  {-2, 2, 2},  {-2, 2, -2},  {-2, 2, -1},
    {-2, -2, 0}, {-2, -2, 1}, {-2, -2, 2}, {-2, -2, -2}, {-2, -2, -1},
    {-2, -1, 0}, {-2, -1, 1}, {-2, -1, 2}, {-2, -1, -2}, {-2, -1, -1},
    {-1, 0, 0},  {-1, 0, 1},  {-1, 0, 2},  {-1, 0, -2},  {-1, 0, -1},
    {-1, 1, 0},  {-1, 1, 1},  {-1, 1, 2},  {-1, 1, -2},  {-1, 1, -1},
    {-1, 2, 0},  {-1, 2, 1},  {-1, 2, 2},  {-1, 2, -2},  {-1, 2, -1},
    {-1, -2, 0}, {-1, -2, 1}, {-1, -2, 2}, {-1, -2, -2}, {-1, -2, -1},
    {-1, -1, 0}, {-1, -1, 1}, {-1, -1, 2}, {-1, -1, -2}, {-1, -1, -1}};

static MatINT *get_point_group_reciprocal(const MatINT *rotations,
                                          const int is_time_reversal);
static MatINT *get_point_group_reciprocal_with_q(const MatINT *rot_reciprocal,
                                                 const double symprec,
                                                 const size_t num_q,
                                                 const double qpoints[][3]);
static size_t get_dense_ir_reciprocal_mesh(int grid_address[][3],
                                           size_t ir_mapping_table[],
                                           const int mesh[3],
                                           const int is_shift[3],
                                           const MatINT *rot_reciprocal);
static size_t get_dense_ir_reciprocal_mesh_normal(int grid_address[][3],
                                                  size_t ir_mapping_table[],
                                                  const int mesh[3],
                                                  const int is_shift[3],
                                                  const MatINT *rot_reciprocal);
static size_t get_dense_ir_reciprocal_mesh_distortion(
    int grid_address[][3], size_t ir_mapping_table[], const int mesh[3],
    const int is_shift[3], const MatINT *rot_reciprocal);
static size_t get_dense_num_ir(size_t ir_mapping_table[], const int mesh[3]);
static size_t relocate_dense_BZ_grid_address(
    int bz_grid_address[][3], size_t bz_map[], const int grid_address[][3],
    const int mesh[3], const double rec_lattice[3][3], const int is_shift[3]);
static double get_tolerance_for_BZ_reduction(const double rec_lattice[3][3],
                                             const int mesh[3]);
static int check_mesh_symmetry(const int mesh[3], const int is_shift[3],
                               const MatINT *rot_reciprocal);

/* grid_address (e.g. 4x4x4 mesh, unless GRID_ORDER_XYZ is defined) */
/*    [[ 0  0  0]                                                   */
/*     [ 1  0  0]                                                   */
/*     [ 2  0  0]                                                   */
/*     [-1  0  0]                                                   */
/*     [ 0  1  0]                                                   */
/*     [ 1  1  0]                                                   */
/*     [ 2  1  0]                                                   */
/*     [-1  1  0]                                                   */
/*     ....      ]                                                  */
/*                                                                  */
/* Each value of 'map' corresponds to the index of grid_point.      */
int kpt_get_irreducible_reciprocal_mesh(int grid_address[][3],
                                        int ir_mapping_table[],
                                        const int mesh[3],
                                        const int is_shift[3],
                                        const MatINT *rot_reciprocal) {
    int num_ir;
    size_t i;
    size_t *dense_ir_mapping_table;

    if ((dense_ir_mapping_table = (size_t *)malloc(
             sizeof(size_t) * mesh[0] * mesh[1] * mesh[2])) == NULL) {
        warning_print("spglib: Memory of unique_rot could not be allocated.");
        return 0;
    }

    num_ir = kpt_get_dense_irreducible_reciprocal_mesh(
        grid_address, dense_ir_mapping_table, mesh, is_shift, rot_reciprocal);

    for (i = 0; i < mesh[0] * mesh[1] * mesh[2]; i++) {
        ir_mapping_table[i] = dense_ir_mapping_table[i];
    }

    free(dense_ir_mapping_table);
    dense_ir_mapping_table = NULL;

    return num_ir;
}

size_t kpt_get_dense_irreducible_reciprocal_mesh(int grid_address[][3],
                                                 size_t ir_mapping_table[],
                                                 const int mesh[3],
                                                 const int is_shift[3],
                                                 const MatINT *rot_reciprocal) {
    size_t num_ir;

    num_ir = get_dense_ir_reciprocal_mesh(grid_address, ir_mapping_table, mesh,
                                          is_shift, rot_reciprocal);

    return num_ir;
}

int kpt_get_stabilized_reciprocal_mesh(
    int grid_address[][3], int ir_mapping_table[], const int mesh[3],
    const int is_shift[3], const int is_time_reversal, const MatINT *rotations,
    const size_t num_q, const double qpoints[][3]) {
    int num_ir;
    size_t i;
    size_t *dense_ir_mapping_table;

    if ((dense_ir_mapping_table = (size_t *)malloc(
             sizeof(size_t) * mesh[0] * mesh[1] * mesh[2])) == NULL) {
        warning_print("spglib: Memory of unique_rot could not be allocated.");
        return 0;
    }

    num_ir = kpt_get_dense_stabilized_reciprocal_mesh(
        grid_address, dense_ir_mapping_table, mesh, is_shift, is_time_reversal,
        rotations, num_q, qpoints);

    for (i = 0; i < mesh[0] * mesh[1] * mesh[2]; i++) {
        ir_mapping_table[i] = dense_ir_mapping_table[i];
    }

    free(dense_ir_mapping_table);
    dense_ir_mapping_table = NULL;

    return num_ir;
}

size_t kpt_get_dense_stabilized_reciprocal_mesh(
    int grid_address[][3], size_t ir_mapping_table[], const int mesh[3],
    const int is_shift[3], const int is_time_reversal, const MatINT *rotations,
    const size_t num_q, const double qpoints[][3]) {
    size_t num_ir;
    MatINT *rot_reciprocal, *rot_reciprocal_q;
    double tolerance;

    rot_reciprocal = NULL;
    rot_reciprocal_q = NULL;

    rot_reciprocal = get_point_group_reciprocal(rotations, is_time_reversal);
    tolerance = 0.01 / (mesh[0] + mesh[1] + mesh[2]);
    rot_reciprocal_q = get_point_group_reciprocal_with_q(
        rot_reciprocal, tolerance, num_q, qpoints);

    num_ir = get_dense_ir_reciprocal_mesh(grid_address, ir_mapping_table, mesh,
                                          is_shift, rot_reciprocal_q);

    mat_free_MatINT(rot_reciprocal_q);
    rot_reciprocal_q = NULL;
    mat_free_MatINT(rot_reciprocal);
    rot_reciprocal = NULL;
    return num_ir;
}

void kpt_get_dense_grid_points_by_rotations(size_t rot_grid_points[],
                                            const int address_orig[3],
                                            const int (*rot_reciprocal)[3][3],
                                            const int num_rot,
                                            const int mesh[3],
                                            const int is_shift[3]) {
    int i;
    int address_double_orig[3], address_double[3];

    for (i = 0; i < 3; i++) {
        address_double_orig[i] = address_orig[i] * 2 + is_shift[i];
    }
    for (i = 0; i < num_rot; i++) {
        mat_multiply_matrix_vector_i3(address_double, rot_reciprocal[i],
                                      address_double_orig);
        rot_grid_points[i] =
            kgd_get_dense_grid_point_double_mesh(address_double, mesh);
    }
}

void kpt_get_dense_BZ_grid_points_by_rotations(
    size_t rot_grid_points[], const int address_orig[3],
    const int (*rot_reciprocal)[3][3], const int num_rot, const int mesh[3],
    const int is_shift[3], const size_t bz_map[]) {
    int i;
    int address_double_orig[3], address_double[3], bzmesh[3];

    for (i = 0; i < 3; i++) {
        bzmesh[i] = mesh[i] * 2;
        address_double_orig[i] = address_orig[i] * 2 + is_shift[i];
    }
    for (i = 0; i < num_rot; i++) {
        mat_multiply_matrix_vector_i3(address_double, rot_reciprocal[i],
                                      address_double_orig);
        rot_grid_points[i] = bz_map[kgd_get_dense_grid_point_double_mesh(
            address_double, bzmesh)];
    }
}

int kpt_relocate_BZ_grid_address(int bz_grid_address[][3], int bz_map[],
                                 const int grid_address[][3], const int mesh[3],
                                 const double rec_lattice[3][3],
                                 const int is_shift[3]) {
    int i, num_bz_map, num_bzgp;
    size_t *dense_bz_map;

    num_bz_map = mesh[0] * mesh[1] * mesh[2] * 8;

    if ((dense_bz_map = (size_t *)malloc(sizeof(size_t) * num_bz_map)) ==
        NULL) {
        warning_print("spglib: Memory of unique_rot could not be allocated.");
        return 0;
    }

    num_bzgp = kpt_relocate_dense_BZ_grid_address(bz_grid_address, dense_bz_map,
                                                  grid_address, mesh,
                                                  rec_lattice, is_shift);

    for (i = 0; i < num_bz_map; i++) {
        if (dense_bz_map[i] == num_bz_map) {
            bz_map[i] = -1;
        } else {
            bz_map[i] = dense_bz_map[i];
        }
    }

    free(dense_bz_map);
    dense_bz_map = NULL;

    return num_bzgp;
}

size_t kpt_relocate_dense_BZ_grid_address(
    int bz_grid_address[][3], size_t bz_map[], const int grid_address[][3],
    const int mesh[3], const double rec_lattice[3][3], const int is_shift[3]) {
    return relocate_dense_BZ_grid_address(bz_grid_address, bz_map, grid_address,
                                          mesh, rec_lattice, is_shift);
}

MatINT *kpt_get_point_group_reciprocal(const MatINT *rotations,
                                       const int is_time_reversal) {
    return get_point_group_reciprocal(rotations, is_time_reversal);
}

MatINT *kpt_get_point_group_reciprocal_with_q(const MatINT *rot_reciprocal,
                                              const double symprec,
                                              const size_t num_q,
                                              const double qpoints[][3]) {
    return get_point_group_reciprocal_with_q(rot_reciprocal, symprec, num_q,
                                             qpoints);
}

/* Return NULL if failed */
static MatINT *get_point_group_reciprocal(const MatINT *rotations,
                                          const int is_time_reversal) {
    int i, j, num_rot;
    MatINT *rot_reciprocal, *rot_return;
    int *unique_rot;
    const int inversion[3][3] = {{-1, 0, 0}, {0, -1, 0}, {0, 0, -1}};

    rot_reciprocal = NULL;
    rot_return = NULL;
    unique_rot = NULL;

    if (is_time_reversal) {
        if ((rot_reciprocal = mat_alloc_MatINT(rotations->size * 2)) == NULL) {
            return NULL;
        }
    } else {
        if ((rot_reciprocal = mat_alloc_MatINT(rotations->size)) == NULL) {
            return NULL;
        }
    }

    if ((unique_rot = (int *)malloc(sizeof(int) * rot_reciprocal->size)) ==
        NULL) {
        warning_print("spglib: Memory of unique_rot could not be allocated.");
        mat_free_MatINT(rot_reciprocal);
        rot_reciprocal = NULL;
        return NULL;
    }

    for (i = 0; i < rot_reciprocal->size; i++) {
        unique_rot[i] = -1;
    }

    for (i = 0; i < rotations->size; i++) {
        mat_transpose_matrix_i3(rot_reciprocal->mat[i], rotations->mat[i]);

        if (is_time_reversal) {
            mat_multiply_matrix_i3(rot_reciprocal->mat[rotations->size + i],
                                   inversion, rot_reciprocal->mat[i]);
        }
    }

    num_rot = 0;
    for (i = 0; i < rot_reciprocal->size; i++) {
        for (j = 0; j < num_rot; j++) {
            if (mat_check_identity_matrix_i3(rot_reciprocal->mat[unique_rot[j]],
                                             rot_reciprocal->mat[i])) {
                goto escape;
            }
        }
        unique_rot[num_rot] = i;
        num_rot++;
    escape:;
    }

    if ((rot_return = mat_alloc_MatINT(num_rot)) != NULL) {
        for (i = 0; i < num_rot; i++) {
            mat_copy_matrix_i3(rot_return->mat[i],
                               rot_reciprocal->mat[unique_rot[i]]);
        }
    }

    free(unique_rot);
    unique_rot = NULL;
    mat_free_MatINT(rot_reciprocal);
    rot_reciprocal = NULL;

    return rot_return;
}

/* Return NULL if failed */
static MatINT *get_point_group_reciprocal_with_q(const MatINT *rot_reciprocal,
                                                 const double symprec,
                                                 const size_t num_q,
                                                 const double qpoints[][3]) {
    int i, j, k, l, is_all_ok, num_rot;
    int *ir_rot;
    double q_rot[3], diff[3];
    MatINT *rot_reciprocal_q;

    ir_rot = NULL;
    rot_reciprocal_q = NULL;
    is_all_ok = 0;
    num_rot = 0;

    if ((ir_rot = (int *)malloc(sizeof(int) * rot_reciprocal->size)) == NULL) {
        warning_print("spglib: Memory of ir_rot could not be allocated.");
        return NULL;
    }

    for (i = 0; i < rot_reciprocal->size; i++) {
        ir_rot[i] = -1;
    }
    for (i = 0; i < rot_reciprocal->size; i++) {
        for (j = 0; j < num_q; j++) {
            is_all_ok = 0;
            mat_multiply_matrix_vector_id3(q_rot, rot_reciprocal->mat[i],
                                           qpoints[j]);

            for (k = 0; k < num_q; k++) {
                for (l = 0; l < 3; l++) {
                    diff[l] = q_rot[l] - qpoints[k][l];
                    diff[l] -= mat_Nint(diff[l]);
                }

                if (mat_Dabs(diff[0]) < symprec &&
                    mat_Dabs(diff[1]) < symprec &&
                    mat_Dabs(diff[2]) < symprec) {
                    is_all_ok = 1;
                    break;
                }
            }

            if (!is_all_ok) {
                break;
            }
        }

        if (is_all_ok) {
            ir_rot[num_rot] = i;
            num_rot++;
        }
    }

    if ((rot_reciprocal_q = mat_alloc_MatINT(num_rot)) != NULL) {
        for (i = 0; i < num_rot; i++) {
            mat_copy_matrix_i3(rot_reciprocal_q->mat[i],
                               rot_reciprocal->mat[ir_rot[i]]);
        }
    }

    free(ir_rot);
    ir_rot = NULL;

    return rot_reciprocal_q;
}

static size_t get_dense_ir_reciprocal_mesh(int grid_address[][3],
                                           size_t ir_mapping_table[],
                                           const int mesh[3],
                                           const int is_shift[3],
                                           const MatINT *rot_reciprocal) {
    if (check_mesh_symmetry(mesh, is_shift, rot_reciprocal)) {
        return get_dense_ir_reciprocal_mesh_normal(
            grid_address, ir_mapping_table, mesh, is_shift, rot_reciprocal);
    } else {
        return get_dense_ir_reciprocal_mesh_distortion(
            grid_address, ir_mapping_table, mesh, is_shift, rot_reciprocal);
    }
}

static size_t get_dense_ir_reciprocal_mesh_normal(
    int grid_address[][3], size_t ir_mapping_table[], const int mesh[3],
    const int is_shift[3], const MatINT *rot_reciprocal) {
    /* In the following loop, mesh is doubled. */
    /* Even and odd mesh numbers correspond to */
    /* is_shift[i] are 0 or 1, respectively. */
    /* is_shift = [0,0,0] gives Gamma center mesh. */
    /* grid: reducible grid points */
    /* ir_mapping_table: the mapping from each point to ir-point. */

    long i;
    size_t grid_point_rot;
    int j;
    int address_double[3], address_double_rot[3];

    kgd_get_all_grid_addresses(grid_address, mesh);

#pragma omp parallel for private(j, grid_point_rot, address_double, \
                                 address_double_rot)
    for (i = 0; i < mesh[0] * mesh[1] * (size_t)(mesh[2]); i++) {
        kgd_get_grid_address_double_mesh(address_double, grid_address[i], mesh,
                                         is_shift);
        ir_mapping_table[i] = i;
        for (j = 0; j < rot_reciprocal->size; j++) {
            mat_multiply_matrix_vector_i3(
                address_double_rot, rot_reciprocal->mat[j], address_double);
            grid_point_rot =
                kgd_get_dense_grid_point_double_mesh(address_double_rot, mesh);
            if (grid_point_rot < ir_mapping_table[i]) {
#ifdef _OPENMP
                ir_mapping_table[i] = grid_point_rot;
#else
                ir_mapping_table[i] = ir_mapping_table[grid_point_rot];
                break;
#endif
            }
        }
    }

    return get_dense_num_ir(ir_mapping_table, mesh);
}

static size_t get_dense_ir_reciprocal_mesh_distortion(
    int grid_address[][3], size_t ir_mapping_table[], const int mesh[3],
    const int is_shift[3], const MatINT *rot_reciprocal) {
    long i;
    size_t grid_point_rot;
    int j, k, indivisible;
    int address_double[3], address_double_rot[3];
    long long_address_double[3], long_address_double_rot[3], divisor[3];

    /* divisor, long_address_double, and long_address_double_rot have */
    /* long integer type to treat dense mesh. */

    kgd_get_all_grid_addresses(grid_address, mesh);

    for (j = 0; j < 3; j++) {
        divisor[j] = mesh[(j + 1) % 3] * mesh[(j + 2) % 3];
    }

#pragma omp parallel for private(j, k, grid_point_rot, address_double,    \
                                 address_double_rot, long_address_double, \
                                 long_address_double_rot)
    for (i = 0; i < mesh[0] * mesh[1] * (size_t)(mesh[2]); i++) {
        kgd_get_grid_address_double_mesh(address_double, grid_address[i], mesh,
                                         is_shift);
        for (j = 0; j < 3; j++) {
            long_address_double[j] = address_double[j] * divisor[j];
        }
        ir_mapping_table[i] = i;
        for (j = 0; j < rot_reciprocal->size; j++) {
            /* Equivalent to mat_multiply_matrix_vector_i3 except for data type
             */
            for (k = 0; k < 3; k++) {
                long_address_double_rot[k] =
                    rot_reciprocal->mat[j][k][0] * long_address_double[0] +
                    rot_reciprocal->mat[j][k][1] * long_address_double[1] +
                    rot_reciprocal->mat[j][k][2] * long_address_double[2];
            }

            for (k = 0; k < 3; k++) {
                indivisible = long_address_double_rot[k] % divisor[k];
                if (indivisible) {
                    break;
                }
                address_double_rot[k] = long_address_double_rot[k] / divisor[k];
                if ((address_double_rot[k] % 2 != 0 && is_shift[k] == 0) ||
                    (address_double_rot[k] % 2 == 0 && is_shift[k] == 1)) {
                    indivisible = 1;
                    break;
                }
            }
            if (indivisible) {
                continue;
            }
            grid_point_rot =
                kgd_get_dense_grid_point_double_mesh(address_double_rot, mesh);
            if (grid_point_rot < ir_mapping_table[i]) {
#ifdef _OPENMP
                ir_mapping_table[i] = grid_point_rot;
#else
                ir_mapping_table[i] = ir_mapping_table[grid_point_rot];
                break;
#endif
            }
        }
    }

    return get_dense_num_ir(ir_mapping_table, mesh);
}

static size_t get_dense_num_ir(size_t ir_mapping_table[], const int mesh[3]) {
    long i;
    size_t num_ir;

    num_ir = 0;

#pragma omp parallel for reduction(+ : num_ir)
    for (i = 0; i < mesh[0] * mesh[1] * (size_t)(mesh[2]); i++) {
        if (ir_mapping_table[i] == i) {
            num_ir++;
        }
    }

#ifdef _OPENMP
    for (i = 0; i < mesh[0] * mesh[1] * (size_t)(mesh[2]); i++) {
        ir_mapping_table[i] = ir_mapping_table[ir_mapping_table[i]];
    }
#endif

    return num_ir;
}

static size_t relocate_dense_BZ_grid_address(
    int bz_grid_address[][3], size_t bz_map[], const int grid_address[][3],
    const int mesh[3], const double rec_lattice[3][3], const int is_shift[3]) {
    double tolerance, min_distance;
    double q_vector[3], distance[KPT_NUM_BZ_SEARCH_SPACE];
    int bzmesh[3], bz_address_double[3];
    size_t i, boundary_num_gp, total_num_gp, bzgp, gp, num_bzmesh;
    int j, k, min_index;

    tolerance = get_tolerance_for_BZ_reduction(rec_lattice, mesh);
    for (j = 0; j < 3; j++) {
        bzmesh[j] = mesh[j] * 2;
    }

    num_bzmesh = bzmesh[0] * bzmesh[1] * (size_t)(bzmesh[2]);
    for (i = 0; i < num_bzmesh; i++) {
        bz_map[i] = num_bzmesh;
    }

    boundary_num_gp = 0;
    total_num_gp = mesh[0] * mesh[1] * (size_t)(mesh[2]);

    /* Multithreading doesn't work for this loop since gp calculated */
    /* with boundary_num_gp is unstable to store bz_grid_address. */
    for (i = 0; i < total_num_gp; i++) {
        for (j = 0; j < KPT_NUM_BZ_SEARCH_SPACE; j++) {
            for (k = 0; k < 3; k++) {
                q_vector[k] =
                    ((grid_address[i][k] + bz_search_space[j][k] * mesh[k]) *
                         2 +
                     is_shift[k]) /
                    ((double)mesh[k]) / 2;
            }
            mat_multiply_matrix_vector_d3(q_vector, rec_lattice, q_vector);
            distance[j] = mat_norm_squared_d3(q_vector);
        }
        min_distance = distance[0];
        min_index = 0;
        for (j = 1; j < KPT_NUM_BZ_SEARCH_SPACE; j++) {
            if (distance[j] < min_distance) {
                min_distance = distance[j];
                min_index = j;
            }
        }

        for (j = 0; j < KPT_NUM_BZ_SEARCH_SPACE; j++) {
            if (distance[j] < min_distance + tolerance) {
                if (j == min_index) {
                    gp = i;
                } else {
                    gp = boundary_num_gp + total_num_gp;
                }

                for (k = 0; k < 3; k++) {
                    bz_grid_address[gp][k] =
                        grid_address[i][k] + bz_search_space[j][k] * mesh[k];
                    bz_address_double[k] =
                        bz_grid_address[gp][k] * 2 + is_shift[k];
                }
                bzgp = kgd_get_dense_grid_point_double_mesh(bz_address_double,
                                                            bzmesh);
                bz_map[bzgp] = gp;
                if (j != min_index) {
                    boundary_num_gp++;
                }
            }
        }
    }

    return boundary_num_gp + total_num_gp;
}

static double get_tolerance_for_BZ_reduction(const double rec_lattice[3][3],
                                             const int mesh[3]) {
    int i, j;
    double tolerance;
    double length[3];

    for (i = 0; i < 3; i++) {
        length[i] = 0;
        for (j = 0; j < 3; j++) {
            length[i] += rec_lattice[j][i] * rec_lattice[j][i];
        }
        length[i] /= mesh[i] * mesh[i];
    }
    tolerance = length[0];
    for (i = 1; i < 3; i++) {
        if (tolerance < length[i]) {
            tolerance = length[i];
        }
    }
    tolerance *= 0.01;

    return tolerance;
}

static int check_mesh_symmetry(const int mesh[3], const int is_shift[3],
                               const MatINT *rot_reciprocal) {
    int i, j, k, sum;
    int eq[3];

    eq[0] = 0; /* a=b */
    eq[1] = 0; /* b=c */
    eq[2] = 0; /* c=a */

    /* Check 3 and 6 fold rotations and non-conventional choice of unit cells */
    for (i = 0; i < rot_reciprocal->size; i++) {
        sum = 0;
        for (j = 0; j < 3; j++) {
            for (k = 0; k < 3; k++) {
                sum += abs(rot_reciprocal->mat[i][j][k]);
            }
        }
        if (sum > 3) {
            return 0;
        }
    }

    for (i = 0; i < rot_reciprocal->size; i++) {
        if (rot_reciprocal->mat[i][0][0] == 0 &&
            rot_reciprocal->mat[i][1][0] == 1 &&
            rot_reciprocal->mat[i][2][0] == 0) {
            eq[0] = 1;
        }
        if (rot_reciprocal->mat[i][0][0] == 0 &&
            rot_reciprocal->mat[i][1][0] == 1 &&
            rot_reciprocal->mat[i][2][0] == 0) {
            eq[1] = 1;
        }
        if (rot_reciprocal->mat[i][0][0] == 0 &&
            rot_reciprocal->mat[i][1][0] == 0 &&
            rot_reciprocal->mat[i][2][0] == 1) {
            eq[2] = 1;
        }
    }

    return (((eq[0] && mesh[0] == mesh[1] && is_shift[0] == is_shift[1]) ||
             (!eq[0])) &&
            ((eq[1] && mesh[1] == mesh[2] && is_shift[1] == is_shift[2]) ||
             (!eq[1])) &&
            ((eq[2] && mesh[2] == mesh[0] && is_shift[2] == is_shift[0]) ||
             (!eq[2])));
}
