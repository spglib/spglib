// Copyright (C) 2008 Atsushi Togo
// This file is part of spglib.
// SPDX-License-Identifier: BSD-3-Clause

#include "spglib.h"
#include "base.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arithmetic.h"
#include "cell.h"
#include "debug.h"
#include "delaunay.h"
#include "determination.h"
#include "kgrid.h"
#include "kpoint.h"
#include "magnetic_spacegroup.h"
#include "mathfunc.h"
#include "msg_database.h"
#include "niggli.h"
#include "pointgroup.h"
#include "primitive.h"
#include "refinement.h"
#include "spacegroup.h"
#include "spg_database.h"
#include "spin.h"
#include "symmetry.h"
#include "version.h"

// Windows does not support _Thread_local. Use appropriate aliases
// Reference: https://stackoverflow.com/a/18298965
#ifndef thread_local
    #if __STDC_VERSION__ >= 201112 && !defined __STDC_NO_THREADS__
        #define thread_local _Thread_local
    #elif defined _MSC_VER
        #define thread_local __declspec(thread)
    #elif defined __GNUC__
        #define thread_local __thread
    #else
        #error "Cannot define thread_local"
    #endif
#endif

/*-------*/
/* error */
/*-------*/
static thread_local SpglibError spglib_error_code = SPGLIB_SUCCESS;

typedef struct {
    SpglibError error;
    char *message;
} SpglibErrorMessage;

static SpglibErrorMessage spglib_error_message[] = {
    {SPGLIB_SUCCESS, "no error"},
    {SPGERR_SPACEGROUP_SEARCH_FAILED, "spacegroup search failed"},
    {SPGERR_CELL_STANDARDIZATION_FAILED, "cell standardization failed"},
    {SPGERR_SYMMETRY_OPERATION_SEARCH_FAILED,
     "symmetry operation search failed"},
    {SPGERR_ATOMS_TOO_CLOSE, "too close distance between atoms"},
    {SPGERR_POINTGROUP_NOT_FOUND, "pointgroup not found"},
    {SPGERR_NIGGLI_FAILED, "Niggli reduction failed"},
    {SPGERR_DELAUNAY_FAILED, "Delaunay reduction failed"},
    {SPGERR_ARRAY_SIZE_SHORTAGE, "array size shortage"},
    {SPGERR_NONE, ""},
};

/*---------*/
/* general */
/*---------*/
static SpglibDataset *get_dataset(double const lattice[3][3],
                                  double const position[][3], int const types[],
                                  int const num_atom, int const hall_number,
                                  double const symprec,
                                  double const angle_tolerance);
static SpglibDataset *get_layer_dataset(
    double const lattice[3][3], double const position[][3], int const types[],
    int const num_atom, int const aperiodic_axis, int const hall_number,
    double const symprec, double const angle_tolerance);
static SpglibMagneticDataset *get_magnetic_dataset(
    double const lattice[3][3], double const position[][3], int const types[],
    double const *tensors, int const tensor_rank, int const num_atom,
    int const is_axial, double const symprec, double const angle_tolerance,
    double const mag_symprec);
static SpglibDataset *init_dataset(void);
static SpglibMagneticDataset *init_magnetic_dataset(void);
static int set_dataset(SpglibDataset *dataset, Cell const *cell,
                       Primitive const *primitive, Spacegroup const *spacegroup,
                       ExactStructure *exstr);
static int set_magnetic_dataset(SpglibMagneticDataset *dataset,
                                int const num_atoms, Cell const *cell_std,
                                MagneticSymmetry const *magnetic_symmetry,
                                MagneticDataset const *msgdata,
                                int const *equivalent_atoms,
                                double const primitive_lattice[3][3]);
static int get_symmetry_from_dataset(
    int rotation[][3][3], double translation[][3], int const max_size,
    double const lattice[3][3], double const position[][3], int const types[],
    int const num_atom, double const symprec, double const angle_tolerance);
static MagneticSymmetry *get_symmetry_with_site_tensors(
    int equivalent_atoms[], int **permutations, double primitive_lattice[3][3],
    Cell const *cell, int const with_time_reversal, int const is_axial,
    double const symprec, double const angle_tolerance,
    double const mag_symprec);
static int get_multiplicity(double const lattice[3][3],
                            double const position[][3], int const types[],
                            int const num_atom, double const symprec,
                            double const angle_tolerance);
static int standardize_primitive(double lattice[3][3], double position[][3],
                                 int types[], int const num_atom,
                                 double const symprec,
                                 double const angle_tolerance);
static int standardize_cell(double lattice[3][3], double position[][3],
                            int types[], int const num_atom,
                            int const num_array_size, double const symprec,
                            double const angle_tolerance);
static int get_standardized_cell(double lattice[3][3], double position[][3],
                                 int types[], int const num_atom,
                                 int const num_array_size,
                                 int const to_primitive, double const symprec,
                                 double const angle_tolerance);
static Centering get_centering(int hall_number);
static void set_cell(double lattice[3][3], double position[][3], int types[],
                     Cell *cell);
static int get_international(char symbol[11], double const lattice[3][3],
                             double const position[][3], int const types[],
                             int const num_atom, double const symprec,
                             double const angle_tolerance);
static int get_schoenflies(char symbol[7], double const lattice[3][3],
                           double const position[][3], int const types[],
                           int const num_atom, double const symprec,
                           double const angle_tolerance);
static SpglibSpacegroupType get_spacegroup_type(int const hall_number);
static int get_hall_number_from_symmetry(int const rotation[][3][3],
                                         double const translation[][3],
                                         int const num_operations,
                                         double const lattice[3][3],
                                         int const transform_lattice_by_tmat,
                                         double const symprec);

/*---------*/
/* kpoints */
/*---------*/
static int get_ir_reciprocal_mesh(int grid_address[][3], int ir_mapping_table[],
                                  int const mesh[3], int const is_shift[3],
                                  int const is_time_reversal,
                                  double const lattice[3][3],
                                  double const position[][3], int const types[],
                                  int const num_atom, double const symprec,
                                  double const angle_tolerance);
static size_t get_dense_ir_reciprocal_mesh(
    int grid_address[][3], size_t ir_mapping_table[], int const mesh[3],
    int const is_shift[3], int const is_time_reversal,
    double const lattice[3][3], double const position[][3], int const types[],
    size_t const num_atom, double const symprec, double const angle_tolerance);

static int get_stabilized_reciprocal_mesh(
    int grid_address[][3], int ir_mapping_table[], int const mesh[3],
    int const is_shift[3], int const is_time_reversal, int const num_rot,
    int const rotations[][3][3], size_t const num_q, double const qpoints[][3]);
static size_t get_dense_stabilized_reciprocal_mesh(
    int grid_address[][3], size_t ir_mapping_table[], int const mesh[3],
    int const is_shift[3], int const is_time_reversal, size_t const num_rot,
    int const rotations[][3][3], size_t const num_q, double const qpoints[][3]);

/*========*/
/* global */
/*========*/

/*-----------------------------------------*/
/* Version: spglib-[major].[minor].[micro] */
/*-----------------------------------------*/
char const *spg_get_version(void) {
    spglib_error_code = SPGLIB_SUCCESS;
    return SPGLIB_VERSION;
}

char const *spg_get_version_full(void) {
    spglib_error_code = SPGLIB_SUCCESS;
    return SPGLIB_VERSION_FULL;
}

char const *spg_get_commit(void) {
    spglib_error_code = SPGLIB_SUCCESS;
    return SPGLIB_COMMIT;
}

int spg_get_major_version(void) {
    spglib_error_code = SPGLIB_SUCCESS;
    return SPGLIB_MAJOR_VERSION;
}

int spg_get_minor_version(void) {
    spglib_error_code = SPGLIB_SUCCESS;
    return SPGLIB_MINOR_VERSION;
}

int spg_get_micro_version(void) {
    spglib_error_code = SPGLIB_SUCCESS;
    return SPGLIB_MICRO_VERSION;
}

/*-------*/
/* error */
/*-------*/
SpglibError spg_get_error_code(void) { return spglib_error_code; }
SPG_API_TEST void spg_set_error_code(SpglibError err) {
    spglib_error_code = err;
}

char *spg_get_error_message(SpglibError error) {
    int i;

    for (i = 0; i < 100; i++) {
        if (SPGERR_NONE == spglib_error_message[i].error) {
            break;
        }

        if (error == spglib_error_message[i].error) {
            return spglib_error_message[i].message;
        }
    }

    return NULL;
}

/*---------*/
/* general */
/*---------*/
/* Return NULL if failed */
SpglibDataset *spg_get_dataset(double const lattice[3][3],
                               double const position[][3], int const types[],
                               int const num_atom, double const symprec) {
    return get_dataset(lattice, position, types, num_atom, 0, symprec, -1.0);
}

SpglibDataset *spg_get_layer_dataset(double const lattice[3][3],
                                     double const position[][3],
                                     int const types[], int const num_atom,
                                     int const aperiodic_axis,
                                     double const symprec) {
    return get_layer_dataset(lattice, position, types, num_atom, aperiodic_axis,
                             0, symprec, -1.0);
}

SpglibMagneticDataset *spg_get_magnetic_dataset(
    double const lattice[3][3], double const position[][3], int const types[],
    double const *tensors, int const tensor_rank, int const num_atom,
    int const is_axial, double const symprec) {
    return get_magnetic_dataset(lattice, position, types, tensors, tensor_rank,
                                num_atom, is_axial, symprec, -1.0, -1.0);
}

SpglibMagneticDataset *spgms_get_magnetic_dataset(
    double const lattice[3][3], double const position[][3], int const types[],
    double const *tensors, int const tensor_rank, int const num_atom,
    int const is_axial, double const symprec, double const angle_tolerance,
    double const mag_symprec) {
    return get_magnetic_dataset(lattice, position, types, tensors, tensor_rank,
                                num_atom, is_axial, symprec, angle_tolerance,
                                mag_symprec);
}

/* Return NULL if failed */
SpglibDataset *spgat_get_dataset(double const lattice[3][3],
                                 double const position[][3], int const types[],
                                 int const num_atom, double const symprec,
                                 double const angle_tolerance) {
    return get_dataset(lattice, position, types, num_atom, 0, symprec,
                       angle_tolerance);
}

