// Copyright (C) 2010 Atsushi Togo
// This file is part of spglib.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef __spg_database_H__
#define __spg_database_H__

#include "spacegroup.h"
#include "symmetry.h"

typedef struct {
    int number;
    char schoenflies[7];
    char hall_symbol[17];
    char international[32];
    char international_full[20];
    char international_short[11];
    char choice[6];
    Centering centering;
    int pointgroup_number;
} SpacegroupType;

void spgdb_decode_symmetry(int rot[3][3], double trans[3], int const encoded);
int spgdb_get_operation(int rot[3][3], double trans[3], int const hall_number);
void spgdb_get_operation_index(int indices[2], int const hall_number);
Symmetry *spgdb_get_spacegroup_operations(int const hall_number);
SpacegroupType spgdb_get_spacegroup_type(int const hall_number);
int spgdb_remove_space(char symbol[], int const num_char);

#endif
