// Copyright (C) 2008 Atsushi Togo
// This file is part of spglib.
// SPDX-License-Identifier: BSD-3-Clause

#include "symmetry.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "cell.h"
#include "debug.h"
#include "delaunay.h"
#include "mathfunc.h"
#include "overlap.h"

#define NUM_ATOMS_CRITERION_FOR_OPENMP 1000
#define ANGLE_REDUCE_RATE 0.95
#define SIN_DTHETA2_CUTOFF 1e-12
#define NUM_ATTEMPT 100
#define PI 3.14159265358979323846
/* Tolerance of angle between lattice vectors in degrees */
/* Negative value invokes converter from symprec. */
static int relative_axes[][3] = {
    {1, 0, 0},   {0, 1, 0},   {0, 0, 1},  {-1, 0, 0},  {0, -1, 0},   /* 5 */
    {0, 0, -1},  {0, 1, 1},   {1, 0, 1},  {1, 1, 0},   {0, -1, -1},  /* 10 */
    {-1, 0, -1}, {-1, -1, 0}, {0, 1, -1}, {-1, 0, 1},  {1, -1, 0},   /* 15 */
    {0, -1, 1},  {1, 0, -1},  {-1, 1, 0}, {1, 1, 1},   {-1, -1, -1}, /* 20 */
    {-1, 1, 1},  {1, -1, 1},  {1, 1, -1}, {1, -1, -1}, {-1, 1, -1},  /* 25 */
    {-1, -1, 1},
};