/* Return NULL if failed */
SpglibDataset *spg_get_dataset_with_hall_number(
    double const lattice[3][3], double const position[][3], int const types[],
    int const num_atom, int const hall_number, double const symprec) {
    return get_dataset(lattice, position, types, num_atom, hall_number, symprec,
                       -1.0);
}

/* Return NULL if failed */
SpglibDataset *spgat_get_dataset_with_hall_number(
    double const lattice[3][3], double const position[][3], int const types[],
    int const num_atom, int const hall_number, double const symprec,
    double const angle_tolerance) {
    return get_dataset(lattice, position, types, num_atom, hall_number, symprec,
                       angle_tolerance);
}

void spg_free_dataset(SpglibDataset *dataset) {
    if (dataset->n_operations > 0) {
        free(dataset->rotations);
        dataset->rotations = NULL;
        free(dataset->translations);
        dataset->translations = NULL;
        dataset->n_operations = 0;
    }

    if (dataset->n_atoms > 0) {
        free(dataset->wyckoffs);
        dataset->wyckoffs = NULL;
        free(dataset->equivalent_atoms);
        dataset->equivalent_atoms = NULL;
        free(dataset->crystallographic_orbits);
        dataset->crystallographic_orbits = NULL;
        free(dataset->site_symmetry_symbols);
        dataset->site_symmetry_symbols = NULL;
        free(dataset->mapping_to_primitive);
        dataset->mapping_to_primitive = NULL;
        dataset->n_atoms = 0;
    }

    if (dataset->n_std_atoms > 0) {
        free(dataset->std_positions);
        dataset->std_positions = NULL;
        free(dataset->std_types);
        dataset->std_types = NULL;
        free(dataset->std_mapping_to_primitive);
        dataset->std_mapping_to_primitive = NULL;
        dataset->n_std_atoms = 0;
    }

    dataset->spacegroup_number = 0;
    dataset->hall_number = 0;
    strcpy(dataset->international_symbol, "");
    strcpy(dataset->hall_symbol, "");
    strcpy(dataset->choice, "");

    free(dataset);
}

void spg_free_magnetic_dataset(SpglibMagneticDataset *dataset) {
    /* Magnetic symmetry operations */
    if (dataset->n_operations > 0) {
        free(dataset->rotations);
        dataset->rotations = NULL;
        free(dataset->translations);
        dataset->translations = NULL;
        free(dataset->time_reversals);
        dataset->time_reversals = NULL;
    }

    /* Equivalent atoms */
    if (dataset->n_atoms > 0) {
        free(dataset->equivalent_atoms);
        dataset->equivalent_atoms = NULL;
    }

    /* Standardized crystal structure */
    if (dataset->n_std_atoms > 0) {
        free(dataset->std_positions);
        dataset->std_positions = NULL;
        free(dataset->std_types);
        dataset->std_types = NULL;
        free(dataset->std_tensors);
        dataset->std_tensors = NULL;
        dataset->n_std_atoms = 0;
    }

    free(dataset);
    dataset = NULL;
}

/* Return 0 if failed */
int spg_get_symmetry(int rotation[][3][3], double translation[][3],
                     int const max_size, double const lattice[3][3],
                     double const position[][3], int const types[],
                     int const num_atom, double const symprec) {
    return get_symmetry_from_dataset(rotation, translation, max_size, lattice,
                                     position, types, num_atom, symprec, -1.0);
}

/* Return 0 if failed */
int spgat_get_symmetry(int rotation[][3][3], double translation[][3],
                       int const max_size, double const lattice[3][3],
                       double const position[][3], int const types[],
                       int const num_atom, double const symprec,
                       double const angle_tolerance) {
    return get_symmetry_from_dataset(rotation, translation, max_size, lattice,
                                     position, types, num_atom, symprec,
                                     angle_tolerance);
}

/* Return 0 if failed */
int spg_get_symmetry_with_collinear_spin(
    int rotation[][3][3], double translation[][3], int equivalent_atoms[],
    int const max_size, double const lattice[3][3], double const position[][3],
    int const types[], double const spins[], int const num_atom,
    double const symprec) {
    return spgms_get_symmetry_with_collinear_spin(
        rotation, translation, equivalent_atoms, max_size, lattice, position,
        types, spins, num_atom, symprec, -1.0, -1.0);
}

/* Return 0 if failed */
int spgat_get_symmetry_with_collinear_spin(
    int rotation[][3][3], double translation[][3], int equivalent_atoms[],
    int const max_size, double const lattice[3][3], double const position[][3],
    int const types[], double const spins[], int const num_atom,
    double const symprec, double const angle_tolerance) {
    return spgms_get_symmetry_with_collinear_spin(
        rotation, translation, equivalent_atoms, max_size, lattice, position,
        types, spins, num_atom, symprec, angle_tolerance, -1.0);
}

int spgms_get_symmetry_with_collinear_spin(
    int rotation[][3][3], double translation[][3], int equivalent_atoms[],
    int const max_size, double const lattice[3][3], double const position[][3],
    int const types[], double const spins[], int const num_atom,
    double const symprec, double const angle_tolerance,
    double const mag_symprec) {
    int succeeded;
    double primitive_lattice[3][3];
    int *spin_flips;

    if ((spin_flips = (int *)malloc(sizeof(int) * max_size)) == NULL) {
        warning_memory("spin_flips");
        return 0;
    }

    succeeded = spgms_get_symmetry_with_site_tensors(
        rotation, translation, equivalent_atoms, primitive_lattice, spin_flips,
        max_size, lattice, position, types, spins, 0 /* tensor_rank */,
        num_atom, 1 /* with_time_reversal */, 0 /* is_axial */, symprec,
        angle_tolerance, mag_symprec);

    free(spin_flips);
    spin_flips = NULL;

    return succeeded;
}

/* Return 0 if failed */
/* Need to get rotation and translation before calling this function. */
int spg_get_symmetry_with_site_tensors(
    int rotation[][3][3], double translation[][3], int equivalent_atoms[],
    double primitive_lattice[3][3], int *spin_flips, int const max_size,
    double const lattice[3][3], double const position[][3], int const types[],
    double const *tensors, int const tensor_rank, int const num_atom,
    int const with_time_reversal, int const is_axial, double const symprec) {
    return spgms_get_symmetry_with_site_tensors(
        rotation, translation, equivalent_atoms, primitive_lattice, spin_flips,
        max_size, lattice, position, types, tensors, tensor_rank, num_atom,
        with_time_reversal, is_axial, symprec, -1.0, -1.0);
}

/* Return 0 if failed */
int spgat_get_symmetry_with_site_tensors(
    int rotation[][3][3], double translation[][3], int equivalent_atoms[],
    double primitive_lattice[3][3], int *spin_flips, int const max_size,
    double const lattice[3][3], double const position[][3], int const types[],
    double const *tensors, int const tensor_rank, int const num_atom,
    int const with_time_reversal, int const is_axial, double const symprec,
    double const angle_tolerance) {
    return spgms_get_symmetry_with_site_tensors(
        rotation, translation, equivalent_atoms, primitive_lattice, spin_flips,
        max_size, lattice, position, types, tensors, tensor_rank, num_atom,
        with_time_reversal, is_axial, symprec, angle_tolerance, -1.0);
}

/* Return 0 if failed */
int spgms_get_symmetry_with_site_tensors(
    int rotation[][3][3], double translation[][3], int equivalent_atoms[],
    double primitive_lattice[3][3], int *spin_flips, int const max_size,
    double const lattice[3][3], double const position[][3], int const types[],
    double const *tensors, int const tensor_rank, int const num_atom,
    int const with_time_reversal, int const is_axial, double const symprec,
    double const angle_tolerance, double const mag_symprec) {
    int i, size;
    MagneticSymmetry *magnetic_symmetry;
    int *permutations;
    Cell *cell;

    magnetic_symmetry = NULL;
    permutations = NULL;

    if ((cell = cel_alloc_cell(num_atom, tensor_rank)) == NULL) {
        return 0;
    }
    cel_set_cell_with_tensors(cell, lattice, position, types, tensors);

    if ((magnetic_symmetry = get_symmetry_with_site_tensors(
             equivalent_atoms, &permutations, primitive_lattice, cell,
             with_time_reversal, is_axial, symprec, angle_tolerance,
             mag_symprec)) == NULL) {
        /* spglib_error_code is filled in get_symmetry_with_tensors */
        return 0;
    }

    if (magnetic_symmetry->size > max_size) {
        fprintf(stderr, "spglib: Indicated max size(=%d) is less than number ",
                max_size);
        fprintf(stderr, "spglib: of symmetry operations(=%d).\n",
                magnetic_symmetry->size);
        sym_free_magnetic_symmetry(magnetic_symmetry);
        magnetic_symmetry = NULL;
        spglib_error_code = SPGERR_ARRAY_SIZE_SHORTAGE;
        return 0;
    }

    for (i = 0; i < magnetic_symmetry->size; i++) {
        mat_copy_matrix_i3(rotation[i], magnetic_symmetry->rot[i]);
        mat_copy_vector_d3(translation[i], magnetic_symmetry->trans[i]);
        /* spin_flip=1 for timerev=true, spin_flip=-1 for timerev=false */
        spin_flips[i] = 1 - 2 * magnetic_symmetry->timerev[i];
    }
    size = magnetic_symmetry->size;

    sym_free_magnetic_symmetry(magnetic_symmetry);
    magnetic_symmetry = NULL;
    free(permutations);
    permutations = NULL;
    cel_free_cell(cell);
    cell = NULL;

    spglib_error_code = SPGLIB_SUCCESS;
    return size;
}

