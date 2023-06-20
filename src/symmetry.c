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
static const int relative_axes[][3] = {
    {1, 0, 0},   {0, 1, 0},   {0, 0, 1},  {-1, 0, 0},  {0, -1, 0},   /* 5 */
    {0, 0, -1},  {0, 1, 1},   {1, 0, 1},  {1, 1, 0},   {0, -1, -1},  /* 10 */
    {-1, 0, -1}, {-1, -1, 0}, {0, 1, -1}, {-1, 0, 1},  {1, -1, 0},   /* 15 */
    {0, -1, 1},  {1, 0, -1},  {-1, 1, 0}, {1, 1, 1},   {-1, -1, -1}, /* 20 */
    {-1, 1, 1},  {1, -1, 1},  {1, 1, -1}, {1, -1, -1}, {-1, 1, -1},  /* 25 */
    {-1, -1, 1},
};

static const int identity[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

static int get_index_with_least_atoms(const Cell *cell);
static VecDBL *get_translation(const int rot[3][3], const Cell *cell,
                               double symprec, int is_identity);
static Symmetry *get_operations(const Cell *primitive, double symprec,
                                double angle_symprec);
static Symmetry *reduce_operation(const Cell *primitive,
                                  const Symmetry *symmetry, double symprec,
                                  double angle_symprec, int is_pure_trans);
static int search_translation_part(int atoms_found[], const Cell *cell,
                                   const int rot[3][3], int min_atom_index,
                                   const double origin[3], double symprec,
                                   int is_identity);
static int search_pure_translations(int atoms_found[], const Cell *cell,
                                    const double trans[3], double symprec);
static int is_overlap_all_atoms(const double test_trans[3], const int rot[3][3],
                                const Cell *cell, double symprec,
                                int is_identity);
static PointSymmetry transform_pointsymmetry(
    const PointSymmetry *point_sym_prim, const double new_lattice[3][3],
    const double original_lattice[3][3]);
static Symmetry *get_space_group_operations(const PointSymmetry *lattice_sym,
                                            const Cell *primitive,
                                            double symprec);
static void set_axes(int axes[3][3], int a1, int a2, int a3);
static PointSymmetry get_lattice_symmetry(const Cell *cell, double symprec,
                                          double angle_symprec);
static int is_identity_metric(const double metric_rotated[3][3],
                              const double metric_orig[3][3], double symprec,
                              double angle_symprec);
static double get_angle(const double metric[3][3], int i, int j);

/* get_translation, search_translation_part and search_pure_translations */
/* are duplicated to get the if statement outside the nested loops */
/* I have not tested if it is better in efficiency. */
static VecDBL *get_layer_translation(const int rot[3][3], const Cell *cell,
                                     double symprec, int is_identity);
static int search_layer_translation_part(int atoms_found[], const Cell *cell,
                                         const int rot[3][3],
                                         int min_atom_index,
                                         const double origin[3], double symprec,
                                         int is_identity);
static int search_layer_pure_translations(int atoms_found[], const Cell *cell,
                                          const double trans[3],
                                          const int periodic_axes[2],
                                          double symprec);

/* Return NULL if failed */
Symmetry *sym_alloc_symmetry(const int size) {
    if (size < 1) {
        return NULL;
    }

    Symmetry *symmetry = (Symmetry *)malloc(sizeof(Symmetry));
    if (symmetry == NULL) {
        warning_print("spglib: Memory could not be allocated ");
        return NULL;
    }

    symmetry->size = size;
    symmetry->rot = NULL;
    symmetry->trans = NULL;

    symmetry->rot = (int(*)[3][3])malloc(sizeof(int[3][3]) * size);
    if (symmetry->rot == NULL) {
        warning_print("spglib: Memory could not be allocated ");
        warning_print("(line %d, %s).\n", __LINE__, __FILE__);
        sym_free_symmetry(symmetry);
        symmetry = NULL;
        return NULL;
    }
    symmetry->trans = (double(*)[3])malloc(sizeof(double[3]) * size);
    if (symmetry->trans == NULL) {
        warning_print("spglib: Memory could not be allocated ");
        warning_print("(line %d, %s).\n", __LINE__, __FILE__);
        sym_free_symmetry(symmetry);
        symmetry = NULL;
        return NULL;
    }

    return symmetry;
}

void sym_free_symmetry(Symmetry *symmetry) {
    if (symmetry->rot != NULL) {
        free(symmetry->rot);
        symmetry->rot = NULL;
    }
    if (symmetry->trans != NULL) {
        free(symmetry->trans);
        symmetry->trans = NULL;
    }
    free(symmetry);
}

/* Return NULL if failed */
MagneticSymmetry *sym_alloc_magnetic_symmetry(const int size) {
    if (size < 1) {
        return NULL;
    }

    MagneticSymmetry *symmetry =
        (MagneticSymmetry *)malloc(sizeof(MagneticSymmetry));
    if (symmetry == NULL) {
        warning_print("spglib: Memory could not be allocated ");
        return NULL;
    }

    symmetry->size = size;
    symmetry->rot = NULL;
    symmetry->trans = NULL;
    symmetry->timerev = NULL;

    symmetry->rot = (int(*)[3][3])malloc(sizeof(int[3][3]) * size);
    if (symmetry->rot == NULL) {
        warning_print("spglib: Memory could not be allocated ");
        warning_print("(line %d, %s).\n", __LINE__, __FILE__);
        sym_free_magnetic_symmetry(symmetry);
        symmetry = NULL;
        return NULL;
    }
    symmetry->trans = (double(*)[3])malloc(sizeof(double[3]) * size);
    if (symmetry->trans == NULL) {
        warning_print("spglib: Memory could not be allocated ");
        warning_print("(line %d, %s).\n", __LINE__, __FILE__);
        sym_free_magnetic_symmetry(symmetry);
        symmetry = NULL;
        return NULL;
    }
    symmetry->timerev = (int *)malloc(sizeof(int *) * size);
    if (symmetry->timerev == NULL) {
        warning_print("spglib: Memory could not be allocated ");
        warning_print("(line %d, %s).\n", __LINE__, __FILE__);
        sym_free_magnetic_symmetry(symmetry);
        symmetry = NULL;
        return NULL;
    }
    return symmetry;
}

void sym_free_magnetic_symmetry(MagneticSymmetry *symmetry) {
    if (symmetry->rot != NULL) {
        free(symmetry->rot);
        symmetry->rot = NULL;
    }
    if (symmetry->trans != NULL) {
        free(symmetry->trans);
        symmetry->trans = NULL;
    }
    if (symmetry->timerev) {
        free(symmetry->timerev);
        symmetry->timerev = NULL;
    }
    free(symmetry);
}

/* Return NULL if failed */
Symmetry *sym_get_operation(const Cell *primitive, const double symprec,
                            const double angle_tolerance) {
    debug_print("sym_get_operations:\n");

    return get_operations(primitive, symprec, angle_tolerance);
}

/* Return NULL if failed */
Symmetry *sym_reduce_operation(const Cell *primitive, const Symmetry *symmetry,
                               const double symprec,
                               const double angle_tolerance) {
    return reduce_operation(primitive, symmetry, symprec, angle_tolerance, 0);
}

/* Return NULL if failed */
VecDBL *sym_get_pure_translation(const Cell *cell, const double symprec) {
    debug_print("sym_get_pure_translation (tolerance = %f):\n", symprec);

    VecDBL *pure_trans =
        (cell->aperiodic_axis == -1)
            ? get_translation(identity, cell, symprec, 1)
            : get_layer_translation(identity, cell, symprec, 1);
    if (pure_trans == NULL) {
        warning_print("spglib: get_translation failed (line %d, %s).\n",
                      __LINE__, __FILE__);
        return NULL;
    }

    int multi = pure_trans->size;
    if ((cell->size / multi) * multi == cell->size) {
        debug_print("  sym_get_pure_translation: pure_trans->size = %d\n",
                    multi);
    } else {
        warning_print(
            "spglib: Finding pure translation failed (line %d, %s).\n",
            __LINE__, __FILE__);
        warning_print("        cell->size %d, multi %d\n", cell->size, multi);
        mat_free_VecDBL(pure_trans);
        pure_trans = NULL;
        return NULL;
    }

    return pure_trans;
}

/* Return NULL if failed */
VecDBL *sym_reduce_pure_translation(const Cell *cell, const VecDBL *pure_trans,
                                    const double symprec,
                                    const double angle_tolerance) {
    int multi = pure_trans->size;

    Symmetry *symmetry = sym_alloc_symmetry(multi);
    if (symmetry == NULL) {
        return NULL;
    }

    for (int i = 0; i < multi; i++) {
        mat_copy_matrix_i3(symmetry->rot[i], identity);
        mat_copy_vector_d3(symmetry->trans[i], pure_trans->vec[i]);
    }

    Symmetry *symmetry_reduced =
        reduce_operation(cell, symmetry, symprec, angle_tolerance, 1);
    sym_free_symmetry(symmetry);
    symmetry = NULL;
    if (symmetry_reduced == NULL) {
        return NULL;
    }

    multi = symmetry_reduced->size;

    VecDBL *pure_trans_reduced = mat_alloc_VecDBL(multi);
    if (pure_trans_reduced == NULL) {
        sym_free_symmetry(symmetry_reduced);
        symmetry_reduced = NULL;
        return NULL;
    }

    for (int i = 0; i < multi; i++) {
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
static Symmetry *get_operations(const Cell *primitive, const double symprec,
                                const double angle_symprec) {
    debug_print("get_operations:\n");

    PointSymmetry lattice_sym =
        get_lattice_symmetry(primitive, symprec, angle_symprec);
    if (lattice_sym.size == 0) {
        return NULL;
    }

    return get_space_group_operations(&lattice_sym, primitive, symprec);
}

/* Return NULL if failed */
static Symmetry *reduce_operation(const Cell *primitive,
                                  const Symmetry *symmetry,
                                  const double symprec,
                                  const double angle_symprec,
                                  const int is_pure_trans) {
    debug_print("reduce_operation:\n");
    PointSymmetry point_symmetry;
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

    MatINT *rot = mat_alloc_MatINT(symmetry->size);
    if (rot == NULL) {
        return NULL;
    }

    VecDBL *trans = mat_alloc_VecDBL(symmetry->size);
    if (trans == NULL) {
        mat_free_MatINT(rot);
        rot = NULL;
        return NULL;
    }

    int num_sym = 0;
    for (int i = 0; i < point_symmetry.size; i++) {
        for (int j = 0; j < symmetry->size; j++) {
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

    Symmetry *sym_reduced = sym_alloc_symmetry(num_sym);
    if (sym_reduced != NULL) {
        for (int i = 0; i < num_sym; i++) {
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
static VecDBL *get_translation(const int rot[3][3], const Cell *cell,
                               const double symprec, const int is_identity) {
    double origin[3];

    debug_print("get_translation (tolerance = %f):\n", symprec);
    int *is_found = (int *)malloc(sizeof(int) * cell->size);
    if (is_found == NULL) {
        warning_print("spglib: Memory could not be allocated ");
        return NULL;
    }

    for (int i = 0; i < cell->size; i++) {
        is_found[i] = 0;
    }

    /* Look for the atom index with least number of atoms within same type */
    int min_atom_index = get_index_with_least_atoms(cell);
    if (min_atom_index == -1) {
        debug_print("spglib: get_index_with_least_atoms failed.\n");
        free(is_found);
        is_found = NULL;
        return NULL;
    }

    /* Set min_atom_index as the origin to measure the distance between atoms.
     */
    mat_multiply_matrix_vector_id3(origin, rot, cell->position[min_atom_index]);

    int num_trans = search_translation_part(is_found, cell, rot, min_atom_index,
                                            origin, symprec, is_identity);
    if (num_trans == -1 || num_trans == 0) {
        free(is_found);
        is_found = NULL;
        return NULL;
    }

    VecDBL *trans = mat_alloc_VecDBL(num_trans);
    if (trans == NULL) {
        free(is_found);
        is_found = NULL;
        return NULL;
    }

    for (int i = 0, k = 0; i < cell->size; i++) {
        if (is_found[i]) {
            for (int j = 0; j < 3; j++) {
                trans->vec[k][j] = cell->position[i][j] - origin[j];
                trans->vec[k][j] = mat_Dmod1(trans->vec[k][j]);
            }
            k++;
        }
    }
    free(is_found);
    is_found = NULL;

    return trans;
}

/* Returns -1 on failure. */
static int search_translation_part(int atoms_found[], const Cell *cell,
                                   const int rot[3][3],
                                   const int min_atom_index,
                                   const double origin[3], const double symprec,
                                   const int is_identity) {
    double trans[3];
    OverlapChecker *checker = ovl_overlap_checker_init(cell);
    if (checker == NULL) {
        return -1;
    }

    int num_trans = 0;
    for (int i = 0; i < cell->size; i++) {
        if (atoms_found[i]) {
            continue;
        }

        if (cell->types[i] != cell->types[min_atom_index]) {
            continue;
        }

        for (int j = 0; j < 3; j++) {
            trans[j] = cell->position[i][j] - origin[j];
        }

        int is_overlap =
            ovl_check_total_overlap(checker, trans, rot, symprec, is_identity);
        if (is_overlap == -1) {
            ovl_overlap_checker_free(checker);
            checker = NULL;
            return -1;
        }
        if (is_overlap) {
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
}

static int search_pure_translations(int atoms_found[], const Cell *cell,
                                    const double trans[3],
                                    const double symprec) {
    double vec[3];

    int *copy_atoms_found = (int *)malloc(sizeof(int) * cell->size);
    if (copy_atoms_found == NULL) return 0;
    for (int i = 0; i < cell->size; i++) {
        copy_atoms_found[i] = atoms_found[i];
    }

    int num_trans = 0;
    for (int initial_atom = 0; initial_atom < cell->size; initial_atom++) {
        if (!copy_atoms_found[initial_atom]) {
            continue;
        }

        for (int i = 0, i_atom = initial_atom; i < cell->size; i++) {
            for (int j = 0; j < 3; j++) {
                vec[j] = cell->position[i_atom][j] + trans[j];
            }

            for (int j = 0; j < cell->size; j++) {
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
    copy_atoms_found = NULL;

    return num_trans;
}

/* Thoroughly confirms that a given symmetry operation is a symmetry. */
/* This is a convenient wrapper around ovl_check_total_overlap. */
/* -1: Error.  0: Not a symmetry.  1: Is a symmetry. */
static int is_overlap_all_atoms(const double trans[3], const int rot[3][3],
                                const Cell *cell, const double symprec,
                                const int is_identity) {
    OverlapChecker *checker = ovl_overlap_checker_init(cell);
    if (checker == NULL) {
        return -1;
    }

    int result =
        (cell->aperiodic_axis == -1)
            ? ovl_check_total_overlap(checker, trans, rot, symprec, is_identity)
            : ovl_check_layer_total_overlap(checker, trans, rot, symprec,
                                            is_identity);

    ovl_overlap_checker_free(checker);
    checker = NULL;

    return result;
}

static int get_index_with_least_atoms(const Cell *cell) {
    int *mapping = (int *)malloc(sizeof(int) * cell->size);
    if (mapping == NULL) {
        warning_print("spglib: Memory could not be allocated ");
        return -1;
    }

    for (int i = 0; i < cell->size; i++) {
        mapping[i] = 0;
    }

    for (int i = 0; i < cell->size; i++) {
        for (int j = 0; j < cell->size; j++) {
            if (cell->types[i] == cell->types[j]) {
                mapping[j]++;
                break;
            }
        }
    }

    int min_index = 0;
    for (int i = 0, min = mapping[0]; i < cell->size; i++) {
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
static VecDBL *get_layer_translation(const int rot[3][3], const Cell *cell,
                                     const double symprec,
                                     const int is_identity) {
    double origin[3];

    debug_print("get_translation (tolerance = %f):\n", symprec);

    int *is_found = (int *)malloc(sizeof(int) * cell->size);
    if (is_found == NULL) {
        warning_print("spglib: Memory could not be allocated ");
        return NULL;
    }

    for (int i = 0; i < cell->size; i++) {
        is_found[i] = 0;
    }

    /* Look for the atom index with least number of atoms within same type */
    int min_atom_index = get_index_with_least_atoms(cell);
    if (min_atom_index == -1) {
        debug_print("spglib: get_index_with_least_atoms failed.\n");
        free(is_found);
        is_found = NULL;
        return NULL;
    }

    /* Set min_atom_index as the origin to measure the distance between atoms.
     */
    mat_multiply_matrix_vector_id3(origin, rot, cell->position[min_atom_index]);

    int num_trans = search_layer_translation_part(
        is_found, cell, rot, min_atom_index, origin, symprec, is_identity);
    if (num_trans == -1 || num_trans == 0) {
        free(is_found);
        is_found = NULL;
        return NULL;
    }

    VecDBL *trans = mat_alloc_VecDBL(num_trans);
    if (trans == NULL) {
        free(is_found);
        is_found = NULL;
        return NULL;
    }
    for (int i = 0, k = 0; i < cell->size; i++) {
        if (is_found[i]) {
            for (int j = 0; j < 3; j++) {
                trans->vec[k][j] = cell->position[i][j] - origin[j];
                if (j != cell->aperiodic_axis) {
                    trans->vec[k][j] = mat_Dmod1(trans->vec[k][j]);
                }
            }
            k++;
        }
    }

    free(is_found);
    is_found = NULL;

    return trans;
}

/* Returns -1 on failure. */
static int search_layer_translation_part(int atoms_found[], const Cell *cell,
                                         const int rot[3][3],
                                         const int min_atom_index,
                                         const double origin[3],
                                         const double symprec,
                                         const int is_identity) {
    double trans[3];
    OverlapChecker *checker = ovl_overlap_checker_init(cell);
    if (checker == NULL) {
        return -1;
    }

    int num_trans = 0;
    for (int i = 0; i < cell->size; i++) {
        if (atoms_found[i]) {
            continue;
        }

        if (cell->types[i] != cell->types[min_atom_index]) {
            continue;
        }

        for (int j = 0; j < 3; j++) {
            trans[j] = cell->position[i][j] - origin[j];
        }

        int is_overlap = ovl_check_layer_total_overlap(checker, trans, rot,
                                                       symprec, is_identity);
        if (is_overlap == -1) {
            ovl_overlap_checker_free(checker);
            checker = NULL;
            return -1;
        }
        if (is_overlap) {
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
}

static int search_layer_pure_translations(int atoms_found[], const Cell *cell,
                                          const double trans[3],
                                          const int periodic_axes[2],
                                          const double symprec) {
    double vec[3];

    int *copy_atoms_found = (int *)malloc(sizeof(int) * cell->size);
    if (copy_atoms_found == NULL) return 0;
    for (int i = 0; i < cell->size; i++) {
        copy_atoms_found[i] = atoms_found[i];
    }

    int num_trans = 0;
    for (int initial_atom = 0; initial_atom < cell->size; initial_atom++) {
        if (!copy_atoms_found[initial_atom]) {
            continue;
        }

        for (int i = 0, i_atom = initial_atom; i < cell->size; i++) {
            for (int j = 0; j < 3; j++) {
                vec[j] = cell->position[i_atom][j] + trans[j];
            }

            for (int j = 0; j < cell->size; j++) {
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
static Symmetry *get_space_group_operations(const PointSymmetry *lattice_sym,
                                            const Cell *primitive,
                                            const double symprec) {
    debug_print("get_space_group_operations (tolerance = %f):\n", symprec);

    VecDBL **trans = (VecDBL **)malloc(sizeof(VecDBL *) * lattice_sym->size);
    if (trans == NULL) {
        warning_print("spglib: Memory could not be allocated ");
        return NULL;
    }

    for (int i = 0; i < lattice_sym->size; i++) {
        trans[i] = NULL;
    }

    int total_num_sym = 0;
    for (int i = 0; i < lattice_sym->size; i++) {
        // TODO: Note that arrays are not sequential
        trans[i] =
            (primitive->aperiodic_axis == -1)
                ? get_translation(lattice_sym->rot[i], primitive, symprec, 0)
                : get_layer_translation(lattice_sym->rot[i], primitive, symprec,
                                        0);
        if (trans[i] != NULL) {
            debug_print("  match translation %d/%d; tolerance = %f\n", i + 1,
                        lattice_sym->size, symprec);

            total_num_sym += trans[i]->size;
        }
    }

    Symmetry *symmetry = sym_alloc_symmetry(total_num_sym);
    if (symmetry == NULL) {
        for (int i = 0; i < lattice_sym->size; i++) {
            if (trans[i] != NULL) {
                mat_free_VecDBL(trans[i]);
                trans[i] = NULL;
            }
        }
        free(trans);
        trans = NULL;
        return NULL;
    }

    int num_sym = 0;
    for (int i = 0; i < lattice_sym->size; i++) {
        if (trans[i] == NULL) {
            continue;
        }
        for (int j = 0; j < trans[i]->size; j++) {
            mat_copy_vector_d3(symmetry->trans[num_sym + j], trans[i]->vec[j]);
            mat_copy_matrix_i3(symmetry->rot[num_sym + j], lattice_sym->rot[i]);
        }
        num_sym += trans[i]->size;
        mat_free_VecDBL(trans[i]);
        trans[i] = NULL;
    }
    free(trans);
    trans = NULL;

    return symmetry;
}

/* lattice_sym.size = 0 is returned if failed. */
static PointSymmetry get_lattice_symmetry(const Cell *cell,
                                          const double symprec,
                                          const double angle_symprec) {
    int axes[3][3];
    double lattice[3][3], min_lattice[3][3];
    double metric[3][3], metric_orig[3][3];
    PointSymmetry lattice_sym;

    debug_print("get_lattice_symmetry:\n");

    lattice_sym.size = 0;

    int aperiodic_axis = cell->aperiodic_axis;
    if (aperiodic_axis == -1) {
        if (!del_delaunay_reduce(min_lattice, cell->lattice, symprec)) {
            debug_print("get_lattice_symmetry failed.\n");
            return lattice_sym;
        }
    } else {
        if (!del_layer_delaunay_reduce(min_lattice, cell->lattice,
                                       aperiodic_axis, symprec)) {
            debug_print("get_lattice_symmetry failed.\n");
            return lattice_sym;
        }
    }

    mat_get_metric(metric_orig, min_lattice);

    double angle_tol = angle_symprec;
    for (int attempt = 0; attempt < NUM_ATTEMPT; attempt++) {
        int num_sym = 0;
        for (int i = 0; i < 26; i++) {
            for (int j = 0; j < 26; j++) {
                for (int k = 0; k < 26; k++) {
                    /* For layer group, some rotations are not permitted. */
                    if (aperiodic_axis != -1 &&
                        ((aperiodic_axis == 2 && k != 2 && k != 5) ||
                         (aperiodic_axis == 0 && i != 0 && i != 3) ||
                         (aperiodic_axis == 1 && j != 1 && j != 4))) {
                        continue;
                    }
                    set_axes(axes, i, j, k);
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
                            warning_print(
                                "spglib: Too many lattice symmetries was "
                                "found.\n");
                            if (angle_tol > 0) {
                                angle_tol *= ANGLE_REDUCE_RATE;
                                warning_print(
                                    "        Reduce angle tolerance to %f\n",
                                    angle_tol);
                            }
                            warning_print("        (line %d, %s).\n", __LINE__,
                                          __FILE__);
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
    debug_print("get_lattice_symmetry failed.\n");
    return lattice_sym;
}

static int is_identity_metric(const double metric_rotated[3][3],
                              const double metric_orig[3][3],
                              const double symprec,
                              const double angle_symprec) {
    int elem_sets[3][2] = {{0, 1}, {0, 2}, {1, 2}};
    double length_orig[3], length_rot[3];

    for (int i = 0; i < 3; i++) {
        length_orig[i] = sqrt(metric_orig[i][i]);
        length_rot[i] = sqrt(metric_rotated[i][i]);
        if (mat_Dabs(length_orig[i] - length_rot[i]) > symprec) {
            return 0;
        }
    }

    for (int i = 0; i < 3; i++) {
        int j = elem_sets[i][0];
        int k = elem_sets[i][1];
        if (angle_symprec > 0) {
            if (mat_Dabs(get_angle(metric_orig, j, k) -
                         get_angle(metric_rotated, j, k)) > angle_symprec) {
                return 0;
            }
        } else {
            /* dtheta = arccos(cos(theta1) - arccos(cos(theta2))) */
            /*        = arccos(c1) - arccos(c2) */
            /*        = arccos(c1c2 + sqrt((1-c1^2)(1-c2^2))) */
            /* sin(dtheta) = sin(arccos(x)) = sqrt(1 - x^2) */
            double cos1 = metric_orig[j][k] / length_orig[j] / length_orig[k];
            double cos2 = metric_rotated[j][k] / length_rot[j] / length_rot[k];
            double x =
                cos1 * cos2 + sqrt(1 - cos1 * cos1) * sqrt(1 - cos2 * cos2);
            double sin_dtheta2 = 1 - x * x;
            double length_ave2 = ((length_orig[j] + length_rot[j]) *
                                  (length_orig[k] + length_rot[k])) /
                                 4;
            if (sin_dtheta2 > SIN_DTHETA2_CUTOFF) {
                if (sin_dtheta2 * length_ave2 > symprec * symprec) {
                    return 0;
                }
            }
        }
    }

    return 1;
}

static double get_angle(const double metric[3][3], const int i, const int j) {
    double length_i = sqrt(metric[i][i]);
    double length_j = sqrt(metric[j][j]);

    return acos(metric[i][j] / length_i / length_j) / PI * 180;
}

static PointSymmetry transform_pointsymmetry(
    const PointSymmetry *lat_sym_orig, const double new_lattice[3][3],
    const double original_lattice[3][3]) {
    double trans_mat[3][3], inv_mat[3][3], drot[3][3];
    PointSymmetry lat_sym_new;

    lat_sym_new.size = 0;

    mat_inverse_matrix_d3(inv_mat, original_lattice, 0);
    mat_multiply_matrix_d3(trans_mat, inv_mat, new_lattice);

    int size = 0;
    for (int i = 0; i < lat_sym_orig->size; i++) {
        mat_cast_matrix_3i_to_3d(drot, lat_sym_orig->rot[i]);
        mat_get_similar_matrix_d3(drot, drot, trans_mat, 0);

        /* new_lattice may have lower point symmetry than original_lattice.*/
        /* The operations that have non-integer elements are not counted. */
        if (mat_is_int_matrix(
                drot, mat_Dabs(mat_get_determinant_d3(trans_mat)) / 10)) {
            mat_cast_matrix_3d_to_3i(lat_sym_new.rot[size], drot);
            if (abs(mat_get_determinant_i3(lat_sym_new.rot[size])) != 1) {
                warning_print(
                    "spglib: A point symmetry operation is not unimodular.");
                warning_print("(line %d, %s).\n", __LINE__, __FILE__);
                return lat_sym_new;
            }
            size++;
        }
    }

    if (!(lat_sym_orig->size == size)) {
        warning_print(
            "spglib: Some of point symmetry operations were dropped.");
        warning_print("(line %d, %s).\n", __LINE__, __FILE__);
    }

    lat_sym_new.size = size;
    return lat_sym_new;
}

// @brief Set an integer matrix to `axes`. Three integer vectors are specified
//        by indices of array `symmetry.c:relative_axes`.
static void set_axes(int axes[3][3], const int a1, const int a2, const int a3) {
    for (int i = 0; i < 3; i++) {
        axes[i][0] = relative_axes[a1][i];
    }
    for (int i = 0; i < 3; i++) {
        axes[i][1] = relative_axes[a2][i];
    }
    for (int i = 0; i < 3; i++) {
        axes[i][2] = relative_axes[a3][i];
    }
}
