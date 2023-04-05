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

#ifndef __kpoint_H__
#define __kpoint_H__

#include <stddef.h>

#include "mathfunc.h"

int kpt_get_irreducible_reciprocal_mesh(int grid_address[][3],
                                        int ir_mapping_table[],
                                        const int mesh[3],
                                        const int is_shift[3],
                                        const MatINT *rot_reciprocal);
size_t kpt_get_dense_irreducible_reciprocal_mesh(int grid_address[][3],
                                                 size_t ir_mapping_table[],
                                                 const int mesh[3],
                                                 const int is_shift[3],
                                                 const MatINT *rot_reciprocal);
int kpt_get_stabilized_reciprocal_mesh(
    int grid_address[][3], int ir_mapping_table[], const int mesh[3],
    const int is_shift[3], const int is_time_reversal, const MatINT *rotations,
    const size_t num_q, const double qpoints[][3]);
size_t kpt_get_dense_stabilized_reciprocal_mesh(
    int grid_address[][3], size_t ir_mapping_table[], const int mesh[3],
    const int is_shift[3], const int is_time_reversal, const MatINT *rotations,
    const size_t num_q, const double qpoints[][3]);
void kpt_get_dense_grid_points_by_rotations(size_t rot_grid_points[],
                                            const int address_orig[3],
                                            const int (*rot_reciprocal)[3][3],
                                            const int num_rot,
                                            const int mesh[3],
                                            const int is_shift[3]);
void kpt_get_dense_BZ_grid_points_by_rotations(
    size_t rot_grid_points[], const int address_orig[3],
    const int (*rot_reciprocal)[3][3], const int num_rot, const int mesh[3],
    const int is_shift[3], const size_t bz_map[]);
int kpt_relocate_BZ_grid_address(int bz_grid_address[][3], int bz_map[],
                                 const int grid_address[][3], const int mesh[3],
                                 const double rec_lattice[3][3],
                                 const int is_shift[3]);
size_t kpt_relocate_dense_BZ_grid_address(
    int bz_grid_address[][3], size_t bz_map[], const int grid_address[][3],
    const int mesh[3], const double rec_lattice[3][3], const int is_shift[3]);
MatINT *kpt_get_point_group_reciprocal(const MatINT *rotations,
                                       const int is_time_reversal);
MatINT *kpt_get_point_group_reciprocal_with_q(const MatINT *rot_reciprocal,
                                              const double symprec,
                                              const size_t num_q,
                                              const double qpoints[][3]);

#endif