/* Deprecated at v2.0 */
int spg_get_hall_number_from_symmetry(int const rotation[][3][3],
                                      double const translation[][3],
                                      int const num_operations,
                                      double const symprec) {
    int hall_number;
    double lattice[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

    hall_number = get_hall_number_from_symmetry(
        rotation, translation, num_operations, lattice, 0, symprec);

    if (hall_number) {
        spglib_error_code = SPGLIB_SUCCESS;
        return hall_number;
    } else {
        spglib_error_code = SPGERR_SPACEGROUP_SEARCH_FAILED;
        return 0;
    }
}

SpglibSpacegroupType spg_get_spacegroup_type_from_symmetry(
    int const rotation[][3][3], double const translation[][3],
    int const num_operations, double const lattice[3][3],
    double const symprec) {
    int hall_number;
    SpglibSpacegroupType spglibtype;

    hall_number = get_hall_number_from_symmetry(
        rotation, translation, num_operations, lattice, 1, symprec);

    if (hall_number) {
        spglibtype = get_spacegroup_type(hall_number);
        spglib_error_code = SPGLIB_SUCCESS;
    } else {
        goto err;
    }

    return spglibtype;

err:
    spglib_error_code = SPGERR_SPACEGROUP_SEARCH_FAILED;
    return get_spacegroup_type(0);
}

SpglibMagneticSpacegroupType spg_get_magnetic_spacegroup_type_from_symmetry(
    int const rotations[][3][3], double const translations[][3],
    int const *time_reversals, int const num_operations,
    double const lattice[3][3], double const symprec) {
    int i;
    MagneticSymmetry *magnetic_symmetry;
    MagneticDataset *msgdata;
    SpglibMagneticSpacegroupType spglibtype, matched;

    /* Initialization */
    spglibtype.uni_number = 0;
    spglibtype.litvin_number = 0;
    strcpy(spglibtype.bns_number, "");
    strcpy(spglibtype.og_number, "");
    spglibtype.number = 0;
    spglibtype.type = 0;

    magnetic_symmetry = NULL;
    msgdata = NULL;

    if ((magnetic_symmetry = sym_alloc_magnetic_symmetry(num_operations)) ==
        NULL) {
        return spglibtype;
    }
    for (i = 0; i < num_operations; i++) {
        mat_copy_matrix_i3(magnetic_symmetry->rot[i], rotations[i]);
        mat_copy_vector_d3(magnetic_symmetry->trans[i], translations[i]);
        magnetic_symmetry->timerev[i] = time_reversals[i];
    }

    if ((msgdata = msg_identify_magnetic_space_group_type(
             lattice, magnetic_symmetry, symprec)) == NULL) {
        sym_free_magnetic_symmetry(magnetic_symmetry);
        magnetic_symmetry = NULL;
        return spglibtype;
    }

    matched = spg_get_magnetic_spacegroup_type(msgdata->uni_number);
    spglibtype.uni_number = matched.uni_number;
    spglibtype.litvin_number = matched.litvin_number;
    strcpy(spglibtype.bns_number, matched.bns_number);
    strcpy(spglibtype.og_number, matched.og_number);
    spglibtype.number = matched.number;
    spglibtype.type = matched.type;

    sym_free_magnetic_symmetry(magnetic_symmetry);
    magnetic_symmetry = NULL;
    free(msgdata);
    msgdata = NULL;

    return spglibtype;
}

/* Return 0 if failed */
int spg_get_multiplicity(double const lattice[3][3], double const position[][3],
                         int const types[], int const num_atom,
                         double const symprec) {
    return get_multiplicity(lattice, position, types, num_atom, symprec, -1.0);
}

/* Return 0 if failed */
int spgat_get_multiplicity(double const lattice[3][3],
                           double const position[][3], int const types[],
                           int const num_atom, double const symprec,
                           double const angle_tolerance) {
    return get_multiplicity(lattice, position, types, num_atom, symprec,
                            angle_tolerance);
}

/* Return 0 if failed */
int spg_get_international(char symbol[11], double const lattice[3][3],
                          double const position[][3], int const types[],
                          int const num_atom, double const symprec) {
    return get_international(symbol, lattice, position, types, num_atom,
                             symprec, -1.0);
}

/* Return 0 if failed */
int spgat_get_international(char symbol[11], double const lattice[3][3],
                            double const position[][3], int const types[],
                            int const num_atom, double const symprec,
                            double const angle_tolerance) {
    return get_international(symbol, lattice, position, types, num_atom,
                             symprec, angle_tolerance);
}

/* Return 0 if failed */
int spg_get_schoenflies(char symbol[7], double const lattice[3][3],
                        double const position[][3], int const types[],
                        int const num_atom, double const symprec) {
    return get_schoenflies(symbol, lattice, position, types, num_atom, symprec,
                           -1.0);
}

/* Return 0 if failed */
int spgat_get_schoenflies(char symbol[7], double const lattice[3][3],
                          double const position[][3], int const types[],
                          int const num_atom, double const symprec,
                          double const angle_tolerance) {
    return get_schoenflies(symbol, lattice, position, types, num_atom, symprec,
                           angle_tolerance);
}

/* Return 0 if failed */
int spg_get_pointgroup(char symbol[6], int transform_mat[3][3],
                       int const rotations[][3][3], int const num_rotations) {
    Pointgroup pointgroup;

    pointgroup = ptg_get_transformation_matrix(
        transform_mat, rotations, num_rotations, -1 /* aperiodic_axis*/);

    if (pointgroup.number == 0) {
        spglib_error_code = SPGERR_POINTGROUP_NOT_FOUND;
        return 0;
    }

    memcpy(symbol, pointgroup.symbol, 6);

    spglib_error_code = SPGLIB_SUCCESS;
    return pointgroup.number;
}

/* Return 0 if failed */
int spg_get_symmetry_from_database(int rotations[192][3][3],
                                   double translations[192][3],
                                   int const hall_number) {
    int i, size;
    Symmetry *symmetry;

    symmetry = NULL;

    if ((symmetry = spgdb_get_spacegroup_operations(hall_number)) == NULL) {
        goto err;
    }

    for (i = 0; i < symmetry->size; i++) {
        mat_copy_matrix_i3(rotations[i], symmetry->rot[i]);
        mat_copy_vector_d3(translations[i], symmetry->trans[i]);
    }
    size = symmetry->size;

    sym_free_symmetry(symmetry);
    symmetry = NULL;

    spglib_error_code = SPGLIB_SUCCESS;
    return size;

err:
    spglib_error_code = SPGERR_SPACEGROUP_SEARCH_FAILED;
    return 0;
}

/* Return 0 if failed */
int spg_get_magnetic_symmetry_from_database(int rotations[384][3][3],
                                            double translations[384][3],
                                            int time_reversals[384],
                                            int const uni_number,
                                            int const hall_number) {
    int i, size;
    MagneticSymmetry *symmetry;

    symmetry = NULL;

    if ((symmetry = msgdb_get_spacegroup_operations(uni_number, hall_number)) ==
        NULL) {
        spglib_error_code = SPGERR_SPACEGROUP_SEARCH_FAILED;
        return 0;
    }

    for (i = 0; i < symmetry->size; i++) {
        mat_copy_matrix_i3(rotations[i], symmetry->rot[i]);
        mat_copy_vector_d3(translations[i], symmetry->trans[i]);
        time_reversals[i] = symmetry->timerev[i];
    }
    size = symmetry->size;

    sym_free_magnetic_symmetry(symmetry);
    symmetry = NULL;

    spglib_error_code = SPGLIB_SUCCESS;
    return size;
}

/* Return spglibtype.number = 0 if failed */
SpglibSpacegroupType spg_get_spacegroup_type(int const hall_number) {
    SpglibSpacegroupType spglibtype;
    spglibtype = get_spacegroup_type(hall_number);
    if (0 < hall_number && hall_number < 531) {
        spglib_error_code = SPGLIB_SUCCESS;
    } else {
        spglib_error_code = SPGERR_SPACEGROUP_SEARCH_FAILED;
    }

    return spglibtype;
}

SpglibMagneticSpacegroupType spg_get_magnetic_spacegroup_type(
    int const uni_number) {
    SpglibMagneticSpacegroupType spglibtype;
    MagneticSpacegroupType msgtype;

    /* Initialization */
    spglibtype.uni_number = 0;
    spglibtype.litvin_number = 0;
    strcpy(spglibtype.bns_number, "");
    strcpy(spglibtype.og_number, "");
    spglibtype.number = 0;
    spglibtype.type = 0;

    if (uni_number > 0 && uni_number <= 1651) {
        msgtype = msgdb_get_magnetic_spacegroup_type(uni_number);
        spglibtype.uni_number = msgtype.uni_number;
        spglibtype.litvin_number = msgtype.litvin_number;
        memcpy(spglibtype.bns_number, msgtype.bns_number, 8);
        memcpy(spglibtype.og_number, msgtype.og_number, 12);
        spglibtype.number = msgtype.number;
        spglibtype.type = msgtype.type;

        spglib_error_code = SPGLIB_SUCCESS;
    } else {
        /* out of range */
        spglib_error_code = SPGERR_SPACEGROUP_SEARCH_FAILED;
    }

    return spglibtype;
}

/* Return 0 if failed */
int spg_standardize_cell(double lattice[3][3], double position[][3],
                         int types[], int const num_atom,
                         int const to_primitive, int const no_idealize,
                         double const symprec) {
    return spgat_standardize_cell(lattice, position, types, num_atom,
                                  to_primitive, no_idealize, symprec, -1.0);
}

/* Return 0 if failed */
int spgat_standardize_cell(double lattice[3][3], double position[][3],
                           int types[], int const num_atom,
                           int const to_primitive, int const no_idealize,
                           double const symprec, double const angle_tolerance) {
    if (to_primitive) {
        if (no_idealize) {
            return get_standardized_cell(lattice, position, types, num_atom, 0,
                                         1, symprec, angle_tolerance);
        } else {
            return standardize_primitive(lattice, position, types, num_atom,
                                         symprec, angle_tolerance);
        }
    } else {
        if (no_idealize) {
            return get_standardized_cell(lattice, position, types, num_atom, 0,
                                         0, symprec, angle_tolerance);
        } else {
            return standardize_cell(lattice, position, types, num_atom, 0,
                                    symprec, angle_tolerance);
        }
    }
}

/* Return 0 if failed */
int spg_find_primitive(double lattice[3][3], double position[][3], int types[],
                       int const num_atom, double const symprec) {
    return standardize_primitive(lattice, position, types, num_atom, symprec,
                                 -1.0);
}

/* Return 0 if failed */
int spgat_find_primitive(double lattice[3][3], double position[][3],
                         int types[], int const num_atom, double const symprec,
                         double const angle_tolerance) {
    return standardize_primitive(lattice, position, types, num_atom, symprec,
                                 angle_tolerance);
}

/* Return 0 if failed */
int spg_refine_cell(double lattice[3][3], double position[][3], int types[],
                    int const num_atom, double const symprec) {
    return standardize_cell(lattice, position, types, num_atom, 0, symprec,
                            -1.0);
}

/* Return 0 if failed */
int spgat_refine_cell(double lattice[3][3], double position[][3], int types[],
                      int const num_atom, double const symprec,
                      double const angle_tolerance) {
    return standardize_cell(lattice, position, types, num_atom, 0, symprec,
                            angle_tolerance);
}

int spg_delaunay_reduce(double lattice[3][3], double const symprec) {
    int i, j, succeeded;
    double red_lattice[3][3];

    succeeded = del_delaunay_reduce(red_lattice, lattice, symprec);

    if (succeeded) {
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                lattice[i][j] = red_lattice[i][j];
            }
        }
        spglib_error_code = SPGLIB_SUCCESS;
    } else {
        spglib_error_code = SPGERR_DELAUNAY_FAILED;
    }

    return succeeded;
}

