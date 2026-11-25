// Copyright (C) 2011 Atsushi Togo
// This file is part of spglib.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef __refinement_H__
#define __refinement_H__

#include "cell.h"
#include "spacegroup.h"
#include "symmetry.h"

typedef struct {
    Cell *bravais;
    Symmetry *symmetry;
    int *wyckoffs;
    char (*site_symmetry_symbols)[7];
    int *equivalent_atoms;
    int *crystallographic_orbits;
    int *std_mapping_to_primitive;
    double rotation[3][3];
} ExactStructure;

ExactStructure *ref_get_exact_structure_and_symmetry(Spacegroup *spacegroup,
                                                     Cell const *primitive,
                                                     Cell const *cell,
                                                     int const *mapping_table,
                                                     double const symprec);
Symmetry *ref_get_primitive_symmetry(double const t_mat[3][3],
                                     Symmetry const *sym);
void ref_free_exact_structure(ExactStructure *exstr);
int ref_find_similar_bravais_lattice(Spacegroup *spacegroup,
                                     double const symprec);
void ref_get_conventional_lattice(double lattice[3][3],
                                  Spacegroup const *spacegroup);
void ref_measure_rigid_rotation(double rotation[3][3],
                                double const bravais_lattice[3][3],
                                double const std_lattice[3][3]);

#endif
