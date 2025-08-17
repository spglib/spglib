// Copyright (C) 2015 Atsushi Togo
// This file is part of spglib.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef __NIGGLI_H__
#define __NIGGLI_H__

#include "base.h"

#define NIGGLI_MAJOR_VERSION 0
#define NIGGLI_MINOR_VERSION 1
#define NIGGLI_MICRO_VERSION 2

int niggli_get_major_version(void);
int niggli_get_minor_version(void);
int niggli_get_micro_version(void);
SPG_API_TEST int niggli_reduce(double *lattice_, double const eps_,
                               int const aperiodic_axis);

#endif