/*---------*/
/* kpoints */
/*---------*/
int spg_get_grid_point_from_address(int const grid_address[3],
                                    int const mesh[3]) {
    int address_double[3];
    int is_shift[3];

    is_shift[0] = 0;
    is_shift[1] = 0;
    is_shift[2] = 0;
    kgd_get_grid_address_double_mesh(address_double, grid_address, mesh,
                                     is_shift);
    return kgd_get_grid_point_double_mesh(address_double, mesh);
}

size_t spg_get_dense_grid_point_from_address(int const grid_address[3],
                                             int const mesh[3]) {
    int address_double[3];
    int is_shift[3];

    is_shift[0] = 0;
    is_shift[1] = 0;
    is_shift[2] = 0;
    kgd_get_grid_address_double_mesh(address_double, grid_address, mesh,
                                     is_shift);
    return kgd_get_dense_grid_point_double_mesh(address_double, mesh);
}

int spg_get_ir_reciprocal_mesh(int grid_address[][3], int ir_mapping_table[],
                               int const mesh[3], int const is_shift[3],
                               int const is_time_reversal,
                               double const lattice[3][3],
                               double const position[][3], int const types[],
                               int const num_atom, double const symprec) {
    return get_ir_reciprocal_mesh(grid_address, ir_mapping_table, mesh,
                                  is_shift, is_time_reversal, lattice, position,
                                  types, num_atom, symprec, -1.0);
}

size_t spg_get_dense_ir_reciprocal_mesh(
    int grid_address[][3], size_t ir_mapping_table[], int const mesh[3],
    int const is_shift[3], int const is_time_reversal,
    double const lattice[3][3], double const position[][3], int const types[],
    int const num_atom, double const symprec) {
    return get_dense_ir_reciprocal_mesh(
        grid_address, ir_mapping_table, mesh, is_shift, is_time_reversal,
        lattice, position, types, num_atom, symprec, -1.0);
}

int spg_get_stabilized_reciprocal_mesh(
    int grid_address[][3], int ir_mapping_table[], int const mesh[3],
    int const is_shift[3], int const is_time_reversal, int const num_rot,
    int const rotations[][3][3], int const num_q, double const qpoints[][3]) {
    return get_stabilized_reciprocal_mesh(grid_address, ir_mapping_table, mesh,
                                          is_shift, is_time_reversal, num_rot,
                                          rotations, num_q, qpoints);
}

size_t spg_get_dense_stabilized_reciprocal_mesh(
    int grid_address[][3], size_t ir_mapping_table[], int const mesh[3],
    int const is_shift[3], int const is_time_reversal, int const num_rot,
    int const rotations[][3][3], int const num_q, double const qpoints[][3]) {
    return get_dense_stabilized_reciprocal_mesh(
        grid_address, ir_mapping_table, mesh, is_shift, is_time_reversal,
        num_rot, rotations, num_q, qpoints);
}

void spg_get_dense_grid_points_by_rotations(size_t rot_grid_points[],
                                            int const address_orig[3],
                                            int const num_rot,
                                            int const rot_reciprocal[][3][3],
                                            int const mesh[3],
                                            int const is_shift[3]) {
    kpt_get_dense_grid_points_by_rotations(
        rot_grid_points, address_orig, rot_reciprocal, num_rot, mesh, is_shift);
}

void spg_get_dense_BZ_grid_points_by_rotations(
    size_t rot_grid_points[], int const address_orig[3], int const num_rot,
    int const rot_reciprocal[][3][3], int const mesh[3], int const is_shift[3],
    size_t const bz_map[]) {
    kpt_get_dense_BZ_grid_points_by_rotations(rot_grid_points, address_orig,
                                              rot_reciprocal, num_rot, mesh,
                                              is_shift, bz_map);
}

int spg_relocate_BZ_grid_address(int bz_grid_address[][3], int bz_map[],
                                 int const grid_address[][3], int const mesh[3],
                                 double const rec_lattice[3][3],
                                 int const is_shift[3]) {
    return kpt_relocate_BZ_grid_address(bz_grid_address, bz_map, grid_address,
                                        mesh, rec_lattice, is_shift);
}

size_t spg_relocate_dense_BZ_grid_address(
    int bz_grid_address[][3], size_t bz_map[], int const grid_address[][3],
    int const mesh[3], double const rec_lattice[3][3], int const is_shift[3]) {
    return kpt_relocate_dense_BZ_grid_address(
        bz_grid_address, bz_map, grid_address, mesh, rec_lattice, is_shift);
}

/*--------*/
/* Niggli */
/*--------*/
/* Return 0 if failed */
int spg_niggli_reduce(double lattice[3][3], double const symprec) {
    int i, j, succeeded;
    double vals[9];

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            vals[i * 3 + j] = lattice[i][j];
        }
    }

    succeeded = niggli_reduce(vals, symprec, -1 /* aperiodic_axis */);

    if (succeeded) {
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                lattice[i][j] = vals[i * 3 + j];
            }
        }
        spglib_error_code = SPGLIB_SUCCESS;
    } else {
        spglib_error_code = SPGERR_NIGGLI_FAILED;
    }

    return succeeded;
}

/*=======*/
/* local */
/*=======*/

/*---------*/
/* general */
/*---------*/
/* Return NULL if failed */
static SpglibDataset *get_dataset(double const lattice[3][3],
                                  double const position[][3], int const types[],
                                  int const num_atom, int const hall_number,
                                  double const symprec,
                                  double const angle_tolerance) {
    SpglibDataset *dataset;
    Cell *cell;
    DataContainer *container;

    dataset = NULL;
    cell = NULL;
    container = NULL;

    if ((dataset = init_dataset()) == NULL) {
        goto not_found;
    }

    if ((cell = cel_alloc_cell(num_atom, NOSPIN)) == NULL) {
        free(dataset);
        dataset = NULL;
        goto not_found;
    }

    cel_set_cell(cell, lattice, position, types);
    if (cel_any_overlap_with_same_type(cell, symprec)) {
        cel_free_cell(cell);
        cell = NULL;
        free(dataset);
        dataset = NULL;
        goto atoms_too_close;
    }

    if ((container = det_determine_all(cell, hall_number, symprec,
                                       angle_tolerance)) != NULL) {
        if (set_dataset(dataset, cell, container->primitive,
                        container->spacegroup, container->exact_structure)) {
            det_free_container(container);
            container = NULL;
            cel_free_cell(cell);
            cell = NULL;
            goto found;
        }
        det_free_container(container);
        container = NULL;
    }

    cel_free_cell(cell);
    cell = NULL;
    free(dataset);
    dataset = NULL;

not_found:
    spglib_error_code = SPGERR_SPACEGROUP_SEARCH_FAILED;
    return NULL;

atoms_too_close:
    spglib_error_code = SPGERR_ATOMS_TOO_CLOSE;
    return NULL;

found:

    spglib_error_code = SPGLIB_SUCCESS;
    return dataset;
}

/* Return NULL if failed */
static SpglibDataset *get_layer_dataset(
    double const lattice[3][3], double const position[][3], int const types[],
    int const num_atom, int const aperiodic_axis, int const hall_number,
    double const symprec, double const angle_tolerance) {
    SpglibDataset *dataset;
    Cell *cell;
    DataContainer *container;

    int i, lattice_rank, periodic_axes[2];

    dataset = NULL;
    cell = NULL;
    container = NULL;

    if ((dataset = init_dataset()) == NULL) {
        goto not_found;
    }

    if ((cell = cel_alloc_cell(num_atom, NOSPIN)) == NULL) {
        free(dataset);
        dataset = NULL;
        goto not_found;
    }

    cel_set_layer_cell(cell, lattice, position, types, aperiodic_axis);
    if (aperiodic_axis == -1) {
        if (cel_any_overlap_with_same_type(cell, symprec)) {
            cel_free_cell(cell);
            cell = NULL;
            free(dataset);
            dataset = NULL;
            goto atoms_too_close;
        }
    } else {
        lattice_rank = 0;
        for (i = 0; i < 3; i++) {
            if (i != cell->aperiodic_axis) {
                periodic_axes[lattice_rank] = i;
                lattice_rank++;
            }
        }
        if (cel_layer_any_overlap_with_same_type(cell, periodic_axes,
                                                 symprec)) {
            cel_free_cell(cell);
            cell = NULL;
            free(dataset);
            dataset = NULL;
            goto atoms_too_close;
        }
    }

    if ((container = det_determine_all(cell, hall_number, symprec,
                                       angle_tolerance)) != NULL) {
        if (set_dataset(dataset, cell, container->primitive,
                        container->spacegroup, container->exact_structure)) {
            det_free_container(container);
            container = NULL;
            cel_free_cell(cell);
            cell = NULL;
            goto found;
        }
        det_free_container(container);
        container = NULL;
    }

    cel_free_cell(cell);
    cell = NULL;
    free(dataset);
    dataset = NULL;

not_found:
    spglib_error_code = SPGERR_SPACEGROUP_SEARCH_FAILED;
    return NULL;

atoms_too_close:
    spglib_error_code = SPGERR_ATOMS_TOO_CLOSE;
    return NULL;

found:

    spglib_error_code = SPGLIB_SUCCESS;
    return dataset;
}

