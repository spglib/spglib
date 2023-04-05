/* Copyright (C) 2011 Atsushi Togo */
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
/* refinement.c */
/* Copyright (C) 2011 Atsushi Togo */

#include "refinement.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "cell.h"
#include "debug.h"
#include "hall_symbol.h"
#include "mathfunc.h"
#include "pointgroup.h"
#include "site_symmetry.h"
#include "spacegroup.h"
#include "spg_database.h"
#include "symmetry.h"

static Cell *get_Wyckoff_positions(
    int *wyckoffs, char (*site_symmetry_symbols)[7], int *equiv_atoms,
    int *crystallographic_orbits, int *std_mapping_to_primitive,
    const Cell *primitive, const Cell *cell, const Spacegroup *spacegroup,
    const Symmetry *symmetry, const int *mapping_table, const double symprec);
static Cell *get_bravais_exact_positions_and_lattice(
    int *wyckoffs, char (*site_symmetry_symbols)[7], int *equiv_atoms,
    int *std_mapping_to_primitive, const Spacegroup *spacegroup,
    const Cell *primitive, const double symprec);
static Cell *expand_positions_in_bravais(
    int *wyckoffs, char (*site_symmetry_symbols)[7], int *equiv_atoms,
    int *std_mapping_to_primitive, const Cell *conv_prim,
    const Symmetry *conv_sym, const int num_pure_trans,
    const int *wyckoffs_prim, const char (*site_symmetry_symbols_prim)[7],
    const int *equiv_atoms_prim);
static Cell *get_conventional_primitive(const Spacegroup *spacegroup,
                                        const Cell *primitive);
static int get_number_of_pure_translation(const Symmetry *conv_sym);
static void set_tricli(double lattice[3][3], const double metric[3][3]);
static void set_monocli(double lattice[3][3], const double metric[3][3],
                        const char choice[6]);
static void set_layer_monocli(double lattice[3][3], const double metric[3][3],
                              const char choice[6]);
static void set_ortho(double lattice[3][3], const double metric[3][3]);
static void set_tetra(double lattice[3][3], const double metric[3][3]);
static void set_trigo(double lattice[3][3], const double metric[3][3]);
static void set_rhomb(double lattice[3][3], const double metric[3][3]);
static void set_cubic(double lattice[3][3], const double metric[3][3]);

static Symmetry *get_refined_symmetry_operations(const Cell *cell,
                                                 const Cell *primitive,
                                                 const Spacegroup *spacegroup,
                                                 const double symprec);
static void set_translation_with_origin_shift(Symmetry *conv_sym,
                                              const double origin_shift[3]);
static Symmetry *get_primitive_db_symmetry(const double t_mat[3][3],
                                           const Symmetry *conv_sym);
static void get_corners(int corners[3][8], const int t_mat[3][3]);
static void get_surrounding_frame(int frame[3], const int t_mat[3][3]);
static int set_crystallographic_orbits(int *equiv_atoms_cell,
                                       const Cell *primitive, const Cell *cell,
                                       const int *equiv_atoms_prim,
                                       const int *mapping_table);
static void set_equivalent_atoms_broken_symmetry(int *equiv_atoms_cell,
                                                 const Cell *cell,
                                                 const Symmetry *symmetry,
                                                 const int *mapping_table,
                                                 const double symprec);
static int search_equivalent_atom(const int atom_index, const Cell *cell,
                                  const Symmetry *symmetry,
                                  const double symprec);
static int search_layer_equivalent_atom(const int atom_index, const Cell *cell,
                                        const Symmetry *symmetry,
                                        const int periodic_axes[2],
                                        const double symprec);
static Symmetry *recover_symmetry_in_original_cell(
    const Symmetry *prim_sym, const int t_mat[3][3], const double lattice[3][3],
    const int multiplicity, const int aperiodic_axis, const double symprec);
static VecDBL *get_lattice_translations(const int frame[3],
                                        const double inv_tmat[3][3]);
static VecDBL *remove_overlapping_lattice_points(const double lattice[3][3],
                                                 const VecDBL *lattice_trans,
                                                 const double symprec);
static Symmetry *get_symmetry_in_original_cell(const int t_mat[3][3],
                                               const double inv_tmat[3][3],
                                               const double lattice[3][3],
                                               const Symmetry *prim_sym,
                                               const double symprec);
static Symmetry *copy_symmetry_upon_lattice_points(const VecDBL *pure_trans,
                                                   const Symmetry *t_sym,
                                                   const int aperiodic_axis);
static void get_orthonormal_basis(double basis[3][3],
                                  const double lattice[3][3]);
static const int identity[3][3] = {
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1},
};

/* Return NULL if failed */
/* spacegroup->bravais_lattice and spacegroup->origin_shift are overwritten */
/* by refined ones. */
ExactStructure *ref_get_exact_structure_and_symmetry(Spacegroup *spacegroup,
                                                     const Cell *primitive,
                                                     const Cell *cell,
                                                     const int *mapping_table,
                                                     const double symprec) {
    int *std_mapping_to_primitive, *wyckoffs, *equivalent_atoms;
    int *crystallographic_orbits;
    double rotation[3][3];
    Cell *bravais;
    Symmetry *symmetry;
    ExactStructure *exact_structure;
    char(*site_symmetry_symbols)[7];

    std_mapping_to_primitive = NULL;
    wyckoffs = NULL;
    site_symmetry_symbols = NULL;
    equivalent_atoms = NULL;
    crystallographic_orbits = NULL;
    bravais = NULL;
    symmetry = NULL;
    exact_structure = NULL;

    if (!ref_find_similar_bravais_lattice(spacegroup, symprec)) {
        goto err;
    }

    if ((symmetry = get_refined_symmetry_operations(cell, primitive, spacegroup,
                                                    symprec)) == NULL) {
        goto err;
    }

    if ((wyckoffs = (int *)malloc(sizeof(int) * cell->size)) == NULL) {
        warning_print("spglib: Memory could not be allocated.");
        goto err;
    }

    if ((site_symmetry_symbols =
             (char(*)[7])malloc(sizeof(char[7]) * cell->size)) == NULL) {
        warning_print("spglib: Memory could not be allocated.");
        goto err;
    }

    if ((equivalent_atoms = (int *)malloc(sizeof(int) * cell->size)) == NULL) {
        warning_print("spglib: Memory could not be allocated.");
        goto err;
    }

    if ((crystallographic_orbits = (int *)malloc(sizeof(int) * cell->size)) ==
        NULL) {
        warning_print("spglib: Memory could not be allocated.");
        goto err;
    }

    if ((std_mapping_to_primitive =
             (int *)malloc(sizeof(int) * primitive->size * 4)) == NULL) {
        warning_print("spglib: Memory could not be allocated.");
        goto err;
    }

    if ((bravais = get_Wyckoff_positions(
             wyckoffs, site_symmetry_symbols, equivalent_atoms,
             crystallographic_orbits, std_mapping_to_primitive, primitive, cell,
             spacegroup, symmetry, mapping_table, symprec)) == NULL) {
        sym_free_symmetry(symmetry);
        symmetry = NULL;
        goto err;
    }

    if ((exact_structure = (ExactStructure *)malloc(sizeof(ExactStructure))) ==
        NULL) {
        warning_print("spglib: Memory could not be allocated.");
        sym_free_symmetry(symmetry);
        symmetry = NULL;
        cel_free_cell(bravais);
        bravais = NULL;
        goto err;
    }

    ref_measure_rigid_rotation(rotation, spacegroup->bravais_lattice,
                               bravais->lattice);

    exact_structure->bravais = bravais;
    exact_structure->symmetry = symmetry;
    exact_structure->wyckoffs = wyckoffs;
    exact_structure->site_symmetry_symbols = site_symmetry_symbols;
    exact_structure->equivalent_atoms = equivalent_atoms;
    exact_structure->crystallographic_orbits = crystallographic_orbits;
    exact_structure->std_mapping_to_primitive = std_mapping_to_primitive;
    mat_copy_matrix_d3(exact_structure->rotation, rotation);

    return exact_structure;

err:
    if (wyckoffs != NULL) {
        free(wyckoffs);
        wyckoffs = NULL;
    }
    if (site_symmetry_symbols != NULL) {
        free(site_symmetry_symbols);
        site_symmetry_symbols = NULL;
    }
    if (equivalent_atoms != NULL) {
        free(equivalent_atoms);
        equivalent_atoms = NULL;
    }
    if (crystallographic_orbits != NULL) {
        free(crystallographic_orbits);
        crystallographic_orbits = NULL;
    }
    if (std_mapping_to_primitive != NULL) {
        free(std_mapping_to_primitive);
        std_mapping_to_primitive = NULL;
    }

    return NULL;
}