static int identity[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

static int get_index_with_least_atoms(Cell const *cell);
static VecDBL *get_translation(int const rot[3][3], Cell const *cell,
                               double const symprec, int const is_identity);
static Symmetry *get_operations(Cell const *primitive, double const symprec,
                                double const angle_symprec);
static Symmetry *reduce_operation(Cell const *primitive,
                                  Symmetry const *symmetry,
                                  double const symprec,
                                  double const angle_symprec,
                                  int const is_pure_trans);
static int search_translation_part(int atoms_found[], Cell const *cell,
                                   int const rot[3][3],
                                   int const min_atom_index,
                                   double const origin[3], double const symprec,
                                   int const is_identity);
static int search_pure_translations(int atoms_found[], Cell const *cell,
                                    double const trans[3],
                                    double const symprec);
static int is_overlap_all_atoms(double const test_trans[3], int const rot[3][3],
                                Cell const *cell, double const symprec,
                                int const is_identity);
static PointSymmetry transform_pointsymmetry(
    PointSymmetry const *point_sym_prim, double const new_lattice[3][3],
    double const original_lattice[3][3]);
static Symmetry *get_space_group_operations(PointSymmetry const *lattice_sym,
                                            Cell const *primitive,
                                            double const symprec);
static void set_axes(int axes[3][3], int const a1, int const a2, int const a3);
static PointSymmetry get_lattice_symmetry(Cell const *cell,
                                          double const symprec,
                                          double const angle_symprec);
static int is_identity_metric(double const metric_rotated[3][3],
                              double const metric_orig[3][3],
                              double const symprec, double const angle_symprec);
static double get_angle(double const metric[3][3], int const i, int const j);

/* get_translation, search_translation_part and search_pure_translations */
/* are duplicated to get the if statement outside the nested loops */
/* I have not tested if it is better in efficiency. */
static VecDBL *get_layer_translation(int const rot[3][3], Cell const *cell,
                                     double const symprec,
                                     int const is_identity);
static int search_layer_translation_part(int atoms_found[], Cell const *cell,
                                         int const rot[3][3],
                                         int const min_atom_index,
                                         double const origin[3],
                                         double const symprec,
                                         int const is_identity);
static int search_layer_pure_translations(int atoms_found[], Cell const *cell,
                                          double const trans[3],
                                          int const periodic_axes[2],
                                          double const symprec);

/* Return NULL if failed */
Symmetry *sym_alloc_symmetry(int const size) {
    Symmetry *symmetry;

    symmetry = NULL;

    if (size < 1) {
        return NULL;
    }

    if ((symmetry = (Symmetry *)malloc(sizeof(Symmetry))) == NULL) {
        warning_memory("symmetry");
        return NULL;
    }

    symmetry->size = size;
    symmetry->rot = NULL;
    symmetry->trans = NULL;

    if ((symmetry->rot = (int (*)[3][3])malloc(sizeof(int[3][3]) * size)) ==
        NULL) {
        warning_memory("symmetry->rot");
        free(symmetry);
        symmetry = NULL;
        return NULL;
    }
    if ((symmetry->trans = (double (*)[3])malloc(sizeof(double[3]) * size)) ==
        NULL) {
        warning_memory("symmetry->trans");
        free(symmetry->rot);
        symmetry->rot = NULL;
        free(symmetry);
        symmetry = NULL;
        return NULL;
    }

    return symmetry;
}

void sym_free_symmetry(Symmetry *symmetry) {
    if (symmetry->size > 0) {
        free(symmetry->rot);
        symmetry->rot = NULL;
        free(symmetry->trans);
        symmetry->trans = NULL;
    }
    free(symmetry);
}

/* Return NULL if failed */
MagneticSymmetry *sym_alloc_magnetic_symmetry(int const size) {
    MagneticSymmetry *symmetry;

    symmetry = NULL;

    if (size < 1) {
        return NULL;
    }

    if ((symmetry = (MagneticSymmetry *)malloc(sizeof(MagneticSymmetry))) ==
        NULL) {
        warning_memory("symmetry");
        return NULL;
    }

    symmetry->size = size;
    symmetry->rot = NULL;
    symmetry->trans = NULL;
    symmetry->timerev = NULL;

    if ((symmetry->rot = (int (*)[3][3])malloc(sizeof(int[3][3]) * size)) ==
        NULL) {
        warning_memory("symmetry->rot");
        free(symmetry);
        symmetry = NULL;
        return NULL;
    }
    if ((symmetry->trans = (double (*)[3])malloc(sizeof(double[3]) * size)) ==
        NULL) {
        warning_memory("symmetry->trans");
        free(symmetry->rot);
        symmetry->rot = NULL;
        free(symmetry);
        symmetry = NULL;
        return NULL;
    }
    if ((symmetry->timerev = (int *)malloc(sizeof(int *) * size)) == NULL) {
        warning_memory("symmetry->timerev");
        free(symmetry->rot);
        symmetry->rot = NULL;
        free(symmetry->trans);
        symmetry->trans = NULL;
        free(symmetry);
        symmetry = NULL;
        return NULL;
    }
    return symmetry;
}

void sym_free_magnetic_symmetry(MagneticSymmetry *symmetry) {
    if (symmetry->size > 0) {
        free(symmetry->rot);
        symmetry->rot = NULL;
        free(symmetry->trans);
        symmetry->trans = NULL;
        free(symmetry->timerev);
        symmetry->timerev = NULL;
    }
    free(symmetry);
}

/* Return NULL if failed */
Symmetry *sym_get_operation(Cell const *primitive, double const symprec,
                            double const angle_tolerance) {
    debug_print("sym_get_operations:\n");

    return get_operations(primitive, symprec, angle_tolerance);
}

/* Return NULL if failed */
Symmetry *sym_reduce_operation(Cell const *primitive, Symmetry const *symmetry,
                               double const symprec,
                               double const angle_tolerance) {
    return reduce_operation(primitive, symmetry, symprec, angle_tolerance, 0);
}

/* Return NULL if failed */
VecDBL *sym_get_pure_translation(Cell const *cell, double const symprec) {
    int multi;
    VecDBL *pure_trans;

    debug_print("sym_get_pure_translation (tolerance = %f):\n", symprec);

    multi = 0;
    pure_trans = NULL;
    if (cell->aperiodic_axis == -1) {
        pure_trans = get_translation(identity, cell, symprec, 1);
    } else {
        pure_trans = get_layer_translation(identity, cell, symprec, 1);
    }
    if (pure_trans == NULL) {
        debug_print("spglib: get_translation failed.\n");
        return NULL;
    }

    multi = pure_trans->size;
    if ((cell->size / multi) * multi == cell->size) {
        debug_print("spglib: sym_get_pure_translation: pure_trans->size = %d\n",
                    multi);
    } else {
        warning_print("spglib: Finding pure translation failed.\n");
        warning_print("        cell->size %d, multi %d\n", cell->size, multi);
    }

    return pure_trans;
}

/* Return NULL if failed */
VecDBL *sym_reduce_pure_translation(Cell const *cell, VecDBL const *pure_trans,
                                    double const symprec,
                                    double const angle_tolerance) {
    int i, multi;
    Symmetry *symmetry, *symmetry_reduced;
    VecDBL *pure_trans_reduced;

    symmetry = NULL;
    symmetry_reduced = NULL;
    pure_trans_reduced = NULL;

    multi = pure_trans->size;

    if ((symmetry = sym_alloc_symmetry(multi)) == NULL) {
        return NULL;
    }

    for (i = 0; i < multi; i++) {
        mat_copy_matrix_i3(symmetry->rot[i], identity);
        mat_copy_vector_d3(symmetry->trans[i], pure_trans->vec[i]);
    }

    if ((symmetry_reduced = reduce_operation(cell, symmetry, symprec,
                                             angle_tolerance, 1)) == NULL) {
        sym_free_symmetry(symmetry);
        symmetry = NULL;
        return NULL;
    }

    sym_free_symmetry(symmetry);
    symmetry = NULL;
    multi = symmetry_reduced->size;

    if ((pure_trans_reduced = mat_alloc_VecDBL(multi)) == NULL) {
        sym_free_symmetry(symmetry_reduced);
        symmetry_reduced = NULL;
        return NULL;
    }

    for (i = 0; i < multi; i++) {
        mat_copy_vector_d3(pure_trans_reduced->vec[i],
                           symmetry_reduced->trans[i]);
    }
    sym_free_symmetry(symmetry_reduced);
    symmetry_reduced = NULL;

    return pure_trans_reduced;
}

/* Warning! Comment 1 does not seem to happen. There is nothing about input
 * cell.*/
/* 1) Pointgroup operations of the primitive cell are obtained. */
/*    These are constrained by the input cell lattice pointgroup, */
/*    i.e., even if the lattice of the primitive cell has higher */
/*    symmetry than that of the input cell, it is not considered. */
/* 2) Spacegroup operations are searched for the primitive cell */
/*    using the constrained point group operations. */
/* 3) The spacegroup operations for the primitive cell are */
/*    transformed to those of original input cells, if the input cell */
/*    was not a primitive cell. */
static Symmetry *get_operations(Cell const *primitive, double const symprec,
                                double const angle_symprec) {
    PointSymmetry lattice_sym;
    Symmetry *symmetry;

    debug_print("get_operations:\n");

    symmetry = NULL;

    lattice_sym = get_lattice_symmetry(primitive, symprec, angle_symprec);
    if (lattice_sym.size == 0) {
        return NULL;
    }

    if ((symmetry = get_space_group_operations(&lattice_sym, primitive,
                                               symprec)) == NULL) {
        return NULL;
    }

    return symmetry;
}

/* Return NULL if failed */
static Symmetry *reduce_operation(Cell const *primitive,
                                  Symmetry const *symmetry,
                                  double const symprec,
                                  double const angle_symprec,
                                  int const is_pure_trans) {
    int i, j, num_sym;
    Symmetry *sym_reduced;
    PointSymmetry point_symmetry;
    MatINT *rot;
    VecDBL *trans;

    debug_print("reduce_operation:\n");

    sym_reduced = NULL;
    rot = NULL;
    trans = NULL;

    if (is_pure_trans) {
        point_symmetry.size = 1;
        mat_copy_matrix_i3(point_symmetry.rot[0], identity);
    } else {
        point_symmetry =
            get_lattice_symmetry(primitive, symprec, angle_symprec);
        if (point_symmetry.size == 0) {
            return NULL;
        }
    }

    if ((rot = mat_alloc_MatINT(symmetry->size)) == NULL) {
        return NULL;
    }

    if ((trans = mat_alloc_VecDBL(symmetry->size)) == NULL) {
        mat_free_MatINT(rot);
        rot = NULL;
        return NULL;
    }

    num_sym = 0;
    for (i = 0; i < point_symmetry.size; i++) {
        for (j = 0; j < symmetry->size; j++) {
            if (mat_check_identity_matrix_i3(point_symmetry.rot[i],
                                             symmetry->rot[j])) {
                if (is_overlap_all_atoms(symmetry->trans[j], symmetry->rot[j],
                                         primitive, symprec, 0)) {
                    mat_copy_matrix_i3(rot->mat[num_sym], symmetry->rot[j]);
                    mat_copy_vector_d3(trans->vec[num_sym], symmetry->trans[j]);
                    num_sym++;
                }
            }
        }
    }

    if ((sym_reduced = sym_alloc_symmetry(num_sym)) != NULL) {
        for (i = 0; i < num_sym; i++) {
            mat_copy_matrix_i3(sym_reduced->rot[i], rot->mat[i]);
            mat_copy_vector_d3(sym_reduced->trans[i], trans->vec[i]);
        }
    }

    mat_free_MatINT(rot);
    rot = NULL;
    mat_free_VecDBL(trans);
    trans = NULL;

    return sym_reduced;
}

/* Look for the translations which satisfy the input symmetry operation. */
/* This function is heaviest in this code. */
/* Return NULL if failed */
static VecDBL *get_translation(int const rot[3][3], Cell const *cell,
                               double const symprec, int const is_identity) {
    int i, j, k, min_atom_index, num_trans;
    int *is_found;
    double origin[3];
    VecDBL *trans;

    debug_print("get_translation (tolerance = %f):\n", symprec);

    num_trans = 0;
    is_found = NULL;
    trans = NULL;

    if ((is_found = (int *)malloc(sizeof(int) * cell->size)) == NULL) {
        warning_memory("is_found");
        return NULL;
    }

    for (i = 0; i < cell->size; i++) {
        is_found[i] = 0;
    }

    /* Look for the atom index with least number of atoms within same type */
    min_atom_index = get_index_with_least_atoms(cell);
    if (min_atom_index == -1) {
        debug_print("spglib: get_index_with_least_atoms failed.\n");
        goto ret;
    }

    /* Set min_atom_index as the origin to measure the distance between atoms.
     */
    mat_multiply_matrix_vector_id3(origin, rot, cell->position[min_atom_index]);

    num_trans = search_translation_part(is_found, cell, rot, min_atom_index,
                                        origin, symprec, is_identity);
    if (num_trans == -1 || num_trans == 0) {
        goto ret;
    }

    if ((trans = mat_alloc_VecDBL(num_trans)) == NULL) {
        goto ret;
    }

    k = 0;
    for (i = 0; i < cell->size; i++) {
        if (is_found[i]) {
            for (j = 0; j < 3; j++) {
                trans->vec[k][j] = cell->position[i][j] - origin[j];
                trans->vec[k][j] = mat_Dmod1(trans->vec[k][j]);
            }
            k++;
        }
    }

ret:
    free(is_found);
    is_found = NULL;

    return trans;
}

/* Returns -1 on failure. */
static int search_translation_part(int atoms_found[], Cell const *cell,
                                   int const rot[3][3],
                                   int const min_atom_index,
                                   double const origin[3], double const symprec,
                                   int const is_identity) {
    int i, j, num_trans, is_overlap;
    double trans[3];
    OverlapChecker *checker;

    checker = NULL;

    if ((checker = ovl_overlap_checker_init(cell)) == NULL) {
        return -1;
    }

    num_trans = 0;

    for (i = 0; i < cell->size; i++) {
        if (atoms_found[i]) {
            continue;
        }

        if (cell->types[i] != cell->types[min_atom_index]) {
            continue;
        }

        for (j = 0; j < 3; j++) {
            trans[j] = cell->position[i][j] - origin[j];
        }

        is_overlap =
            ovl_check_total_overlap(checker, trans, rot, symprec, is_identity);
        if (is_overlap == -1) {
            goto err;
        } else if (is_overlap) {
            atoms_found[i] = 1;
            num_trans++;
            if (is_identity) {
                num_trans +=
                    search_pure_translations(atoms_found, cell, trans, symprec);
            }
        }
    }

    ovl_overlap_checker_free(checker);
    checker = NULL;
    return num_trans;

err:
    ovl_overlap_checker_free(checker);
    checker = NULL;
    return -1;
}

static int search_pure_translations(int atoms_found[], Cell const *cell,
                                    double const trans[3],
                                    double const symprec) {
    int i, j, num_trans, i_atom, initial_atom;
    int *copy_atoms_found;
    double vec[3];

    num_trans = 0;

    copy_atoms_found = (int *)malloc(sizeof(int) * cell->size);
    for (i = 0; i < cell->size; i++) {
        copy_atoms_found[i] = atoms_found[i];
    }

    for (initial_atom = 0; initial_atom < cell->size; initial_atom++) {
        if (!copy_atoms_found[initial_atom]) {
            continue;
        }

        i_atom = initial_atom;

        for (i = 0; i < cell->size; i++) {
            for (j = 0; j < 3; j++) {
                vec[j] = cell->position[i_atom][j] + trans[j];
            }

            for (j = 0; j < cell->size; j++) {
                if (cel_is_overlap_with_same_type(
                        vec, cell->position[j], cell->types[i_atom],
                        cell->types[j], cell->lattice, symprec)) {
                    if (!atoms_found[j]) {
                        atoms_found[j] = 1;
                        num_trans++;
                    }
                    i_atom = j;

                    break;
                }
            }

            if (i_atom == initial_atom) {
                break;
            }
        }
    }

    free(copy_atoms_found);

    return num_trans;
}

/* Thoroughly confirms that a given symmetry operation is a symmetry. */
/* This is a convenient wrapper around ovl_check_total_overlap. */
/* -1: Error.  0: Not a symmetry.  1: Is a symmetry. */
static int is_overlap_all_atoms(double const trans[3], int const rot[3][3],
                                Cell const *cell, double const symprec,
                                int const is_identity) {
    OverlapChecker *checker;
    int result;

    checker = NULL;

    if ((checker = ovl_overlap_checker_init(cell)) == NULL) {
        return -1;
    }

    if (cell->aperiodic_axis == -1) {
        result =
            ovl_check_total_overlap(checker, trans, rot, symprec, is_identity);
    } else {
        result = ovl_check_layer_total_overlap(checker, trans, rot, symprec,
                                               is_identity);
    }

    ovl_overlap_checker_free(checker);
    checker = NULL;

    return result;
}

static int get_index_with_least_atoms(Cell const *cell) {
    int i, j, min, min_index;
    int *mapping;

    mapping = NULL;

    if ((mapping = (int *)malloc(sizeof(int) * cell->size)) == NULL) {
        warning_memory("mapping");
        return -1;
    }

    for (i = 0; i < cell->size; i++) {
        mapping[i] = 0;
    }

    for (i = 0; i < cell->size; i++) {
        for (j = 0; j < cell->size; j++) {
            if (cell->types[i] == cell->types[j]) {
                mapping[j]++;
                break;
            }
        }
    }

    min = mapping[0];
    min_index = 0;
    for (i = 0; i < cell->size; i++) {
        if (min > mapping[i] && mapping[i] > 0) {
            min = mapping[i];
            min_index = i;
        }
    }

    free(mapping);
    mapping = NULL;

    return min_index;
}

/* Look for the translations which satisfy the input symmetry operation. */
/* This function is heaviest in this code. */
/* Return NULL if failed */
static VecDBL *get_layer_translation(int const rot[3][3], Cell const *cell,
                                     double const symprec,
                                     int const is_identity) {
    int i, j, k, min_atom_index, num_trans;
    int *is_found;
    double origin[3];
    VecDBL *trans;

    debug_print("get_translation (tolerance = %f):\n", symprec);

    num_trans = 0;
    is_found = NULL;
    trans = NULL;

    if ((is_found = (int *)malloc(sizeof(int) * cell->size)) == NULL) {
        warning_memory("is_found");
        return NULL;
    }

    for (i = 0; i < cell->size; i++) {
        is_found[i] = 0;
    }

    /* Look for the atom index with least number of atoms within same type */
    min_atom_index = get_index_with_least_atoms(cell);
    if (min_atom_index == -1) {
        debug_print("spglib: get_index_with_least_atoms failed.\n");
        goto ret;
    }

    /* Set min_atom_index as the origin to measure the distance between atoms.
     */
    mat_multiply_matrix_vector_id3(origin, rot, cell->position[min_atom_index]);

    num_trans = search_layer_translation_part(
        is_found, cell, rot, min_atom_index, origin, symprec, is_identity);
    if (num_trans == -1 || num_trans == 0) {
        goto ret;
    }

    if ((trans = mat_alloc_VecDBL(num_trans)) == NULL) {
        goto ret;
    }

    k = 0;
    for (i = 0; i < cell->size; i++) {
        if (is_found[i]) {
            for (j = 0; j < 3; j++) {
                trans->vec[k][j] = cell->position[i][j] - origin[j];
                if (j != cell->aperiodic_axis) {
                    trans->vec[k][j] = mat_Dmod1(trans->vec[k][j]);
                }
            }
            k++;
        }
    }

ret:
    free(is_found);
    is_found = NULL;

    return trans;
}

/* Returns -1 on failure. */
static int search_layer_translation_part(int atoms_found[], Cell const *cell,
                                         int const rot[3][3],
                                         int const min_atom_index,
                                         double const origin[3],
                                         double const symprec,
                                         int const is_identity) {
    int i, j, num_trans, is_overlap;
    double trans[3];
    OverlapChecker *checker;

    checker = NULL;

    if ((checker = ovl_overlap_checker_init(cell)) == NULL) {
        return -1;
    }

    num_trans = 0;

    for (i = 0; i < cell->size; i++) {
        if (atoms_found[i]) {
            continue;
        }

        if (cell->types[i] != cell->types[min_atom_index]) {
            continue;
        }

        for (j = 0; j < 3; j++) {
            trans[j] = cell->position[i][j] - origin[j];
        }

        is_overlap = ovl_check_layer_total_overlap(checker, trans, rot, symprec,
                                                   is_identity);
        if (is_overlap == -1) {
            goto err;
        } else if (is_overlap) {
            atoms_found[i] = 1;
            num_trans++;
            if (is_identity) {
                num_trans += search_layer_pure_translations(
                    atoms_found, cell, trans, checker->periodic_axes, symprec);
            }
        }
    }

    ovl_overlap_checker_free(checker);
    checker = NULL;
    return num_trans;

err:
    ovl_overlap_checker_free(checker);
    checker = NULL;
    return -1;
}

static int search_layer_pure_translations(int atoms_found[], Cell const *cell,
                                          double const trans[3],
                                          int const periodic_axes[2],
                                          double const symprec) {
    int i, j, num_trans, i_atom, initial_atom;
    int *copy_atoms_found;
    double vec[3];

    num_trans = 0;

    copy_atoms_found = (int *)malloc(sizeof(int) * cell->size);
    for (i = 0; i < cell->size; i++) {
        copy_atoms_found[i] = atoms_found[i];
    }

    for (initial_atom = 0; initial_atom < cell->size; initial_atom++) {
        if (!copy_atoms_found[initial_atom]) {
            continue;
        }

        i_atom = initial_atom;

        for (i = 0; i < cell->size; i++) {
            for (j = 0; j < 3; j++) {
                vec[j] = cell->position[i_atom][j] + trans[j];
            }

            for (j = 0; j < cell->size; j++) {
                if (cel_layer_is_overlap_with_same_type(
                        vec, cell->position[j], cell->types[i_atom],
                        cell->types[j], cell->lattice, periodic_axes,
                        symprec)) {
                    if (!atoms_found[j]) {
                        atoms_found[j] = 1;
                        num_trans++;
                    }
                    i_atom = j;

                    break;
                }
            }

            if (i_atom == initial_atom) {
                break;
            }
        }
    }

    free(copy_atoms_found);

    return num_trans;
}

/* Return NULL if failed */
static Symmetry *get_space_group_operations(PointSymmetry const *lattice_sym,
                                            Cell const *primitive,
                                            double const symprec) {
    int i, j, num_sym, total_num_sym;
    VecDBL **trans;
    Symmetry *symmetry;

    debug_print("get_space_group_operations (tolerance = %f):\n", symprec);

    trans = NULL;
    symmetry = NULL;

    if ((trans = (VecDBL **)malloc(sizeof(VecDBL *) * lattice_sym->size)) ==
        NULL) {
        warning_memory("trans");
        return NULL;
    }

    for (i = 0; i < lattice_sym->size; i++) {
        trans[i] = NULL;
    }

    total_num_sym = 0;

    if (primitive->aperiodic_axis == -1) {
        for (i = 0; i < lattice_sym->size; i++) {
            if ((trans[i] = get_translation(lattice_sym->rot[i], primitive,
                                            symprec, 0)) != NULL) {
                debug_print("  match translation %d/%d; tolerance = %f\n",
                            i + 1, lattice_sym->size, symprec);

                total_num_sym += trans[i]->size;
            }
        }
    } else {
        for (i = 0; i < lattice_sym->size; i++) {
            if ((trans[i] = get_layer_translation(
                     lattice_sym->rot[i], primitive, symprec, 0)) != NULL) {
                debug_print("  match translation %d/%d; tolerance = %f\n",
                            i + 1, lattice_sym->size, symprec);

                total_num_sym += trans[i]->size;
            }
        }
    }

    if ((symmetry = sym_alloc_symmetry(total_num_sym)) == NULL) {
        goto ret;
    }

    num_sym = 0;
    for (i = 0; i < lattice_sym->size; i++) {
        if (trans[i] == NULL) {
            continue;
        }
        for (j = 0; j < trans[i]->size; j++) {
            mat_copy_vector_d3(symmetry->trans[num_sym + j], trans[i]->vec[j]);
            mat_copy_matrix_i3(symmetry->rot[num_sym + j], lattice_sym->rot[i]);
        }
        num_sym += trans[i]->size;
    }

ret:
    for (i = 0; i < lattice_sym->size; i++) {
        if (trans[i] != NULL) {
            mat_free_VecDBL(trans[i]);
            trans[i] = NULL;
        }
    }
    free(trans);
    trans = NULL;

    return symmetry;
}

/* lattice_sym.size = 0 is returned if failed. */
static PointSymmetry get_lattice_symmetry(Cell const *cell,
                                          double const symprec,
                                          double const angle_symprec) {
    int i, j, k, attempt, num_sym, aperiodic_axis;
    double angle_tol;
    int axes[3][3];
    double lattice[3][3], min_lattice[3][3];
    double metric[3][3], metric_orig[3][3];
    PointSymmetry lattice_sym;

    debug_print("get_lattice_symmetry:\n");

    lattice_sym.size = 0;

    aperiodic_axis = cell->aperiodic_axis;

    if (aperiodic_axis == -1) {
        if (!del_delaunay_reduce(min_lattice, cell->lattice, symprec)) {
            goto err;
        }
    } else {
        if (!del_layer_delaunay_reduce(min_lattice, cell->lattice,
                                       aperiodic_axis, symprec)) {
            goto err;
        }
    }

    mat_get_metric(metric_orig, min_lattice);
    angle_tol = angle_symprec;

    for (attempt = 0; attempt < NUM_ATTEMPT; attempt++) {
        num_sym = 0;
        for (i = 0; i < 26; i++) {
            for (j = 0; j < 26; j++) {
                for (k = 0; k < 26; k++) {
                    set_axes(axes, i, j, k);
                    /* For layer groups, the off-diagonal elements for the
                     * aperiodic axis are set to be zero.
                     * Please note that for inclined aperiodic axis, some
                     * operations will not be found */
                    switch (aperiodic_axis) {
                        case 2:
                            /*    {{W_11, W_12,      0}, *
                             * W = {W_21, W_22,      0}, *
                             *     {   0,    0, (+/-)1}} */
                            if (axes[0][2] || axes[1][2] || axes[2][0] ||
                                axes[2][1])
                                continue;
                            break;
                        case 0:
                            /*    {{(+/-)1,    0,    0}, *
                             * W = {     0, W_22, W_23}, *
                             *     {     0, W_32, W_33}} */
                            if (axes[0][1] || axes[0][2] || axes[1][0] ||
                                axes[2][0])
                                continue;
                            break;
                        case 1:
                            /*    {{W_11,      0, W_13}, *
                             * W = {   0, (+/-)1,    0}, *
                             *     {W_31,      0, W_33}} */
                            if (axes[0][1] || axes[1][0] || axes[1][2] ||
                                axes[2][1])
                                continue;
                            break;
                        default:
                            break;
                    }
                    if (!((mat_get_determinant_i3(axes) == 1) ||
                          (mat_get_determinant_i3(axes) == -1))) {
                        continue;
                    }
                    mat_multiply_matrix_di3(lattice, min_lattice, axes);
                    mat_get_metric(metric, lattice);

                    if (is_identity_metric(metric, metric_orig, symprec,
                                           angle_tol)) {
                        if ((aperiodic_axis == -1 && num_sym >= 48) ||
                            (aperiodic_axis != -1 && num_sym >= 24)) {
                            debug_print(
                                "spglib: Too many lattice symmetries were "
                                "found.\n");
                            if (angle_tol > 0) {
                                angle_tol *= ANGLE_REDUCE_RATE;
                                debug_print(
                                    "        Reducing angle tolerance to %f\n",
                                    angle_tol);
                            }
                            goto next_attempt;
                        }

                        mat_copy_matrix_i3(lattice_sym.rot[num_sym], axes);
                        num_sym++;
                    }
                }
            }
        }

        if ((aperiodic_axis == -1 && num_sym <= 48) ||
            (aperiodic_axis != -1 && num_sym <= 24) || angle_tol < 0) {
            lattice_sym.size = num_sym;
            return transform_pointsymmetry(&lattice_sym, cell->lattice,
                                           min_lattice);
        }

    next_attempt:;
    }

err:
    debug_print("get_lattice_symmetry failed.\n");
    return lattice_sym;
}

static int is_identity_metric(double const metric_rotated[3][3],
                              double const metric_orig[3][3],
                              double const symprec,
                              double const angle_symprec) {
    int i, j, k;
    int elem_sets[3][2] = {{0, 1}, {0, 2}, {1, 2}};
    double cos1, cos2, x, length_ave2, sin_dtheta2;
    double length_orig[3], length_rot[3];

    for (i = 0; i < 3; i++) {
        length_orig[i] = sqrt(metric_orig[i][i]);
        length_rot[i] = sqrt(metric_rotated[i][i]);
        if (mat_Dabs(length_orig[i] - length_rot[i]) > symprec) {
            goto fail;
        }
    }

    for (i = 0; i < 3; i++) {
        j = elem_sets[i][0];
        k = elem_sets[i][1];
        if (angle_symprec > 0) {
            if (mat_Dabs(get_angle(metric_orig, j, k) -
                         get_angle(metric_rotated, j, k)) > angle_symprec) {
                goto fail;
            }
        } else {
            /* dtheta = arccos(cos(theta1) - arccos(cos(theta2))) */
            /*        = arccos(c1) - arccos(c2) */
            /*        = arccos(c1c2 + sqrt((1-c1^2)(1-c2^2))) */
            /* sin(dtheta) = sin(arccos(x)) = sqrt(1 - x^2) */
            cos1 = metric_orig[j][k] / length_orig[j] / length_orig[k];
            cos2 = metric_rotated[j][k] / length_rot[j] / length_rot[k];
            x = cos1 * cos2 + sqrt(1 - cos1 * cos1) * sqrt(1 - cos2 * cos2);
            sin_dtheta2 = 1 - x * x;
            length_ave2 = ((length_orig[j] + length_rot[j]) *
                           (length_orig[k] + length_rot[k])) /
                          4;
            if (sin_dtheta2 > SIN_DTHETA2_CUTOFF) {
                if (sin_dtheta2 * length_ave2 > symprec * symprec) {
                    goto fail;
                }
            }
        }
    }

    return 1;

fail:
    return 0;
}

static double get_angle(double const metric[3][3], int const i, int const j) {
    double length_i, length_j;

    length_i = sqrt(metric[i][i]);
    length_j = sqrt(metric[j][j]);

    return acos(metric[i][j] / length_i / length_j) / PI * 180;
}

static PointSymmetry transform_pointsymmetry(
    PointSymmetry const *lat_sym_orig, double const new_lattice[3][3],
    double const original_lattice[3][3]) {
    int i, size;
    double trans_mat[3][3], inv_mat[3][3], drot[3][3];
    PointSymmetry lat_sym_new;

    lat_sym_new.size = 0;

    mat_inverse_matrix_d3(inv_mat, original_lattice, 0);
    mat_multiply_matrix_d3(trans_mat, inv_mat, new_lattice);

    size = 0;
    for (i = 0; i < lat_sym_orig->size; i++) {
        mat_cast_matrix_3i_to_3d(drot, lat_sym_orig->rot[i]);
        mat_get_similar_matrix_d3(drot, drot, trans_mat, 0);

        /* new_lattice may have lower point symmetry than original_lattice.*/
        /* The operations that have non-integer elements are not counted. */
        if (mat_is_int_matrix(
                drot, mat_Dabs(mat_get_determinant_d3(trans_mat)) / 10)) {
            mat_cast_matrix_3d_to_3i(lat_sym_new.rot[size], drot);
            if (abs(mat_get_determinant_i3(lat_sym_new.rot[size])) != 1) {
                warning_print(
                    "spglib: A point symmetry operation is not unimodular.\n");
                goto err;
            }
            size++;
        }
    }

    if (!(lat_sym_orig->size == size)) {
        warning_print(
            "spglib: Some of point symmetry operations were dropped.\n");
    }

    lat_sym_new.size = size;
    return lat_sym_new;

err:
    return lat_sym_new;
}

// @brief Set an integer matrix to `axes`. Three integer vectors are specified
//        by indices of array `symmetry.c:relative_axes`.
static void set_axes(int axes[3][3], int const a1, int const a2, int const a3) {
    int i;
    for (i = 0; i < 3; i++) {
        axes[i][0] = relative_axes[a1][i];
    }
    for (i = 0; i < 3; i++) {
        axes[i][1] = relative_axes[a2][i];
    }
    for (i = 0; i < 3; i++) {
        axes[i][2] = relative_axes[a3][i];
    }
}