/* Return NULL if failed */
static SpglibMagneticDataset *get_magnetic_dataset(
    double const lattice[3][3], double const position[][3], int const types[],
    double const *tensors, int const tensor_rank, int const num_atom,
    int const is_axial, double const symprec, double const angle_tolerance,
    double const mag_symprec) {
    Cell *cell, *exact_cell, *exact_cell_std;
    Spacegroup *fsg, *xsg;
    MagneticSymmetry *magnetic_symmetry, *representatives;
    MagneticDataset *msgdata;
    SpglibMagneticDataset *dataset;
    int *equivalent_atoms, *permutations;
    double primitive_lattice[3][3];

    cell = NULL;
    exact_cell = NULL;
    exact_cell_std = NULL;
    fsg = NULL;
    xsg = NULL;
    magnetic_symmetry = NULL;
    representatives = NULL;
    msgdata = NULL;
    dataset = NULL;
    permutations = NULL;
    equivalent_atoms = NULL;

    /* Set cell and check overlapped atoms */
    if ((cell = cel_alloc_cell(num_atom, tensor_rank)) == NULL) {
        spglib_error_code = SPGERR_SPACEGROUP_SEARCH_FAILED;
        goto finalize;
    }
    cel_set_cell_with_tensors(cell, lattice, position, types, tensors);
    if (cel_any_overlap_with_same_type(cell, symprec)) {
        spglib_error_code = SPGERR_ATOMS_TOO_CLOSE;
        goto finalize;
    }

    if ((equivalent_atoms = (int *)malloc(sizeof(int) * num_atom)) == NULL) {
        spglib_error_code = SPGERR_SYMMETRY_OPERATION_SEARCH_FAILED;
        goto finalize;
    }

    if ((dataset = init_magnetic_dataset()) == NULL) {
        spglib_error_code = SPGERR_SPACEGROUP_SEARCH_FAILED;
        goto finalize;
    }

    /* Get magnetic symmetry operations of MSG */
    if ((magnetic_symmetry = get_symmetry_with_site_tensors(
             equivalent_atoms, &permutations, primitive_lattice, cell,
             1, /* with_time_reversal */
             is_axial, symprec, angle_tolerance, mag_symprec)) == NULL) {
        spglib_error_code = SPGERR_SYMMETRY_OPERATION_SEARCH_FAILED;
        goto finalize;
    }
    debug_print("MSG: order=%d\n", magnetic_symmetry->size);

    /* Identify family space group (FSG) and maximal space group (XSG) */
    if ((msgdata = msg_identify_magnetic_space_group_type(
             cell->lattice, magnetic_symmetry, symprec)) == NULL) {
        spglib_error_code = SPGERR_SPACEGROUP_SEARCH_FAILED;
        goto finalize;
    }

    /* Idealize positions and site tensors */
    // TODO: cell->position may be highly distorted. Use idealized positions by
    // only space groups for input of `spn_get_idealized_cell`
    if ((exact_cell = spn_get_idealized_cell(
             permutations, cell, magnetic_symmetry, 1, is_axial)) == NULL) {
        spglib_error_code = SPGERR_SYMMETRY_OPERATION_SEARCH_FAILED;
        goto finalize;
    }
    if ((exact_cell_std = msg_get_transformed_cell(
             exact_cell, msgdata->transformation_matrix, msgdata->origin_shift,
             msgdata->std_rotation_matrix, magnetic_symmetry, symprec,
             angle_tolerance)) == NULL) {
        spglib_error_code = SPGERR_SYMMETRY_OPERATION_SEARCH_FAILED;
        goto finalize;
    }

    if (!set_magnetic_dataset(dataset, cell->size, exact_cell_std,
                              magnetic_symmetry, msgdata, equivalent_atoms,
                              primitive_lattice)) {
        spglib_error_code = SPGERR_NONE;
        goto finalize;
    }

    spglib_error_code = SPGLIB_SUCCESS;

finalize:
    if (cell != NULL) {
        cel_free_cell(cell);
        cell = NULL;
    }
    if (exact_cell != NULL) {
        cel_free_cell(exact_cell);
        exact_cell = NULL;
    }
    if (exact_cell_std != NULL) {
        cel_free_cell(exact_cell_std);
        exact_cell_std = NULL;
    }
    if (equivalent_atoms != NULL) {
        free(equivalent_atoms);
        equivalent_atoms = NULL;
    }
    if (permutations != NULL) {
        free(permutations);
        permutations = NULL;
    }
    if (magnetic_symmetry != NULL) {
        sym_free_magnetic_symmetry(magnetic_symmetry);
        magnetic_symmetry = NULL;
    }
    if (fsg != NULL) {
        free(fsg);
        fsg = NULL;
    }
    if (xsg != NULL) {
        free(xsg);
        xsg = NULL;
    }
    if (representatives != NULL) {
        sym_free_magnetic_symmetry(representatives);
        representatives = NULL;
    }
    if (msgdata != NULL) {
        free(msgdata);
        msgdata = NULL;
    }

    if (spglib_error_code == SPGLIB_SUCCESS) {
        return dataset;
    } else {
        if (dataset != NULL) spg_free_magnetic_dataset(dataset);
        return NULL;
    }
}

static SpglibDataset *init_dataset(void) {
    int i, j;
    SpglibDataset *dataset;

    dataset = NULL;

    if ((dataset = (SpglibDataset *)malloc(sizeof(SpglibDataset))) == NULL) {
        warning_memory("dataset");
        return NULL;
    }

    dataset->spacegroup_number = 0;
    dataset->hall_number = 0;
    strcpy(dataset->international_symbol, "");
    strcpy(dataset->hall_symbol, "");
    strcpy(dataset->choice, "");
    dataset->origin_shift[0] = 0;
    dataset->origin_shift[1] = 0;
    dataset->origin_shift[2] = 0;
    dataset->n_atoms = 0;
    dataset->wyckoffs = NULL;
    dataset->equivalent_atoms = NULL;
    dataset->crystallographic_orbits = NULL;
    dataset->mapping_to_primitive = NULL;
    dataset->n_operations = 0;
    dataset->rotations = NULL;
    dataset->translations = NULL;
    dataset->n_std_atoms = 0;
    dataset->std_positions = NULL;
    dataset->std_types = NULL;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            dataset->std_rotation_matrix[i][j] = 0;
        }
    }
    dataset->std_mapping_to_primitive = NULL;
    /* dataset->pointgroup_number = 0; */
    strcpy(dataset->pointgroup_symbol, "");

    return dataset;
}

/* If failed, return NULL. */
static SpglibMagneticDataset *init_magnetic_dataset(void) {
    int i, j;
    SpglibMagneticDataset *dataset;

    dataset = NULL;

    if ((dataset = (SpglibMagneticDataset *)malloc(
             sizeof(SpglibMagneticDataset))) == NULL) {
        warning_memory("dataset");
        return NULL;
    }

    dataset->uni_number = 0;
    dataset->msg_type = 0;
    dataset->hall_number = 0;
    dataset->tensor_rank = 0;
    dataset->n_operations = 0;
    dataset->rotations = NULL;
    dataset->translations = NULL;
    dataset->time_reversals = NULL;
    dataset->n_atoms = 0;
    dataset->equivalent_atoms = NULL;
    dataset->n_std_atoms = 0;
    dataset->std_types = NULL;
    dataset->std_positions = NULL;
    dataset->std_tensors = NULL;

    for (i = 0; i < 3; i++) {
        dataset->origin_shift[i] = 0;

        for (j = 0; j < 3; j++) {
            dataset->transformation_matrix[i][j] = 0;
            dataset->std_lattice[i][j] = 0;
            dataset->primitive_lattice[i][j] = 0;
            dataset->std_rotation_matrix[i][j] = 0;
        }
    }

    return dataset;
}