void ref_free_exact_structure(ExactStructure *exstr) {
    if (exstr != NULL) {
        if (exstr->symmetry != NULL) {
            sym_free_symmetry(exstr->symmetry);
            exstr->symmetry = NULL;
        }
        if (exstr->bravais != NULL) {
            cel_free_cell(exstr->bravais);
            exstr->bravais = NULL;
        }
        if (exstr->wyckoffs != NULL) {
            free(exstr->wyckoffs);
            exstr->wyckoffs = NULL;
        }
        if (exstr->equivalent_atoms != NULL) {
            free(exstr->equivalent_atoms);
            exstr->equivalent_atoms = NULL;
        }
        // CAUSES CORRUPTED DOUBLE LINKED LIST
        if (exstr->crystallographic_orbits != NULL) {
            free(exstr->crystallographic_orbits);
            exstr->crystallographic_orbits = NULL;
        }
        if (exstr->std_mapping_to_primitive != NULL) {
            free(exstr->std_mapping_to_primitive);
            exstr->std_mapping_to_primitive = NULL;
        }
        if (exstr->site_symmetry_symbols != NULL) {
            free(exstr->site_symmetry_symbols);
            exstr->site_symmetry_symbols = NULL;
        }
        free(exstr);
    }
}

/* Return NULL if failed */
static Cell *get_Wyckoff_positions(
    int *wyckoffs, char (*site_symmetry_symbols)[7], int *equiv_atoms,
    int *crystallographic_orbits, int *std_mapping_to_primitive,
    const Cell *primitive, const Cell *cell, const Spacegroup *spacegroup,
    const Symmetry *symmetry, const int *mapping_table, const double symprec) {
    Cell *bravais;
    int i, j, num_prim_sym;
    int *wyckoffs_bravais, *equiv_atoms_bravais;
    int operation_index[2];
    char(*site_symmetry_symbols_bravais)[7];

    debug_print("get_Wyckoff_positions\n");

    bravais = NULL;
    wyckoffs_bravais = NULL;
    site_symmetry_symbols_bravais = NULL;
    equiv_atoms_bravais = NULL;

    if ((wyckoffs_bravais = (int *)malloc(sizeof(int) * primitive->size * 4)) ==
        NULL) {
        warning_print("spglib: Memory could not be allocated ");
        return NULL;
    }

    if ((site_symmetry_symbols_bravais = (char(*)[7])malloc(
             sizeof(char[7]) * primitive->size * 4)) == NULL) {
        warning_print("spglib: Memory could not be allocated.");
        free(wyckoffs_bravais);
        wyckoffs_bravais = NULL;
        return NULL;
    }

    if ((equiv_atoms_bravais =
             (int *)malloc(sizeof(int) * primitive->size * 4)) == NULL) {
        warning_print("spglib: Memory could not be allocated ");
        free(wyckoffs_bravais);
        wyckoffs_bravais = NULL;
        free(site_symmetry_symbols_bravais);
        site_symmetry_symbols_bravais = NULL;
        return NULL;
    }

    if ((bravais = get_bravais_exact_positions_and_lattice(
             wyckoffs_bravais, site_symmetry_symbols_bravais,
             equiv_atoms_bravais, std_mapping_to_primitive, spacegroup,
             primitive, symprec)) == NULL) {
        warning_print(
            "spglib: get_bravais_exact_positions_and_lattice failed.");
        warning_print(" (line %d, %s).\n", __LINE__, __FILE__);

        goto ret;
    }

    for (i = 0; i < cell->size; i++) {
        wyckoffs[i] = wyckoffs_bravais[mapping_table[i]];
        for (j = 0; j < 7; j++) {
            site_symmetry_symbols[i][j] =
                site_symmetry_symbols_bravais[mapping_table[i]][j];
        }
    }

    /* crystallographic orbits defined here is almost equivalent to */
    /* symmetrically equivalent atoms. */
    /* Only when broken symmetry due to non-primitive cell */
    /* (so-called supercell) is found, they can be different. */
    /* The terminology of crystallographic orbits may not be appropriate, */
    /* but we admit it because we have already used equivalent_atoms */
    /* with respect to the found symmetry operations before including */
    /* equivalent atoms in crystallographic sense. */
    if (set_crystallographic_orbits(crystallographic_orbits, primitive, cell,
                                    equiv_atoms_bravais, mapping_table) == 0) {
        cel_free_cell(bravais);
        bravais = NULL;

        warning_print("spglib: set_crystallographic_orbits failed.");
        warning_print(" (line %d, %s).\n", __LINE__, __FILE__);

        goto ret;
    }

    /* Check symmetry breaking by unusual multiplicity of primitive cell. */
    spgdb_get_operation_index(operation_index, spacegroup->hall_number);
    num_prim_sym = operation_index[0] / (bravais->size / primitive->size);
    if (cell->size * num_prim_sym != symmetry->size * primitive->size) {
        set_equivalent_atoms_broken_symmetry(equiv_atoms, cell, symmetry,
                                             mapping_table, symprec);
    } else {
        for (i = 0; i < cell->size; i++) {
            equiv_atoms[i] = crystallographic_orbits[i];
        }
    }

ret:
    free(equiv_atoms_bravais);
    equiv_atoms_bravais = NULL;
    free(site_symmetry_symbols_bravais);
    site_symmetry_symbols_bravais = NULL;
    free(wyckoffs_bravais);
    wyckoffs_bravais = NULL;

    return bravais;
}

