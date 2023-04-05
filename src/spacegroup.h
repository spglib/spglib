/* Copyright (C) 2010 Atsushi Togo */
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

#ifndef __spacegroup_H__
#define __spacegroup_H__

#include "cell.h"
#include "mathfunc.h"
#include "primitive.h"
#include "symmetry.h"

typedef struct {
    int number;
    int hall_number;
    int pointgroup_number;
    char schoenflies[7];
    char hall_symbol[17];
    char international[32];
    char international_long[20];
    char international_short[11];
    char choice[6];
    double bravais_lattice[3][3];
    double origin_shift[3];
} Spacegroup;

typedef enum {
    CENTERING_ERROR,
    PRIMITIVE,
    BODY,
    FACE,
    A_FACE,
    B_FACE,
    C_FACE,
    BASE,
    R_CENTER,
} Centering;

Spacegroup *spa_search_spacegroup(const Primitive *primitive,
                                  const int hall_number, const double symprec,
                                  const double angle_tolerance);
Spacegroup *spa_search_spacegroup_with_symmetry(const Symmetry *symmetry,
                                                const double prim_lat[3][3],
                                                const double symprec);
Cell *spa_transform_to_primitive(int *mapping_table, const Cell *cell,
                                 const double trans_mat[3][3],
                                 const Centering centering,
                                 const double symprec);
Cell *spa_transform_from_primitive(const Cell *primitive,
                                   const Centering centering,
                                   const double symprec);
void spa_copy_spacegroup(Spacegroup *dst, const Spacegroup *src);

#endif