/* Return 0 if failed */
static int set_dataset(SpglibDataset *dataset, Cell const *cell,
                       Primitive const *primitive, Spacegroup const *spacegroup,
                       ExactStructure *exstr) {
    int i, j;
    double inv_lat[3][3];
    Pointgroup pointgroup;

    /* Spacegroup type, transformation matrix, origin shift */
    dataset->n_atoms = cell->size;
    dataset->spacegroup_number = spacegroup->number;
    dataset->hall_number = spacegroup->hall_number;
    memcpy(dataset->international_symbol, spacegroup->international_short, 11);
    memcpy(dataset->hall_symbol, spacegroup->hall_symbol, 17);
    memcpy(dataset->choice, spacegroup->choice, 6);
    mat_inverse_matrix_d3(inv_lat, spacegroup->bravais_lattice, 0);
    mat_multiply_matrix_d3(dataset->transformation_matrix, inv_lat,
                           cell->lattice);
    mat_copy_vector_d3(dataset->origin_shift, spacegroup->origin_shift);

    dataset->n_operations = exstr->symmetry->size;

    if ((dataset->rotations = (int (*)[3][3])malloc(
             sizeof(int[3][3]) * dataset->n_operations)) == NULL) {
        warning_memory("dataset->rotations");
        goto err;
    }

    if ((dataset->translations = (double (*)[3])malloc(
             sizeof(double[3]) * dataset->n_operations)) == NULL) {
        warning_memory("dataset->translations");
        goto err;
    }

    for (i = 0; i < exstr->symmetry->size; i++) {
        mat_copy_matrix_i3(dataset->rotations[i], exstr->symmetry->rot[i]);
        mat_copy_vector_d3(dataset->translations[i], exstr->symmetry->trans[i]);
    }

    /* Wyckoff positions */
    if ((dataset->wyckoffs = (int *)malloc(sizeof(int) * dataset->n_atoms)) ==
        NULL) {
        warning_memory("dataset->wyckoffs");
        goto err;
    }

    if ((dataset->site_symmetry_symbols =
             (char (*)[7])malloc(sizeof(char[7]) * dataset->n_atoms)) == NULL) {
        warning_memory("dataset->site_symmetry_symbols");
        goto err;
    }

    if ((dataset->equivalent_atoms =
             (int *)malloc(sizeof(int) * dataset->n_atoms)) == NULL) {
        warning_memory("dataset->equivalent_atoms");
        goto err;
    }

    if ((dataset->crystallographic_orbits =
             (int *)malloc(sizeof(int) * dataset->n_atoms)) == NULL) {
        warning_memory("dataset->crystallographic_orbits");
        goto err;
    }

    for (i = 0; i < dataset->n_atoms; i++) {
        dataset->wyckoffs[i] = exstr->wyckoffs[i];
        for (j = 0; j < 7; j++) {
            dataset->site_symmetry_symbols[i][j] =
                exstr->site_symmetry_symbols[i][j];
        }
        dataset->equivalent_atoms[i] = exstr->equivalent_atoms[i];
        dataset->crystallographic_orbits[i] = exstr->crystallographic_orbits[i];
    }

    if ((dataset->mapping_to_primitive =
             (int *)malloc(sizeof(int) * dataset->n_atoms)) == NULL) {
        warning_memory("dataset->mapping_to_primitive");
        goto err;
    }

    debug_print("refined cell after ref_get_Wyckoff_positions\n");
    debug_print_matrix_d3(exstr->bravais->lattice);
    for (i = 0; i < exstr->bravais->size; i++) {
        debug_print("%d: %f %f %f\n", exstr->bravais->types[i],
                    exstr->bravais->position[i][0],
                    exstr->bravais->position[i][1],
                    exstr->bravais->position[i][2]);
    }

    mat_copy_matrix_d3(dataset->primitive_lattice, primitive->cell->lattice);
    for (i = 0; i < dataset->n_atoms; i++) {
        dataset->mapping_to_primitive[i] = primitive->mapping_table[i];
    }

    dataset->n_std_atoms = exstr->bravais->size;
    mat_copy_matrix_d3(dataset->std_lattice, exstr->bravais->lattice);

    if ((dataset->std_positions = (double (*)[3])malloc(
             sizeof(double[3]) * dataset->n_std_atoms)) == NULL) {
        warning_memory("dataset->std_positions");
        goto err;
    }

    if ((dataset->std_types =
             (int *)malloc(sizeof(int) * dataset->n_std_atoms)) == NULL) {
        warning_memory("dataset->std_types");
        goto err;
    }

    if ((dataset->std_mapping_to_primitive =
             (int *)malloc(sizeof(int) * dataset->n_std_atoms)) == NULL) {
        warning_memory("dataset->std_mapping_to_primitive");
        goto err;
    }

    for (i = 0; i < dataset->n_std_atoms; i++) {
        mat_copy_vector_d3(dataset->std_positions[i],
                           exstr->bravais->position[i]);
        dataset->std_types[i] = exstr->bravais->types[i];
        dataset->std_mapping_to_primitive[i] =
            exstr->std_mapping_to_primitive[i];
    }

    mat_copy_matrix_d3(dataset->std_rotation_matrix, exstr->rotation);

    /* dataset->pointgroup_number = spacegroup->pointgroup_number; */
    pointgroup = ptg_get_pointgroup(spacegroup->pointgroup_number);
    memcpy(dataset->pointgroup_symbol, pointgroup.symbol, 6);

    return 1;

err:
    if (dataset->std_positions != NULL) {
        free(dataset->std_positions);
        dataset->std_positions = NULL;
    }
    if (dataset->std_mapping_to_primitive != NULL) {
        free(dataset->std_mapping_to_primitive);
        dataset->std_mapping_to_primitive = NULL;
    }
    if (dataset->equivalent_atoms != NULL) {
        free(dataset->equivalent_atoms);
        dataset->equivalent_atoms = NULL;
    }
    if (dataset->crystallographic_orbits != NULL) {
        free(dataset->crystallographic_orbits);
        dataset->crystallographic_orbits = NULL;
    }
    if (dataset->mapping_to_primitive != NULL) {
        free(dataset->mapping_to_primitive);
        dataset->mapping_to_primitive = NULL;
    }
    if (dataset->site_symmetry_symbols != NULL) {
        free(dataset->site_symmetry_symbols);
        dataset->site_symmetry_symbols = NULL;
    }
    if (dataset->wyckoffs != NULL) {
        free(dataset->wyckoffs);
        dataset->wyckoffs = NULL;
    }
    if (dataset->translations != NULL) {
        free(dataset->translations);
        dataset->translations = NULL;
    }
    if (dataset->rotations != NULL) {
        free(dataset->rotations);
        dataset->rotations = NULL;
    }

    return 0;
}

static int set_magnetic_dataset(SpglibMagneticDataset *dataset,
                                int const num_atoms, Cell const *cell_std,
                                MagneticSymmetry const *magnetic_symmetry,
                                MagneticDataset const *msgdata,
                                int const *equivalent_atoms,
                                double const primitive_lattice[3][3]) {
    int i, s;

    /* Magnetic space-group type */
    dataset->uni_number = msgdata->uni_number;
    dataset->msg_type = msgdata->msg_type;
    dataset->hall_number = msgdata->hall_number;
    dataset->tensor_rank = cell_std->tensor_rank;

    /* Magnetic symmetry operations */
    dataset->n_operations = magnetic_symmetry->size;
    if ((dataset->rotations = (int (*)[3][3])malloc(
             sizeof(int[3][3]) * dataset->n_operations)) == NULL) {
        warning_memory("dataset->rotations");
        goto err;
    }
    if ((dataset->translations = (double (*)[3])malloc(
             sizeof(double[3]) * dataset->n_operations)) == NULL) {
        warning_memory("dataset->translations");
        goto err;
    }
    if ((dataset->time_reversals =
             (int *)malloc(sizeof(int *) * dataset->n_operations)) == NULL) {
        warning_memory("dataset->time_reversals");
        goto err;
    }
    for (i = 0; i < dataset->n_operations; i++) {
        mat_copy_matrix_i3(dataset->rotations[i], magnetic_symmetry->rot[i]);
        mat_copy_vector_d3(dataset->translations[i],
                           magnetic_symmetry->trans[i]);
        dataset->time_reversals[i] = magnetic_symmetry->timerev[i];
    }

    /* Equivalent atoms */
    dataset->n_atoms = num_atoms;
    if ((dataset->equivalent_atoms =
             (int *)malloc(sizeof(int) * dataset->n_atoms)) == NULL) {
        warning_memory("dataset->equivalent_atoms");
        goto err;
    }
    for (i = 0; i < dataset->n_atoms; i++) {
        dataset->equivalent_atoms[i] = equivalent_atoms[i];
    }

    /* Transformation to standardized setting */
    mat_copy_matrix_d3(dataset->transformation_matrix,
                       msgdata->transformation_matrix);
    mat_copy_vector_d3(dataset->origin_shift, msgdata->origin_shift);

    /* Standardized crystal structure */
    dataset->n_std_atoms = cell_std->size;
    mat_copy_matrix_d3(dataset->std_lattice, cell_std->lattice);

    if ((dataset->std_types =
             (int *)malloc(sizeof(int) * dataset->n_std_atoms)) == NULL)
        goto err;
    if ((dataset->std_positions = (double (*)[3])malloc(
             sizeof(double[3]) * dataset->n_std_atoms)) == NULL)
        goto err;
    if ((dataset->std_tensors = spn_alloc_site_tensors(
             dataset->n_std_atoms, cell_std->tensor_rank)) == NULL)
        goto err;
    for (i = 0; i < dataset->n_std_atoms; i++) {
        dataset->std_types[i] = cell_std->types[i];
        for (s = 0; s < 3; s++) {
            dataset->std_positions[i][s] = cell_std->position[i][s];
        }

        if (cell_std->tensor_rank == COLLINEAR) {
            dataset->std_tensors[i] = cell_std->tensors[i];
        } else if (cell_std->tensor_rank == NONCOLLINEAR) {
            for (s = 0; s < 3; s++) {
                dataset->std_tensors[i * 3 + s] = cell_std->tensors[i * 3 + s];
            }
        }
    }
    mat_copy_matrix_d3(dataset->std_rotation_matrix,
                       msgdata->std_rotation_matrix);

    /* Intermediate datum in symmetry search */
    mat_copy_matrix_d3(dataset->primitive_lattice, primitive_lattice);

    return 1;

err:
    if (dataset->rotations != NULL) {
        free(dataset->rotations);
        dataset->rotations = NULL;
    }
    if (dataset->translations != NULL) {
        free(dataset->translations);
        dataset->translations = NULL;
    }
    if (dataset->time_reversals != NULL) {
        free(dataset->time_reversals);
        dataset->time_reversals = NULL;
    }
    if (dataset->equivalent_atoms != NULL) {
        free(dataset->equivalent_atoms);
        dataset->equivalent_atoms = NULL;
    }
    if (dataset->std_types != NULL) {
        free(dataset->std_types);
        dataset->std_types = NULL;
    }
    if (dataset->std_positions != NULL) {
        free(dataset->std_positions);
        dataset->std_positions = NULL;
    }
    if (dataset->std_tensors != NULL) {
        free(dataset->std_tensors);
        dataset->std_tensors = NULL;
    }

    return 0;
}

/* Return 0 if failed */
static int get_symmetry_from_dataset(
    int rotation[][3][3], double translation[][3], int const max_size,
    double const lattice[3][3], double const position[][3], int const types[],
    int const num_atom, double const symprec, double const angle_tolerance) {
    int i, num_sym;
    SpglibDataset *dataset;

    num_sym = 0;
    dataset = NULL;

    if ((dataset = get_dataset(lattice, position, types, num_atom, 0, symprec,
                               angle_tolerance)) == NULL) {
        return 0;
    }

    if (dataset->n_operations > max_size) {
        fprintf(stderr, "spglib: Indicated max size(=%d) is less than number ",
                max_size);
        fprintf(stderr, "spglib: of symmetry operations(=%d).\n",
                dataset->n_operations);
        goto err;
    }

    num_sym = dataset->n_operations;
    for (i = 0; i < num_sym; i++) {
        mat_copy_matrix_i3(rotation[i], dataset->rotations[i]);
        mat_copy_vector_d3(translation[i], dataset->translations[i]);
    }

    spg_free_dataset(dataset);
    dataset = NULL;
    return num_sym;

err:
    spg_free_dataset(dataset);
    dataset = NULL;
    spglib_error_code = SPGERR_ARRAY_SIZE_SHORTAGE;
    return 0;
}