/* Only the atoms corresponding to those in primitive are returned. */
/* Return NULL if failed */
static Cell *get_bravais_exact_positions_and_lattice(
    int *wyckoffs, char (*site_symmetry_symbols)[7], int *equiv_atoms,
    int *std_mapping_to_primitive, const Spacegroup *spacegroup,
    const Cell *primitive, const double symprec) {
    int i, j, num_pure_trans;
    int *wyckoffs_prim, *equiv_atoms_prim;
    char(*site_symmetry_symbols_prim)[7];
    Symmetry *conv_sym;
    Cell *bravais, *conv_prim;
    VecDBL *exact_positions;

    debug_print("get_bravais_exact_positions_and_lattice\n");

    wyckoffs_prim = NULL;
    equiv_atoms_prim = NULL;
    site_symmetry_symbols_prim = NULL;
    conv_prim = NULL;
    bravais = NULL;
    conv_sym = NULL;
    exact_positions = NULL;

    /* Symmetrize atomic positions of conventional unit cell */
    if ((wyckoffs_prim = (int *)malloc(sizeof(int) * primitive->size)) ==
        NULL) {
        warning_print("spglib: Memory could not be allocated ");
        return NULL;
    }

    if ((site_symmetry_symbols_prim =
             (char(*)[7])malloc(sizeof(char[7]) * primitive->size)) == NULL) {
        warning_print("spglib: Memory could not be allocated ");
        free(wyckoffs_prim);
        wyckoffs_prim = NULL;
        return NULL;
    }

    if ((equiv_atoms_prim = (int *)malloc(sizeof(int) * primitive->size)) ==
        NULL) {
        warning_print("spglib: Memory could not be allocated ");
        free(site_symmetry_symbols_prim);
        site_symmetry_symbols_prim = NULL;
        free(wyckoffs_prim);
        wyckoffs_prim = NULL;
        return NULL;
    }

    for (i = 0; i < primitive->size; i++) {
        wyckoffs_prim[i] = -1;
        equiv_atoms_prim[i] = -1;
        for (j = 0; j < 7; j++) {
            site_symmetry_symbols_prim[i][j] = '\0';
        }
    }

    /* Positions of primitive atoms are represented wrt Bravais lattice */
    if ((conv_prim = get_conventional_primitive(spacegroup, primitive)) ==
        NULL) {
        free(wyckoffs_prim);
        wyckoffs_prim = NULL;
        free(equiv_atoms_prim);
        equiv_atoms_prim = NULL;
        free(site_symmetry_symbols_prim);
        site_symmetry_symbols_prim = NULL;
        return NULL;
    }

    /* Symmetries in database (wrt Bravais lattice) */
    if ((conv_sym = spgdb_get_spacegroup_operations(spacegroup->hall_number)) ==
        NULL) {
        goto err;
    }
    num_pure_trans = get_number_of_pure_translation(conv_sym);

    /* Lattice vectors are set. */
    ref_get_conventional_lattice(conv_prim->lattice, spacegroup);

    /* Aperiodic axis is set. */
    conv_prim->aperiodic_axis = spacegroup->hall_number > 0 ? -1 : 2;

    if ((exact_positions = ssm_get_exact_positions(
             wyckoffs_prim, equiv_atoms_prim, site_symmetry_symbols_prim,
             conv_prim, conv_sym, num_pure_trans, spacegroup->hall_number,
             symprec)) == NULL) {
        sym_free_symmetry(conv_sym);
        conv_sym = NULL;
        warning_print("spglib: ssm_get_exact_positions failed.");
        warning_print(" (line %d, %s).\n", __LINE__, __FILE__);
        goto err;
    }

    for (i = 0; i < conv_prim->size; i++) {
        mat_copy_vector_d3(conv_prim->position[i], exact_positions->vec[i]);
    }

    /* The order of atoms in bravais is: */
    /* [atoms in primitive cell] * number of pure translations. */
    bravais = expand_positions_in_bravais(
        wyckoffs, site_symmetry_symbols, equiv_atoms, std_mapping_to_primitive,
        conv_prim, conv_sym, num_pure_trans, wyckoffs_prim,
        site_symmetry_symbols_prim, equiv_atoms_prim);

    mat_free_VecDBL(exact_positions);
    exact_positions = NULL;
    sym_free_symmetry(conv_sym);
    conv_sym = NULL;
err:
    free(wyckoffs_prim);
    wyckoffs_prim = NULL;
    free(equiv_atoms_prim);
    equiv_atoms_prim = NULL;
    free(site_symmetry_symbols_prim);
    site_symmetry_symbols_prim = NULL;
    cel_free_cell(conv_prim);
    conv_prim = NULL;

    return bravais;
}

/* Return NULL if failed */
static Cell *expand_positions_in_bravais(
    int *wyckoffs, char (*site_symmetry_symbols)[7], int *equiv_atoms,
    int *std_mapping_to_primitive, const Cell *conv_prim,
    const Symmetry *conv_sym, const int num_pure_trans,
    const int *wyckoffs_prim, const char (*site_symmetry_symbols_prim)[7],
    const int *equiv_atoms_prim) {
    int i, j, k, lattice_rank;
    int num_atom;
    Cell *bravais;

    bravais = NULL;

    if ((bravais = cel_alloc_cell(conv_prim->size * num_pure_trans,
                                  conv_prim->tensor_rank)) == NULL) {
        return NULL;
    }

    num_atom = 0;
    for (i = 0; i < conv_sym->size; i++) {
        /* Referred atoms in Bravais lattice */
        if (mat_check_identity_matrix_i3(identity, conv_sym->rot[i])) {
            /* The order of atoms in conv_prim is same as that in primitive. */
            for (j = 0; j < conv_prim->size; j++) {
                bravais->types[num_atom] = conv_prim->types[j];
                mat_copy_vector_d3(bravais->position[num_atom],
                                   conv_prim->position[j]);
                for (k = 0; k < 3; k++) {
                    bravais->position[num_atom][k] += conv_sym->trans[i][k];
                }
                wyckoffs[num_atom] = wyckoffs_prim[j];
                for (k = 0; k < 7; k++) {
                    site_symmetry_symbols[num_atom][k] =
                        site_symmetry_symbols_prim[j][k];
                }
                equiv_atoms[num_atom] = equiv_atoms_prim[j];
                std_mapping_to_primitive[num_atom] = j;
                num_atom++;
            }
        }
    }

    if (conv_prim->aperiodic_axis == -1) {
        lattice_rank = 3;
    } else {
        lattice_rank = 2;
    }
    for (i = 0; i < num_atom; i++) {
        for (k = 0; k < lattice_rank; k++) {
            bravais->position[i][k] = mat_Dmod1(bravais->position[i][k]);
        }
    }

    mat_copy_matrix_d3(bravais->lattice, conv_prim->lattice);
    bravais->aperiodic_axis = conv_prim->aperiodic_axis;

    return bravais;
}

