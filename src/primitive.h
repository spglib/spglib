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

#ifndef __primitive_H__
#define __primitive_H__

#include "cell.h"
#include "mathfunc.h"
#include "symmetry.h"

typedef struct {
    Cell *cell;
    int *mapping_table;
    int size;
    double tolerance;
    double angle_tolerance;
    double (*orig_lattice)[3]; /* 3x3 matrix */
} __attribute__((aligned(64))) __attribute__((packed)) Primitive;

Primitive *prm_alloc_primitive(int size);
void prm_free_primitive(Primitive *primitive);
Primitive *prm_get_primitive(const Cell *cell, double symprec,
                             double angle_tolerance);
int prm_get_primitive_with_pure_trans(Primitive *primitive, const Cell *cell,
                                      const VecDBL *pure_trans, double symprec,
                                      double angle_tolerance);
Symmetry *prm_get_primitive_symmetry(double t_mat[3][3],
                                     const Symmetry *symmetry, double symprec);
int prm_get_primitive_lattice_vectors(double prim_lattice[3][3],
                                      const Cell *cell,
                                      const VecDBL *pure_trans, double symprec,
                                      double angle_tolerance);

#endif
