// Copyright (C) 2011 Atsushi Togo
// This file is part of spglib.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef __site_symmetry_H__
#define __site_symmetry_H__

#include "cell.h"
#include "mathfunc.h"
#include "symmetry.h"

VecDBL *ssm_get_exact_positions(int *wyckoffs, int *equiv_atoms,
                                char (*site_symmetry_symbols)[7],
                                Cell const *bravais, Symmetry const *conv_sym,
                                int const num_pure_trans, int const hall_number,
                                double const symprec);

#endif