static int get_number_of_pure_translation(const Symmetry *conv_sym) {
    int i, num_pure_trans = 0;

    for (i = 0; i < conv_sym->size; i++) {
        if (mat_check_identity_matrix_i3(identity, conv_sym->rot[i])) {
            num_pure_trans++;
        }
    }

    return num_pure_trans;
}

static Cell *get_conventional_primitive(const Spacegroup *spacegroup,
                                        const Cell *primitive) {
    int i, j;
    double inv_brv[3][3], trans_mat[3][3];
    Cell *conv_prim;

    conv_prim = NULL;

    if ((conv_prim = cel_alloc_cell(primitive->size, primitive->tensor_rank)) ==
        NULL) {
        return NULL;
    }

    mat_inverse_matrix_d3(inv_brv, spacegroup->bravais_lattice, 0);
    mat_multiply_matrix_d3(trans_mat, inv_brv, primitive->lattice);

    for (i = 0; i < primitive->size; i++) {
        conv_prim->types[i] = primitive->types[i];
        mat_multiply_matrix_vector_d3(conv_prim->position[i], trans_mat,
                                      primitive->position[i]);
        for (j = 0; j < 3; j++) {
            conv_prim->position[i][j] += spacegroup->origin_shift[j];
            if (primitive->aperiodic_axis == -1 || j != 2) {
                conv_prim->position[i][j] =
                    mat_Dmod1(conv_prim->position[i][j]);
            }
        }
    }

    return conv_prim;
}

/* Return standardized transformation matrix for given space-group type and
 * settings. */
void ref_get_conventional_lattice(double lattice[3][3],
                                  const Spacegroup *spacegroup) {
    int i, j;
    double metric[3][3];
    Pointgroup pointgroup;

    pointgroup = ptg_get_pointgroup(spacegroup->pointgroup_number);

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            lattice[i][j] = 0;
        }
    }

    mat_get_metric(metric, spacegroup->bravais_lattice);

    debug_print("[line %d, %s]\n", __LINE__, __FILE__);
    debug_print("bravais lattice\n");
    debug_print_matrix_d3(spacegroup->bravais_lattice);
    debug_print("%s\n", spacegroup->choice);

    switch (pointgroup.holohedry) {
        case TRICLI:
            set_tricli(lattice, metric);
            break;
        case MONOCLI:
            if (spacegroup->hall_number > 0) { /* b-axis is the unique axis. */
                set_monocli(lattice, metric, spacegroup->choice);
            } else {
                set_layer_monocli(lattice, metric, spacegroup->choice);
            }
            break;
        case ORTHO:
            set_ortho(lattice, metric);
            break;
        case TETRA:
            set_tetra(lattice, metric);
            break;
        case TRIGO:
            if (spacegroup->choice[0] == 'R') {
                set_rhomb(lattice, metric);
            } else {
                set_trigo(lattice, metric);
            }
            break;
        case HEXA:
            set_trigo(lattice, metric);
            break;
        case CUBIC:
            set_cubic(lattice, metric);
            break;
        case HOLOHEDRY_NONE:
            break;
    }
}

/* The conversion refers the wikipedia, */
/* http://en.wikipedia.org/wiki/Fractional_coordinates */
static void set_tricli(double lattice[3][3], const double metric[3][3]) {
    double a, b, c, alpha, beta, gamma, cg, cb, ca, sg;

    a = sqrt(metric[0][0]);
    b = sqrt(metric[1][1]);
    c = sqrt(metric[2][2]);
    alpha = acos(metric[1][2] / b / c);
    beta = acos(metric[0][2] / a / c);
    gamma = acos(metric[0][1] / a / b);

    cg = cos(gamma);
    cb = cos(beta);
    ca = cos(alpha);
    sg = sin(gamma);

    lattice[0][0] = a;
    lattice[0][1] = b * cg;
    lattice[0][2] = c * cb;
    lattice[1][1] = b * sg;
    lattice[1][2] = c * (ca - cb * cg) / sg;
    lattice[2][2] =
        c * sqrt(1 - ca * ca - cb * cb - cg * cg + 2 * ca * cb * cg) / sg;
}

static void set_monocli(double lattice[3][3], const double metric[3][3],
                        const char choice[6]) {
    double a, b, c, angle;
    int pos_char;

    debug_print("set_monocli:\n");
    debug_print_matrix_d3(metric);

    if (choice[0] == '-') {
        pos_char = 1;
    } else {
        pos_char = 0;
    }

    a = sqrt(metric[0][0]);
    b = sqrt(metric[1][1]);
    c = sqrt(metric[2][2]);

    switch (choice[pos_char]) {
        case 'a':
            angle = acos(metric[1][2] / b / c);
            lattice[0][2] = c;
            lattice[1][0] = a;
            lattice[0][1] = b * cos(angle);
            lattice[2][1] = b * sin(angle);
            break;
        case 'b':
            angle = acos(metric[0][2] / a / c);
            lattice[0][0] = a;
            lattice[1][1] = b;
            lattice[0][2] = c * cos(angle);
            lattice[2][2] = c * sin(angle);
            break;
        case 'c':
            angle = acos(metric[0][1] / a / b);
            lattice[0][1] = b;
            lattice[1][2] = c;
            lattice[0][0] = a * cos(angle);
            lattice[2][0] = a * sin(angle);
            break;
        default:
            warning_print("spglib: Monoclinic unique axis could not be found.");
    }
}

/* Monoclinic/Rectangular: a-axis is the unique axis */
/* Monoclinic/Oblique: c-axis is the unique axis */
static void set_layer_monocli(double lattice[3][3], const double metric[3][3],
                              const char choice[6]) {
    double a, b, c, angle;

    debug_print("set_layer_monocli:\n");
    debug_print_matrix_d3(metric);

    a = sqrt(metric[0][0]);
    b = sqrt(metric[1][1]);
    c = sqrt(metric[2][2]);

    switch (choice[0]) {
        case 'a':
            angle = acos(metric[1][2] / b / c);
            lattice[0][0] = a;
            lattice[1][1] = b;
            lattice[1][2] = c * cos(angle);
            lattice[2][2] = c * sin(angle);
            break;
        case 'b': /* This should not happen */
            angle = acos(metric[0][2] / a / c);
            lattice[0][0] = b;
            lattice[1][1] = a;
            lattice[0][2] = c * cos(angle);
            lattice[2][2] = c * sin(angle);
            break;
        case 'c':
            angle = acos(metric[0][1] / a / b);
            lattice[0][0] = a;
            lattice[0][1] = b * cos(angle);
            lattice[1][1] = b * sin(angle);
            lattice[2][2] = c;
            break;
        default:
            warning_print("spglib: Monoclinic unique axis could not be found.");
    }
}

