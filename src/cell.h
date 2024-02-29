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

#ifndef __cell_H__
#define __cell_H__

#include "mathfunc.h"

typedef enum {
    NOSPIN = -1,
    COLLINEAR = 0,
    NONCOLLINEAR = 1,
} SiteTensorType;

typedef struct {
    /* Number of atoms */
    int size;
    /* Used for layer group. Set -1 for space-group search */
    int aperiodic_axis;
    /* 3x3 matrix */
    double (*lattice)[3];
    /* Atomic types with length (size, ) */
    int *types;
    /* Scaled positions with length (size, 3) */
    double (*position)[3];
    /* Rank of site tensors. Set COLLINEAR for scalar, */
    /* and NONCOLLINEAR for vector. */
    /* If no site tensors, set SiteTensorType.NOSPIN. */
    SiteTensorType tensor_rank;
    /* For tensor_rank=COLLINEAR, site tensors with (size, ).*/
    /* For tensor_rank=NONCOLLINEAR, site tensors with (size * 3, ).*/
    double *tensors;
} Cell;

SPG_API_TEST Cell *cel_alloc_cell(int const size,
                                  SiteTensorType const tensor_rank);
SPG_API_TEST void cel_free_cell(Cell *cell);
void cel_set_cell(Cell *cell, double const lattice[3][3],
                  double const position[][3], int const types[]);
SPG_API_TEST void cel_set_layer_cell(Cell *cell, double const lattice[3][3],
                                     double const position[][3],
                                     int const types[],
                                     int const aperiodic_axis);
void cel_set_cell_with_tensors(Cell *cell, double const lattice[3][3],
                               double const position[][3], int const types[],
                               double const *tensors);
Cell *cel_copy_cell(Cell const *cell);
int cel_is_overlap(double const a[3], double const b[3],
                   double const lattice[3][3], double const symprec);
int cel_is_overlap_with_same_type(double const a[3], double const b[3],
                                  int const type_a, int const type_b,
                                  double const lattice[3][3],
                                  double const symprec);
int cel_any_overlap(Cell const *cell, double const symprec);
int cel_any_overlap_with_same_type(Cell const *cell, double const symprec);
Cell *cel_trim_cell(int *mapping_table, double const trimmed_lattice[3][3],
                    Cell const *cell, double const symprec);
int cel_layer_is_overlap(double const a[3], double const b[3],
                         double const lattice[3][3], int const periodic_axes[2],
                         double const symprec);
int cel_layer_is_overlap_with_same_type(double const a[3], double const b[3],
                                        int const type_a, int const type_b,
                                        double const lattice[3][3],
                                        int const periodic_axes[2],
                                        double const symprec);
int cel_layer_any_overlap_with_same_type(Cell const *cell,
                                         int const periodic_axes[2],
                                         double const symprec);

#endif