/* Return NULL if failed */
static MagneticSymmetry *get_symmetry_with_site_tensors(
    int equivalent_atoms[], int **permutations, double primitive_lattice[3][3],
    Cell const *cell, int const with_time_reversal, int const is_axial,
    double const symprec, double const angle_tolerance,
    double const mag_symprec) {
    int i;
    MagneticSymmetry *magnetic_symmetry;
    Symmetry *sym_nonspin;
    SpglibDataset *dataset;
    int *equiv_atoms;

    magnetic_symmetry = NULL;
    sym_nonspin = NULL;
    dataset = NULL;
    equiv_atoms = NULL;

    if ((dataset = get_dataset(cell->lattice, cell->position, cell->types,
                               cell->size, 0, symprec, angle_tolerance)) ==
        NULL) {
        goto err;
    }

    if ((sym_nonspin = sym_alloc_symmetry(dataset->n_operations)) == NULL) {
        spg_free_dataset(dataset);
        dataset = NULL;
        goto err;
    }

    for (i = 0; i < dataset->n_operations; i++) {
        mat_copy_matrix_i3(sym_nonspin->rot[i], dataset->rotations[i]);
        mat_copy_vector_d3(sym_nonspin->trans[i], dataset->translations[i]);
    }
    sym_nonspin->size = dataset->n_operations;
    spg_free_dataset(dataset);
    dataset = NULL;

    if ((magnetic_symmetry = spn_get_operations_with_site_tensors(
             &equiv_atoms, permutations, primitive_lattice, sym_nonspin, cell,
             with_time_reversal, is_axial, symprec, angle_tolerance,
             mag_symprec)) == NULL) {
        sym_free_symmetry(sym_nonspin);
        sym_nonspin = NULL;
        goto err;
    }

    /* Set equivalent_atoms */
    for (i = 0; i < cell->size; i++) {
        equivalent_atoms[i] = equiv_atoms[i];
    }
    free(equiv_atoms);
    equiv_atoms = NULL;

    sym_free_symmetry(sym_nonspin);
    sym_nonspin = NULL;

    if (magnetic_symmetry == NULL) {
        goto err;
    }

    spglib_error_code = SPGLIB_SUCCESS;
    return magnetic_symmetry;

err:
    spglib_error_code = SPGERR_SYMMETRY_OPERATION_SEARCH_FAILED;
    return NULL;
}

/* Return 0 if failed */
static int get_multiplicity(double const lattice[3][3],
                            double const position[][3], int const types[],
                            int const num_atom, double const symprec,
                            double const angle_tolerance) {
    int size;
    SpglibDataset *dataset;

    size = 0;
    dataset = NULL;

    if ((dataset = get_dataset(lattice, position, types, num_atom, 0, symprec,
                               angle_tolerance)) == NULL) {
        return 0;
    }

    size = dataset->n_operations;
    spg_free_dataset(dataset);
    dataset = NULL;

    return size;
}