static void set_ortho(double lattice[3][3], const double metric[3][3]) {
    double a, b, c;
    a = sqrt(metric[0][0]);
    b = sqrt(metric[1][1]);
    c = sqrt(metric[2][2]);
    lattice[0][0] = a;
    lattice[1][1] = b;
    lattice[2][2] = c;
}

static void set_tetra(double lattice[3][3], const double metric[3][3]) {
    double a, b, c;
    a = sqrt(metric[0][0]);
    b = sqrt(metric[1][1]);
    c = sqrt(metric[2][2]);
    lattice[0][0] = (a + b) / 2;
    lattice[1][1] = (a + b) / 2;
    lattice[2][2] = c;
}

static void set_rhomb(double lattice[3][3], const double metric[3][3]) {
    double a, b, c, angle, ahex, chex;

    a = sqrt(metric[0][0]);
    b = sqrt(metric[1][1]);
    c = sqrt(metric[2][2]);
    angle = acos(
        (metric[0][1] / a / b + metric[0][2] / a / c + metric[1][2] / b / c) /
        3);

    /* Reference, http://cst-www.nrl.navy.mil/lattice/struk/rgr.html */
    ahex = 2 * (a + b + c) / 3 * sin(angle / 2);
    chex = (a + b + c) / 3 * sqrt(3 * (1 + 2 * cos(angle)));
    lattice[0][0] = ahex / 2;
    lattice[1][0] = ahex / (2 * sqrt(3));
    lattice[2][0] = chex / 3;
    lattice[0][1] = -ahex / 2;
    lattice[1][1] = ahex / (2 * sqrt(3));
    lattice[2][1] = chex / 3;
    lattice[0][2] = 0;
    lattice[1][2] = -ahex / sqrt(3);
    lattice[2][2] = chex / 3;

#ifdef SPGDEBUG
    debug_print("Rhombo lattice: %f %f %f %f %f %f\n", a, b, c,
                acos(metric[0][1] / a / b) / 3.14 * 180,
                acos(metric[0][2] / a / c) / 3.14 * 180,
                acos(metric[1][2] / b / c) / 3.14 * 180);
    double dmetric[3][3];
    mat_get_metric(dmetric, lattice);
    a = sqrt(dmetric[0][0]);
    b = sqrt(dmetric[1][1]);
    c = sqrt(dmetric[2][2]);
    debug_print("Rhombo lattice symmetrized: %f %f %f %f %f %f\n", a, b, c,
                acos(dmetric[0][1] / a / b) / 3.14 * 180,
                acos(dmetric[0][2] / a / c) / 3.14 * 180,
                acos(dmetric[1][2] / b / c) / 3.14 * 180);
#endif
}

static void set_trigo(double lattice[3][3], const double metric[3][3]) {
    double a, b, c;

    a = sqrt(metric[0][0]);
    b = sqrt(metric[1][1]);
    c = sqrt(metric[2][2]);
    lattice[0][0] = (a + b) / 2;
    lattice[0][1] = -(a + b) / 4;
    lattice[1][1] = (a + b) / 4 * sqrt(3);
    lattice[2][2] = c;
}

static void set_cubic(double lattice[3][3], const double metric[3][3]) {
    double a, b, c;

    a = sqrt(metric[0][0]);
    b = sqrt(metric[1][1]);
    c = sqrt(metric[2][2]);
    lattice[0][0] = (a + b + c) / 3;
    lattice[1][1] = (a + b + c) / 3;
    lattice[2][2] = (a + b + c) / 3;
}

/* Return NULL if failed */
static Symmetry *get_refined_symmetry_operations(const Cell *cell,
                                                 const Cell *primitive,
                                                 const Spacegroup *spacegroup,
                                                 const double symprec) {
    int t_mat_int[3][3];
    double inv_prim_lat[3][3], t_mat[3][3];
    Symmetry *conv_sym, *prim_sym, *symmetry;

    conv_sym = NULL;
    prim_sym = NULL;
    symmetry = NULL;

    /* Primitive symmetry from database */
    if ((conv_sym = spgdb_get_spacegroup_operations(spacegroup->hall_number)) ==
        NULL) {
        return NULL;
    }

    mat_inverse_matrix_d3(inv_prim_lat, primitive->lattice, 0);
    mat_multiply_matrix_d3(t_mat, inv_prim_lat, spacegroup->bravais_lattice);

    set_translation_with_origin_shift(conv_sym, spacegroup->origin_shift);

    if ((prim_sym = get_primitive_db_symmetry(t_mat, conv_sym)) == NULL) {
        sym_free_symmetry(conv_sym);
        conv_sym = NULL;
        return NULL;
    }

    sym_free_symmetry(conv_sym);
    conv_sym = NULL;

    /* Input cell symmetry from primitive symmetry */
    mat_multiply_matrix_d3(t_mat, inv_prim_lat, cell->lattice);
    mat_cast_matrix_3d_to_3i(t_mat_int, t_mat);

    symmetry = recover_symmetry_in_original_cell(
        prim_sym, t_mat_int, cell->lattice, cell->size / primitive->size,
        cell->aperiodic_axis, symprec);

    sym_free_symmetry(prim_sym);
    prim_sym = NULL;

    return symmetry;
}

static int set_crystallographic_orbits(int *equiv_atoms_cell,
                                       const Cell *primitive, const Cell *cell,
                                       const int *equiv_atoms_prim,
                                       const int *mapping_table) {
    int i, j;
    int *equiv_atoms;

    equiv_atoms = NULL;

    if ((equiv_atoms = (int *)malloc(sizeof(int) * primitive->size)) == NULL) {
        warning_print("spglib: Memory could not be allocated ");
        return 0;
    }

    for (i = 0; i < primitive->size; i++) {
        for (j = 0; j < cell->size; j++) {
            if (mapping_table[j] == equiv_atoms_prim[i]) {
                equiv_atoms[i] = j;
                break;
            }
        }
    }
    for (i = 0; i < cell->size; i++) {
        equiv_atoms_cell[i] = equiv_atoms[mapping_table[i]];
    }

    free(equiv_atoms);
    equiv_atoms = NULL;

    return 1;
}

static void set_equivalent_atoms_broken_symmetry(int *equiv_atoms_cell,
                                                 const Cell *cell,
                                                 const Symmetry *symmetry,
                                                 const int *mapping_table,
                                                 const double symprec) {
    int i, j;
    int periodic_axes[2];

    if (cell->aperiodic_axis == -1) {
        for (i = 0; i < cell->size; i++) {
            equiv_atoms_cell[i] = i;
            for (j = 0; j < cell->size; j++) {
                if (mapping_table[i] == mapping_table[j]) {
                    if (i == j) {
                        equiv_atoms_cell[i] =
                            equiv_atoms_cell[search_equivalent_atom(
                                i, cell, symmetry, symprec)];
                    } else {
                        equiv_atoms_cell[i] = equiv_atoms_cell[j];
                    }
                    break;
                }
            }
        }
    } else {
        j = 0;
        for (i = 0; i < 3; i++) {
            if (i != cell->aperiodic_axis) {
                periodic_axes[j] = i;
                j++;
            }
        }
        for (i = 0; i < cell->size; i++) {
            equiv_atoms_cell[i] = i;
            for (j = 0; j < cell->size; j++) {
                if (mapping_table[i] == mapping_table[j]) {
                    if (i == j) {
                        equiv_atoms_cell[i] =
                            equiv_atoms_cell[search_layer_equivalent_atom(
                                i, cell, symmetry, periodic_axes, symprec)];
                    } else {
                        equiv_atoms_cell[i] = equiv_atoms_cell[j];
                    }
                    break;
                }
            }
        }
    }
}

