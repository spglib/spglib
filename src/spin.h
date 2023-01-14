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

#ifndef __spin_H__
#define __spin_H__

#include "cell.h"
#include "mathfunc.h"
#include "symmetry.h"

/**
 * @brief
 *
 * @param[out] equivalent_atoms
 * @param[out] permutations such that the p-th operation in `magnetic_symmetry`
 * maps site-`i` to site-`permutations[p * cell->size + i]`.
 * @param[out] prim_lattice
 * @param[in] sym_nonspin Symmetry operations with ignoring spin
 * @param[in] cell
 * @param[in] with_time_reversal true if consider time reversal operation
 * @param[in] is_axial true if site tensors are axial w.r.t. time-reversal
 * operations
 * @param[in] symprec
 * @param[in] angle_tolerance
 * @param[in] mag_symprec if mag_sympprec < 0, use symprec instead
 * @return Return NULL if failed.
 */
MagneticSymmetry *spn_get_operations_with_site_tensors(
    int **equivalent_atoms, int **permutations, double prim_lattice[3][3],
    const Symmetry *sym_nonspin, const Cell *cell, const int with_time_reversal,
    const int is_axial, const double symprec, const double angle_tolerance,
    const double mag_symprec);
VecDBL *spn_collect_pure_translations_from_magnetic_symmetry(
    const MagneticSymmetry *sym_msg);
Cell *spn_get_idealized_cell(const int *permutations, const Cell *cell,
                             const MagneticSymmetry *magnetic_symmetry,
                             const int with_time_reversal, const int is_axial);
double *spn_alloc_site_tensors(const int num_atoms, const int tensor_rank);

#endif