static int standardize_primitive(double lattice[3][3], double position[][3],
                                 int types[], int const num_atom,
                                 double const symprec,
                                 double const angle_tolerance) {
    int i, num_prim_atom;
    int *mapping_table;
    Centering centering;
    SpglibDataset *dataset;
    Cell *primitive, *bravais;

    double identity[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

    num_prim_atom = 0;
    mapping_table = NULL;
    dataset = NULL;
    primitive = NULL;
    bravais = NULL;

    if ((dataset = get_dataset(lattice, position, types, num_atom, 0, symprec,
                               angle_tolerance)) == NULL) {
        return 0;
    }

    if ((centering = get_centering(dataset->hall_number)) == CENTERING_ERROR) {
        spg_free_dataset(dataset);
        dataset = NULL;
        goto err;
    }

    if ((bravais = cel_alloc_cell(dataset->n_std_atoms, NOSPIN)) == NULL) {
        spg_free_dataset(dataset);
        dataset = NULL;
        goto err;
    }

    cel_set_cell(bravais, dataset->std_lattice, dataset->std_positions,
                 dataset->std_types);

    spg_free_dataset(dataset);
    dataset = NULL;

    if ((mapping_table = (int *)malloc(sizeof(int) * bravais->size)) == NULL) {
        warning_memory("mapping_table");
        cel_free_cell(bravais);
        bravais = NULL;
        goto err;
    }

    primitive = spa_transform_to_primitive(mapping_table, bravais, identity,
                                           centering, symprec);

    for (i = 0; i < primitive->size; i++) {
        /* This is an assertion. */
        if (mapping_table[i] != i) {
            warning_print("spglib: spa_transform_to_primitive failed.\n");
            warning_print(
                "Unexpected atom index mapping to primitive (%d != %d).\n",
                mapping_table[i], i);
            free(mapping_table);
            mapping_table = NULL;
            cel_free_cell(bravais);
            bravais = NULL;
            goto err;
        }
    }

    free(mapping_table);
    mapping_table = NULL;
    cel_free_cell(bravais);
    bravais = NULL;

    if (primitive == NULL) {
        goto err;
    }

    set_cell(lattice, position, types, primitive);
    num_prim_atom = primitive->size;

    cel_free_cell(primitive);
    primitive = NULL;

    return num_prim_atom;

err:
    spglib_error_code = SPGERR_CELL_STANDARDIZATION_FAILED;
    return 0;
}

static int standardize_cell(double lattice[3][3], double position[][3],
                            int types[], int const num_atom,
                            int const num_array_size, double const symprec,
                            double const angle_tolerance) {
    int i, n_std_atoms;
    SpglibDataset *dataset;

    n_std_atoms = 0;
    dataset = NULL;

    if ((dataset = get_dataset(lattice, position, types, num_atom, 0, symprec,
                               angle_tolerance)) == NULL) {
        goto err;
    }

    if (num_array_size > 0) {
        if (num_atom < dataset->n_std_atoms) {
            goto array_size_shortage_err;
        }
    }

    n_std_atoms = dataset->n_std_atoms;
    mat_copy_matrix_d3(lattice, dataset->std_lattice);
    for (i = 0; i < dataset->n_std_atoms; i++) {
        types[i] = dataset->std_types[i];
        mat_copy_vector_d3(position[i], dataset->std_positions[i]);
    }

    spg_free_dataset(dataset);
    dataset = NULL;

    return n_std_atoms;

err:
    spglib_error_code = SPGERR_CELL_STANDARDIZATION_FAILED;
    return 0;

array_size_shortage_err:
    spglib_error_code = SPGERR_ARRAY_SIZE_SHORTAGE;
    return 0;
}

static int get_standardized_cell(double lattice[3][3], double position[][3],
                                 int types[], int const num_atom,
                                 int const num_array_size,
                                 int const to_primitive, double const symprec,
                                 double const angle_tolerance) {
    int i, num_std_atom, num_prim_atom;
    int *mapping_table;
    SpglibDataset *dataset;
    Cell *std_cell, *cell, *primitive;
    Centering centering;

    num_std_atom = 0;
    mapping_table = NULL;
    dataset = NULL;
    std_cell = NULL;
    cell = NULL;
    primitive = NULL;

    if ((dataset = get_dataset(lattice, position, types, num_atom, 0, symprec,
                               angle_tolerance)) == NULL) {
        goto err;
    }

    if ((centering = get_centering(dataset->hall_number)) == CENTERING_ERROR) {
        goto err;
    }

    if ((cell = cel_alloc_cell(num_atom, NOSPIN)) == NULL) {
        spg_free_dataset(dataset);
        dataset = NULL;
        goto err;
    }

    cel_set_cell(cell, lattice, position, types);

    if ((mapping_table = (int *)malloc(sizeof(int) * cell->size)) == NULL) {
        warning_memory("mapping_table");
        cel_free_cell(cell);
        cell = NULL;
        spg_free_dataset(dataset);
        dataset = NULL;
        goto err;
    }

    if ((primitive = spa_transform_to_primitive(mapping_table, cell,
                                                dataset->transformation_matrix,
                                                centering, symprec)) == NULL) {
        warning_print("spglib: spa_transform_to_primitive failed.\n");
    }

    for (i = 0; i < cell->size; i++) {
        /* This is an assertion. */
        if (mapping_table[i] != dataset->mapping_to_primitive[i]) {
            warning_print("spglib: spa_transform_to_primitive failed.\n");
            warning_print(
                "Unexpected atom index mapping to primitive (%d != %d).\n",
                mapping_table[i], dataset->mapping_to_primitive[i]);
            free(mapping_table);
            mapping_table = NULL;
            cel_free_cell(cell);
            cell = NULL;
            spg_free_dataset(dataset);
            dataset = NULL;
            goto err;
        }
    }

    free(mapping_table);
    mapping_table = NULL;
    cel_free_cell(cell);
    cell = NULL;
    spg_free_dataset(dataset);
    dataset = NULL;

    if (primitive == NULL) {
        goto err;
    }

    if (to_primitive || centering == PRIMITIVE) {
        set_cell(lattice, position, types, primitive);
        num_prim_atom = primitive->size;
        cel_free_cell(primitive);
        primitive = NULL;
        return num_prim_atom;
    }

    if ((std_cell = spa_transform_from_primitive(primitive, centering,
                                                 symprec)) == NULL) {
        warning_print("spglib: spa_transform_from_primitive failed.\n");
    }
    cel_free_cell(primitive);
    primitive = NULL;

    if (std_cell == NULL) {
        goto err;
    }

    if (num_array_size > 0) {
        if (num_array_size < std_cell->size) {
            cel_free_cell(std_cell);
            std_cell = NULL;
            goto array_size_shortage_err;
        }
    }

    num_std_atom = std_cell->size;
    set_cell(lattice, position, types, std_cell);
    cel_free_cell(std_cell);
    std_cell = NULL;
    return num_std_atom;

err:
    spglib_error_code = SPGERR_CELL_STANDARDIZATION_FAILED;
    return 0;

array_size_shortage_err:
    spglib_error_code = SPGERR_ARRAY_SIZE_SHORTAGE;
    return 0;
}

static void set_cell(double lattice[3][3], double position[][3], int types[],
                     Cell *cell) {
    int i;

    mat_copy_matrix_d3(lattice, cell->lattice);
    for (i = 0; i < cell->size; i++) {
        types[i] = cell->types[i];
        mat_copy_vector_d3(position[i], cell->position[i]);
    }
}

static Centering get_centering(int hall_number) {
    SpacegroupType spgtype;

    spgtype = spgdb_get_spacegroup_type(hall_number);

    return spgtype.centering;
}

static int get_international(char symbol[11], double const lattice[3][3],
                             double const position[][3], int const types[],
                             int const num_atom, double const symprec,
                             double const angle_tolerance) {
    SpglibDataset *dataset;
    int number;

    dataset = NULL;

    if ((dataset = get_dataset(lattice, position, types, num_atom, 0, symprec,
                               angle_tolerance)) == NULL) {
        goto err;
    }

    if (dataset->spacegroup_number > 0) {
        number = dataset->spacegroup_number;
        memcpy(symbol, dataset->international_symbol, 11);

        spg_free_dataset(dataset);
        dataset = NULL;
    } else {
        spg_free_dataset(dataset);
        dataset = NULL;
        goto err;
    }

    spglib_error_code = SPGLIB_SUCCESS;
    return number;

err:
    spglib_error_code = SPGERR_SPACEGROUP_SEARCH_FAILED;
    return 0;
}

static int get_schoenflies(char symbol[7], double const lattice[3][3],
                           double const position[][3], int const types[],
                           int const num_atom, double const symprec,
                           double const angle_tolerance) {
    SpglibDataset *dataset;
    SpglibSpacegroupType spgtype;
    int number;

    dataset = NULL;

    if ((dataset = get_dataset(lattice, position, types, num_atom, 0, symprec,
                               angle_tolerance)) == NULL) {
        goto err;
    }

    if (dataset->spacegroup_number > 0) {
        number = dataset->spacegroup_number;
        spgtype = spg_get_spacegroup_type(dataset->hall_number);
        memcpy(symbol, spgtype.schoenflies, 7);
        spg_free_dataset(dataset);
        dataset = NULL;
    } else {
        spg_free_dataset(dataset);
        dataset = NULL;
        goto err;
    }

    spglib_error_code = SPGLIB_SUCCESS;
    return number;

err:
    spglib_error_code = SPGERR_SPACEGROUP_SEARCH_FAILED;
    return 0;
}

/*---------*/
/* kpoints */
/*---------*/
static int get_ir_reciprocal_mesh(int grid_address[][3], int ir_mapping_table[],
                                  int const mesh[3], int const is_shift[3],
                                  int const is_time_reversal,
                                  double const lattice[3][3],
                                  double const position[][3], int const types[],
                                  int const num_atom, double const symprec,
                                  double const angle_tolerance) {
    SpglibDataset *dataset;
    int num_ir, i;
    MatINT *rotations, *rot_reciprocal;

    if ((dataset = get_dataset(lattice, position, types, num_atom, 0, symprec,
                               angle_tolerance)) == NULL) {
        return 0;
    }

    if ((rotations = mat_alloc_MatINT(dataset->n_operations)) == NULL) {
        spg_free_dataset(dataset);
        dataset = NULL;
        return 0;
    }

    for (i = 0; i < dataset->n_operations; i++) {
        mat_copy_matrix_i3(rotations->mat[i], dataset->rotations[i]);
    }
    rot_reciprocal =
        kpt_get_point_group_reciprocal(rotations, is_time_reversal);
    num_ir = kpt_get_irreducible_reciprocal_mesh(
        grid_address, ir_mapping_table, mesh, is_shift, rot_reciprocal);
    mat_free_MatINT(rot_reciprocal);
    rot_reciprocal = NULL;
    mat_free_MatINT(rotations);
    rotations = NULL;
    spg_free_dataset(dataset);
    dataset = NULL;
    return num_ir;
}

static size_t get_dense_ir_reciprocal_mesh(
    int grid_address[][3], size_t ir_mapping_table[], int const mesh[3],
    int const is_shift[3], int const is_time_reversal,
    double const lattice[3][3], double const position[][3], int const types[],
    size_t const num_atom, double const symprec, double const angle_tolerance) {
    SpglibDataset *dataset;
    int i;
    size_t num_ir;
    MatINT *rotations, *rot_reciprocal;

    if ((dataset = get_dataset(lattice, position, types, num_atom, 0, symprec,
                               angle_tolerance)) == NULL) {
        return 0;
    }

    if ((rotations = mat_alloc_MatINT(dataset->n_operations)) == NULL) {
        spg_free_dataset(dataset);
        dataset = NULL;
        return 0;
    }

    for (i = 0; i < dataset->n_operations; i++) {
        mat_copy_matrix_i3(rotations->mat[i], dataset->rotations[i]);
    }
    rot_reciprocal =
        kpt_get_point_group_reciprocal(rotations, is_time_reversal);
    num_ir = kpt_get_dense_irreducible_reciprocal_mesh(
        grid_address, ir_mapping_table, mesh, is_shift, rot_reciprocal);
    mat_free_MatINT(rot_reciprocal);
    rot_reciprocal = NULL;
    mat_free_MatINT(rotations);
    rotations = NULL;
    spg_free_dataset(dataset);
    dataset = NULL;
    return num_ir;
}

static int get_stabilized_reciprocal_mesh(
    int grid_address[][3], int map[], int const mesh[3], int const is_shift[3],
    int const is_time_reversal, int const num_rot, int const rotations[][3][3],
    size_t const num_q, double const qpoints[][3]) {
    MatINT *rot_real;
    int i, num_ir;

    rot_real = NULL;

    if ((rot_real = mat_alloc_MatINT(num_rot)) == NULL) {
        return 0;
    }

    for (i = 0; i < num_rot; i++) {
        mat_copy_matrix_i3(rot_real->mat[i], rotations[i]);
    }

    num_ir = kpt_get_stabilized_reciprocal_mesh(grid_address, map, mesh,
                                                is_shift, is_time_reversal,
                                                rot_real, num_q, qpoints);

    mat_free_MatINT(rot_real);
    rot_real = NULL;

    return num_ir;
}

static size_t get_dense_stabilized_reciprocal_mesh(
    int grid_address[][3], size_t ir_mapping_table[], int const mesh[3],
    int const is_shift[3], int const is_time_reversal, size_t const num_rot,
    int const rotations[][3][3], size_t const num_q,
    double const qpoints[][3]) {
    MatINT *rot_real;
    size_t i, num_ir;

    rot_real = NULL;

    if ((rot_real = mat_alloc_MatINT(num_rot)) == NULL) {
        return 0;
    }

    for (i = 0; i < num_rot; i++) {
        mat_copy_matrix_i3(rot_real->mat[i], rotations[i]);
    }

    num_ir = kpt_get_dense_stabilized_reciprocal_mesh(
        grid_address, ir_mapping_table, mesh, is_shift, is_time_reversal,
        rot_real, num_q, qpoints);

    mat_free_MatINT(rot_real);
    rot_real = NULL;

    return num_ir;
}

SpglibSpacegroupType get_spacegroup_type(int const hall_number) {
    SpglibSpacegroupType spglibtype;
    SpacegroupType spgtype;
    Pointgroup pointgroup;
    int arth_number;
    char arth_symbol[7];

    spglibtype.number = 0;
    spglibtype.hall_number = 0;
    strcpy(spglibtype.schoenflies, "");
    strcpy(spglibtype.hall_symbol, "");
    strcpy(spglibtype.choice, "");
    strcpy(spglibtype.international, "");
    strcpy(spglibtype.international_full, "");
    strcpy(spglibtype.international_short, "");
    strcpy(spglibtype.pointgroup_international, "");
    strcpy(spglibtype.pointgroup_schoenflies, "");
    spglibtype.arithmetic_crystal_class_number = 0;
    strcpy(spglibtype.arithmetic_crystal_class_symbol, "");

    if (0 < hall_number && hall_number < 531) {
        spgtype = spgdb_get_spacegroup_type(hall_number);
        spglibtype.number = spgtype.number;
        spglibtype.hall_number = hall_number;
        memcpy(spglibtype.schoenflies, spgtype.schoenflies, 7);
        memcpy(spglibtype.hall_symbol, spgtype.hall_symbol, 17);
        memcpy(spglibtype.choice, spgtype.choice, 6);
        memcpy(spglibtype.international, spgtype.international, 32);
        memcpy(spglibtype.international_full, spgtype.international_full, 20);
        memcpy(spglibtype.international_short, spgtype.international_short, 11);
        pointgroup = ptg_get_pointgroup(spgtype.pointgroup_number);
        memcpy(spglibtype.pointgroup_international, pointgroup.symbol, 6);
        memcpy(spglibtype.pointgroup_schoenflies, pointgroup.schoenflies, 4);
        arth_number = arth_get_symbol(arth_symbol, spgtype.number);
        spglibtype.arithmetic_crystal_class_number = arth_number;
        memcpy(spglibtype.arithmetic_crystal_class_symbol, arth_symbol, 7);
    }

    return spglibtype;
}

static int get_hall_number_from_symmetry(int const rotation[][3][3],
                                         double const translation[][3],
                                         int const num_operations,
                                         double const lattice[3][3],
                                         int const transform_lattice_by_tmat,
                                         double const symprec) {
    int i, hall_number;
    Symmetry *symmetry;
    Symmetry *prim_symmetry;
    Spacegroup *spacegroup;
    double t_mat[3][3], t_mat_inv[3][3], prim_lat[3][3];

    symmetry = NULL;
    prim_symmetry = NULL;
    spacegroup = NULL;

    if ((symmetry = sym_alloc_symmetry(num_operations)) == NULL) {
        goto err;
    }

    for (i = 0; i < num_operations; i++) {
        mat_copy_matrix_i3(symmetry->rot[i], rotation[i]);
        mat_copy_vector_d3(symmetry->trans[i], translation[i]);
    }

    prim_symmetry = prm_get_primitive_symmetry(t_mat, symmetry, symprec);
    sym_free_symmetry(symmetry);
    symmetry = NULL;

    if (prim_symmetry == NULL) {
        goto err;
    }

    if (transform_lattice_by_tmat) {
        if (!mat_inverse_matrix_d3(t_mat_inv, t_mat, symprec)) {
            goto err;
        }
        mat_multiply_matrix_d3(prim_lat, lattice, t_mat_inv);
    } else {
        mat_copy_matrix_d3(prim_lat, lattice);
    }

    spacegroup =
        spa_search_spacegroup_with_symmetry(prim_symmetry, prim_lat, symprec);
    sym_free_symmetry(prim_symmetry);
    prim_symmetry = NULL;
    if (spacegroup) {
        hall_number = spacegroup->hall_number;
        free(spacegroup);
        spacegroup = NULL;
    } else {
        goto err;
    }

    return hall_number;

err:
    return 0;
}