static int search_equivalent_atom(const int atom_index, const Cell *cell,
                                  const Symmetry *symmetry,
                                  const double symprec) {
    int i, j;
    double pos_rot[3];

    for (i = 0; i < symmetry->size; i++) {
        mat_multiply_matrix_vector_id3(pos_rot, symmetry->rot[i],
                                       cell->position[atom_index]);
        for (j = 0; j < 3; j++) {
            pos_rot[j] += symmetry->trans[i][j];
        }
        for (j = 0; j < atom_index; j++) {
            if (cel_is_overlap_with_same_type(
                    cell->position[j], pos_rot, cell->types[j],
                    cell->types[atom_index], cell->lattice, symprec)) {
                return j;
            }
        }
    }
    return atom_index;
}

static int search_layer_equivalent_atom(const int atom_index, const Cell *cell,
                                        const Symmetry *symmetry,
                                        const int periodic_axes[2],
                                        const double symprec) {
    int i, j;
    double pos_rot[3];

    for (i = 0; i < symmetry->size; i++) {
        mat_multiply_matrix_vector_id3(pos_rot, symmetry->rot[i],
                                       cell->position[atom_index]);
        for (j = 0; j < 3; j++) {
            pos_rot[j] += symmetry->trans[i][j];
        }
        for (j = 0; j < atom_index; j++) {
            if (cel_layer_is_overlap_with_same_type(
                    cell->position[j], pos_rot, cell->types[j],
                    cell->types[atom_index], cell->lattice, periodic_axes,
                    symprec)) {
                return j;
            }
        }
    }
    return atom_index;
}

static void set_translation_with_origin_shift(Symmetry *conv_sym,
                                              const double origin_shift[3]) {
    int i, j;
    double tmp_vec[3];
    int tmp_mat[3][3];

    /* t' = t + (R-E)w, w is the origin shift */
    for (i = 0; i < conv_sym->size; i++) {
        mat_copy_matrix_i3(tmp_mat, conv_sym->rot[i]);
        tmp_mat[0][0]--;
        tmp_mat[1][1]--;
        tmp_mat[2][2]--;
        mat_multiply_matrix_vector_id3(tmp_vec, tmp_mat, origin_shift);
        for (j = 0; j < 3; j++) {
            conv_sym->trans[i][j] += tmp_vec[j];
        }
    }
}

static Symmetry *get_primitive_db_symmetry(const double t_mat[3][3],
                                           const Symmetry *conv_sym) {
    int i, j, num_op;
    double inv_mat[3][3], tmp_mat[3][3];
    MatINT *r_prim;
    VecDBL *t_prim;
    Symmetry *prim_sym;

    r_prim = NULL;
    t_prim = NULL;
    prim_sym = NULL;

    if ((r_prim = mat_alloc_MatINT(conv_sym->size)) == NULL) {
        return NULL;
    }

    if ((t_prim = mat_alloc_VecDBL(conv_sym->size)) == NULL) {
        mat_free_MatINT(r_prim);
        r_prim = NULL;
        return NULL;
    }

    mat_inverse_matrix_d3(inv_mat, t_mat, 0);

    num_op = 0;
    for (i = 0; i < conv_sym->size; i++) {
        for (j = 0; j < i; j++) {
            if (mat_check_identity_matrix_i3(conv_sym->rot[i],
                                             conv_sym->rot[j])) {
                goto pass;
            }
        }

        /* R' = T*R*T^-1 */
        mat_multiply_matrix_di3(tmp_mat, t_mat, conv_sym->rot[i]);
        mat_multiply_matrix_d3(tmp_mat, tmp_mat, inv_mat);
        mat_cast_matrix_3d_to_3i(r_prim->mat[num_op], tmp_mat);
        /* t' = T*t */
        mat_multiply_matrix_vector_d3(t_prim->vec[num_op], t_mat,
                                      conv_sym->trans[i]);
        num_op++;

    pass:;
    }

    if ((prim_sym = sym_alloc_symmetry(num_op)) == NULL) {
        goto ret;
    }

    for (i = 0; i < num_op; i++) {
        mat_copy_matrix_i3(prim_sym->rot[i], r_prim->mat[i]);
        for (j = 0; j < 3; j++) {
            prim_sym->trans[i][j] =
                t_prim->vec[i][j]; /* This is done in
                                      copy_symmetry_upon_lattice_points */
        }
    }

ret:
    mat_free_MatINT(r_prim);
    r_prim = NULL;
    mat_free_VecDBL(t_prim);
    t_prim = NULL;

    return prim_sym;
}

static void get_surrounding_frame(int frame[3], const int t_mat[3][3]) {
    int i, j, max, min;
    int corners[3][8];

    get_corners(corners, t_mat);

    for (i = 0; i < 3; i++) {
        max = corners[i][0];
        min = corners[i][0];
        for (j = 1; j < 8; j++) {
            if (max < corners[i][j]) {
                max = corners[i][j];
            }
            if (min > corners[i][j]) {
                min = corners[i][j];
            }
        }
        frame[i] = max - min;
    }
}

static void get_corners(int corners[3][8], const int t_mat[3][3]) {
    int i, j;

    /* O */
    for (i = 0; i < 3; i++) {
        corners[i][0] = 0;
    }

    /* a,b,c */
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            corners[j][i + 1] = t_mat[j][i];
        }
    }

    /* b+c, c+a, a+b */
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            corners[j][i + 4] = t_mat[j][(i + 1) % 3] + t_mat[j][(i + 2) % 3];
        }
    }

    /* a+b+c */
    for (i = 0; i < 3; i++) {
        corners[i][7] = t_mat[i][0] + t_mat[i][1] + t_mat[i][2];
    }
}

static Symmetry *recover_symmetry_in_original_cell(
    const Symmetry *prim_sym, const int t_mat[3][3], const double lattice[3][3],
    const int multiplicity, const int aperiodic_axis, const double symprec) {
    Symmetry *symmetry, *t_sym;
    int frame[3];
    double inv_tmat[3][3], tmp_mat[3][3];
    VecDBL *pure_trans, *lattice_trans;

    symmetry = NULL;
    t_sym = NULL;
    pure_trans = NULL;
    lattice_trans = NULL;

    get_surrounding_frame(frame, t_mat);
    mat_cast_matrix_3i_to_3d(tmp_mat, t_mat);
    mat_inverse_matrix_d3(inv_tmat, tmp_mat, 0);

    if ((lattice_trans = get_lattice_translations(frame, inv_tmat)) == NULL) {
        return NULL;
    }

    if ((pure_trans = remove_overlapping_lattice_points(lattice, lattice_trans,
                                                        symprec)) == NULL) {
        mat_free_VecDBL(lattice_trans);
        lattice_trans = NULL;
        return NULL;
    }

    if ((t_sym = get_symmetry_in_original_cell(t_mat, inv_tmat, lattice,
                                               prim_sym, symprec)) == NULL) {
        mat_free_VecDBL(pure_trans);
        pure_trans = NULL;
        mat_free_VecDBL(lattice_trans);
        lattice_trans = NULL;
        return NULL;
    }

    if (pure_trans->size == multiplicity) {
        symmetry = copy_symmetry_upon_lattice_points(pure_trans, t_sym,
                                                     aperiodic_axis);
    }

    mat_free_VecDBL(lattice_trans);
    lattice_trans = NULL;
    mat_free_VecDBL(pure_trans);
    pure_trans = NULL;
    sym_free_symmetry(t_sym);
    t_sym = NULL;

    return symmetry;
}

/* Return NULL if failed */
static VecDBL *get_lattice_translations(const int frame[3],
                                        const double inv_tmat[3][3]) {
    int i, j, k, l, num_trans;
    VecDBL *lattice_trans;

    lattice_trans = NULL;

    if ((lattice_trans = mat_alloc_VecDBL(frame[0] * frame[1] * frame[2])) ==
        NULL) {
        return NULL;
    }

    num_trans = 0;
    for (i = 0; i < frame[0]; i++) {
        for (j = 0; j < frame[1]; j++) {
            for (k = 0; k < frame[2]; k++) {
                lattice_trans->vec[num_trans][0] = i;
                lattice_trans->vec[num_trans][1] = j;
                lattice_trans->vec[num_trans][2] = k;

                /* t' = T^-1*t */
                mat_multiply_matrix_vector_d3(lattice_trans->vec[num_trans],
                                              inv_tmat,
                                              lattice_trans->vec[num_trans]);
                for (l = 0; l < 3; l++) {
                    lattice_trans->vec[num_trans][l] =
                        mat_Dmod1(lattice_trans->vec[num_trans][l]);
                }
                num_trans++;
            }
        }
    }

    return lattice_trans;
}

static VecDBL *remove_overlapping_lattice_points(const double lattice[3][3],
                                                 const VecDBL *lattice_trans,
                                                 const double symprec) {
    int i, j, is_found, num_pure_trans;
    VecDBL *pure_trans, *t;

    pure_trans = NULL;
    t = NULL;

    num_pure_trans = 0;

    if ((t = mat_alloc_VecDBL(lattice_trans->size)) == NULL) {
        return NULL;
    }

    for (i = 0; i < lattice_trans->size; i++) {
        is_found = 0;
        for (j = 0; j < num_pure_trans; j++) {
            if (cel_is_overlap(lattice_trans->vec[i], t->vec[j], lattice,
                               symprec)) {
                is_found = 1;
                break;
            }
        }
        if (!is_found) {
            mat_copy_vector_d3(t->vec[num_pure_trans], lattice_trans->vec[i]);
            num_pure_trans++;
        }
    }

    if ((pure_trans = mat_alloc_VecDBL(num_pure_trans)) == NULL) {
        mat_free_VecDBL(t);
        t = NULL;
        return NULL;
    }

    for (i = 0; i < num_pure_trans; i++) {
        mat_copy_vector_d3(pure_trans->vec[i], t->vec[i]);
    }
    mat_free_VecDBL(t);
    t = NULL;

    return pure_trans;
}

/* Return NULL if failed */
static Symmetry *get_symmetry_in_original_cell(const int t_mat[3][3],
                                               const double inv_tmat[3][3],
                                               const double lattice[3][3],
                                               const Symmetry *prim_sym,
                                               const double symprec) {
    int i, size_sym_orig;
    double tmp_rot_d[3][3], tmp_lat_d[3][3], tmp_lat_i[3][3], tmp_mat[3][3];
    int tmp_rot_i[3][3];
    Symmetry *t_sym, *t_red_sym;

    t_sym = NULL;
    t_red_sym = NULL;

    if ((t_sym = sym_alloc_symmetry(prim_sym->size)) == NULL) {
        return NULL;
    }

    /* transform symmetry operations of primitive cell to those of original */
    size_sym_orig = 0;
    for (i = 0; i < prim_sym->size; i++) {
        /* R' = T^-1*R*T */
        mat_multiply_matrix_di3(tmp_mat, inv_tmat, prim_sym->rot[i]);
        mat_multiply_matrix_di3(tmp_rot_d, tmp_mat, t_mat);

        /* In spglib, symmetry of supercell is defined by the set of symmetry */
        /* operations that are searched among supercell lattice point group */
        /* operations. The supercell lattice may be made by breaking the */
        /* unit cell lattice symmetry. In this case, a part of symmetry */
        /* operations is discarded. */
        mat_cast_matrix_3d_to_3i(tmp_rot_i, tmp_rot_d);
        mat_multiply_matrix_di3(tmp_lat_i, lattice, tmp_rot_i);
        mat_multiply_matrix_d3(tmp_lat_d, lattice, tmp_rot_d);
        if (mat_check_identity_matrix_d3(tmp_lat_i, tmp_lat_d, symprec)) {
            mat_copy_matrix_i3(t_sym->rot[size_sym_orig], tmp_rot_i);
            /* t' = T^-1*t */
            mat_multiply_matrix_vector_d3(t_sym->trans[size_sym_orig], inv_tmat,
                                          prim_sym->trans[i]);
            size_sym_orig++;
        }
    }

    /* Broken symmetry due to supercell multiplicity */
    if (size_sym_orig != prim_sym->size) {
        if ((t_red_sym = sym_alloc_symmetry(size_sym_orig)) == NULL) {
            sym_free_symmetry(t_sym);
            t_sym = NULL;
            return NULL;
        }

        for (i = 0; i < size_sym_orig; i++) {
            mat_copy_matrix_i3(t_red_sym->rot[i], t_sym->rot[i]);
            mat_copy_vector_d3(t_red_sym->trans[i], t_sym->trans[i]);
        }

        sym_free_symmetry(t_sym);
        t_sym = NULL;

        t_sym = t_red_sym;
        t_red_sym = NULL;
    }

    return t_sym;
}

/* Return NULL if failed */
static Symmetry *copy_symmetry_upon_lattice_points(const VecDBL *pure_trans,
                                                   const Symmetry *t_sym,
                                                   const int aperiodic_axis) {
    int i, j, k, size_sym_orig;
    Symmetry *symmetry;

    symmetry = NULL;

    size_sym_orig = t_sym->size;

    if ((symmetry = sym_alloc_symmetry(pure_trans->size * size_sym_orig)) ==
        NULL) {
        return NULL;
    }

    for (i = 0; i < pure_trans->size; i++) {
        for (j = 0; j < size_sym_orig; j++) {
            mat_copy_matrix_i3(symmetry->rot[size_sym_orig * i + j],
                               t_sym->rot[j]);
            mat_copy_vector_d3(symmetry->trans[size_sym_orig * i + j],
                               t_sym->trans[j]);
            for (k = 0; k < 3; k++) {
                symmetry->trans[size_sym_orig * i + j][k] +=
                    pure_trans->vec[i][k];
                if (k != aperiodic_axis) {
                    symmetry->trans[size_sym_orig * i + j][k] =
                        mat_Dmod1(symmetry->trans[size_sym_orig * i + j][k]);
                }
            }
        }
    }

    return symmetry;
}

/* spacegroup->bravais_lattice and spacegroup->origin_shift are overwritten */
/* by refined ones. Return 0 if failed. */
int ref_find_similar_bravais_lattice(Spacegroup *spacegroup,
                                     const double symprec) {
    int i, j, k, rot_i, lattice_rank;
    Symmetry *conv_sym;
    double min_length2, length2, diff, min_length, length;
    double tmp_mat[3][3], std_lattice[3][3];
    double rot_lat[3][3];
    double p[3], shortest_p[3], tmp_vec[3];

    conv_sym = NULL;

    if ((conv_sym = spgdb_get_spacegroup_operations(spacegroup->hall_number)) ==
        NULL) {
        return 0;
    }

    ref_get_conventional_lattice(std_lattice, spacegroup);

    min_length2 = 0;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            min_length2 += spacegroup->bravais_lattice[i][j] *
                           spacegroup->bravais_lattice[i][j];
        }
    }
    min_length = sqrt(min_length2);

    /* For no best match */
    rot_i = -1;

    for (i = 0; i < conv_sym->size; i++) {
        if (mat_get_determinant_i3(conv_sym->rot[i]) < 0) {
            continue;
        }

        /* (a_s', b_s', c_s') = (a_s,b_s,c_s) Rs */
        mat_multiply_matrix_di3(tmp_mat, spacegroup->bravais_lattice,
                                conv_sym->rot[i]);
        length2 = 0;
        for (j = 0; j < 3; j++) {
            for (k = 0; k < 3; k++) {
                diff = tmp_mat[j][k] - std_lattice[j][k];
                length2 += diff * diff;
            }
        }
        length = sqrt(length2);
        if (length < min_length - symprec) {
            mat_copy_matrix_d3(rot_lat, tmp_mat);
            rot_i = i;
            min_length = length;
        }
    }

    /* Given a symmetry operation (W, w), Which is that for */
    /* standardized system, i.e., (a_s, b_s, c_s) and x_s = (P, p)x, */
    /* we view this as change of basis, i.e., inverse of it. */
    /* (W, w)^-1 = (W^-1, -W^-1 w) because */
    /* (W, w)x = x~ -> W^-1 x~ - W^-1 w = (W^-1, -W^-1 w)x~ = x. */
    /* */
    /* We can check this geometrically. */
    /* Basis vectors are rotated and its origin is shifted by W and w. */
    /* (a_s, b_s, c_s) W = (a_s', b_s', c_s') */
    /* The shift is measured in the coordinated before rotation. */
    /* Therefore */
    /* (a_s, b_s, c_s) w = (a_s', b_s', c_s') w' -> w' = W^-1 w. */
    /* From the definition of transformation, we have (W^-1, -W^-1 w). */
    /* From x_s = (P, p) x and x_s' = (W^-1, -W^-1 w) x_s. */
    /* Finally, */
    /* (W^-1, -W^-1 w)(P, p) x = W^-1Px+W^-1p-W^-1w = (W^-1P, W^-1p-W^-1w) */
    min_length = 2;
    lattice_rank = spacegroup->hall_number > 0 ? 3 : 2;
    if (rot_i > -1) {
        for (i = 0; i < conv_sym->size; i++) {
            if (!mat_check_identity_matrix_i3(conv_sym->rot[i],
                                              conv_sym->rot[rot_i])) {
                continue;
            }
            mat_cast_matrix_3i_to_3d(tmp_mat, conv_sym->rot[i]);
            mat_inverse_matrix_d3(tmp_mat, tmp_mat, 0);
            mat_multiply_matrix_vector_d3(p, tmp_mat, spacegroup->origin_shift);
            mat_multiply_matrix_vector_d3(tmp_vec, tmp_mat, conv_sym->trans[i]);
            for (j = 0; j < lattice_rank; j++) {
                p[j] -= tmp_vec[j];
                p[j] -= mat_Nint(p[j]);
            }
            for (j = lattice_rank; j < 3; j++) {
                p[j] -= tmp_vec[j];
            }
            length = sqrt(mat_norm_squared_d3(p));
            if (length < min_length - symprec) {
                min_length = length;
                for (j = 0; j < lattice_rank; j++) {
                    p[j] = mat_Dmod1(p[j]);
                }
                mat_copy_vector_d3(shortest_p, p);
            }
        }
        mat_copy_vector_d3(spacegroup->origin_shift, shortest_p);
        mat_copy_matrix_d3(spacegroup->bravais_lattice, rot_lat);
    }

    sym_free_symmetry(conv_sym);
    conv_sym = NULL;
    return 1;
}

/* Calculate `rotation` s.t. std_lattice = rotation @ bravais_lattice */
void ref_measure_rigid_rotation(double rotation[3][3],
                                const double bravais_lattice[3][3],
                                const double std_lattice[3][3]) {
    /* (a_s^ideal, b_s^ideal, c_s^ideal) = R(a_s, b_s, c_s) */
    double brv_basis[3][3], std_basis[3][3], inv_brv_basis[3][3];

    get_orthonormal_basis(brv_basis, bravais_lattice);
    get_orthonormal_basis(std_basis, std_lattice);
    mat_inverse_matrix_d3(inv_brv_basis, brv_basis, 0);
    mat_multiply_matrix_d3(rotation, std_basis, inv_brv_basis);
}

static void get_orthonormal_basis(double basis[3][3],
                                  const double lattice[3][3]) {
    int i, j;
    double length;
    double basis_T[3][3], lattice_T[3][3];

    mat_transpose_matrix_d3(lattice_T, lattice);
    mat_copy_vector_d3(basis_T[0], lattice_T[0]);
    /* c' = axb */
    mat_cross_product_d3(basis_T[2], lattice_T[0], lattice_T[1]);
    /* b' = (axb)xa = c'xa */
    mat_cross_product_d3(basis_T[1], basis_T[2], lattice_T[0]);

    for (i = 0; i < 3; i++) {
        length = sqrt(mat_norm_squared_d3(basis_T[i]));
        for (j = 0; j < 3; j++) {
            basis_T[i][j] /= length;
        }
    }

    mat_transpose_matrix_d3(basis, basis_T);
}
