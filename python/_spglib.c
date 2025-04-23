// Copyright (C) 2015 Atsushi Togo
// This file is part of spglib.
// SPDX-License-Identifier: BSD-3-Clause

#include <Python.h>
#include <assert.h>
#define NPY_NO_DEPRECATED_API NPY_1_20_API_VERSION
#include <numpy/ndarrayobject.h>
#include <spglib.h>

static PyObject *py_get_version_tuple(PyObject *self, PyObject *args);
static PyObject *py_get_version_string(PyObject *self, PyObject *args);
static PyObject *py_get_version_full(PyObject *self, PyObject *args);
static PyObject *py_get_commit(PyObject *self, PyObject *args);
static PyObject *py_get_dataset(PyObject *self, PyObject *args);
static PyObject *py_get_layerdataset(PyObject *self, PyObject *args);
static PyObject *py_get_magnetic_dataset(PyObject *self, PyObject *args);
static PyObject *py_get_spacegroup_type(PyObject *self, PyObject *args);
static PyObject *py_get_spacegroup_type_from_symmetry(PyObject *self,
                                                      PyObject *args);
static PyObject *py_get_magnetic_spacegroup_type(PyObject *self,
                                                 PyObject *args);
static PyObject *py_get_magnetic_spacegroup_type_from_symmetry(PyObject *self,
                                                               PyObject *args);
static PyObject *py_get_pointgroup(PyObject *self, PyObject *args);
static PyObject *py_standardize_cell(PyObject *self, PyObject *args);
static PyObject *py_refine_cell(PyObject *self, PyObject *args);
static PyObject *py_get_symmetry(PyObject *self, PyObject *args);
static PyObject *py_get_symmetry_with_collinear_spin(PyObject *self,
                                                     PyObject *args);
static PyObject *py_get_symmetry_with_site_tensors(PyObject *self,
                                                   PyObject *args);
static PyObject *py_find_primitive(PyObject *self, PyObject *args);
static PyObject *py_get_grid_point_from_address(PyObject *self, PyObject *args);
static PyObject *py_get_ir_reciprocal_mesh(PyObject *self, PyObject *args);
static PyObject *py_get_stabilized_reciprocal_mesh(PyObject *self,
                                                   PyObject *args);
static PyObject *py_get_grid_points_by_rotations(PyObject *self,
                                                 PyObject *args);
static PyObject *py_get_BZ_grid_points_by_rotations(PyObject *self,
                                                    PyObject *args);
static PyObject *py_relocate_BZ_grid_address(PyObject *self, PyObject *args);
static PyObject *py_get_symmetry_from_database(PyObject *self, PyObject *args);
static PyObject *py_get_magnetic_symmetry_from_database(PyObject *self,
                                                        PyObject *args);
static PyObject *py_delaunay_reduce(PyObject *self, PyObject *args);
static PyObject *py_niggli_reduce(PyObject *self, PyObject *args);
static PyObject *py_get_hall_number_from_symmetry(PyObject *self,
                                                  PyObject *args);
static PyObject *py_get_error_message(PyObject *self, PyObject *args);

struct module_state {
    PyObject *error;
};

static PyObject *error_out(PyObject *m) {
    struct module_state *st = (struct module_state *)PyModule_GetState(m);
    PyErr_SetString(st->error, "something bad happened");
    return NULL;
}

static PyMethodDef _spglib_methods[] = {
    {"error_out", (PyCFunction)error_out, METH_NOARGS, NULL},
    {"version_tuple", py_get_version_tuple, METH_NOARGS, "Spglib version"},
    {"version_string", py_get_version_string, METH_NOARGS, "Spglib version"},
    {"version_full", py_get_version_full, METH_NOARGS, "Spglib version"},
    {"commit", py_get_commit, METH_NOARGS, "Spglib version"},
    {"dataset", py_get_dataset, METH_VARARGS, "Dataset for crystal symmetry"},
    {"layerdataset", py_get_layerdataset, METH_VARARGS,
     "Dataset for layer symmetry"},
    {"magnetic_dataset", py_get_magnetic_dataset, METH_VARARGS,
     "Magnetic dataset for crystal symmetry"},
    {"spacegroup_type", py_get_spacegroup_type, METH_VARARGS,
     "Space-group type symbols"},
    {"spacegroup_type_from_symmetry", py_get_spacegroup_type_from_symmetry,
     METH_VARARGS, "Space-group type symbols from symmetry operations"},
    {"magnetic_spacegroup_type", py_get_magnetic_spacegroup_type, METH_VARARGS,
     "Magnetic space-group type symbols"},
    {"magnetic_spacegroup_type_from_symmetry",
     py_get_magnetic_spacegroup_type_from_symmetry, METH_VARARGS,
     "Magnetic space-group type symbols from symmetry operations"},
    {"symmetry_from_database", py_get_symmetry_from_database, METH_VARARGS,
     "Get symmetry operations from database"},
    {"magnetic_symmetry_from_database", py_get_magnetic_symmetry_from_database,
     METH_VARARGS, "Get magnetic symmetry operations from database"},
    {"pointgroup", py_get_pointgroup, METH_VARARGS,
     "International symbol of pointgroup"},
    {"standardize_cell", py_standardize_cell, METH_VARARGS, "Standardize cell"},
    {"refine_cell", py_refine_cell, METH_VARARGS, "Refine cell"},
    {"symmetry", py_get_symmetry, METH_VARARGS, "Symmetry operations"},
    {"symmetry_with_collinear_spin", py_get_symmetry_with_collinear_spin,
     METH_VARARGS, "Symmetry operations with collinear spin magnetic moments"},
    {"symmetry_with_site_tensors", py_get_symmetry_with_site_tensors,
     METH_VARARGS, "Symmetry operations with site vectors"},
    {"primitive", py_find_primitive, METH_VARARGS,
     "Find primitive cell in the input cell"},
    {"grid_point_from_address", py_get_grid_point_from_address, METH_VARARGS,
     "Translate grid address to grid point index"},
    {"ir_reciprocal_mesh", py_get_ir_reciprocal_mesh, METH_VARARGS,
     "Reciprocal mesh points with map"},
    {"stabilized_reciprocal_mesh", py_get_stabilized_reciprocal_mesh,
     METH_VARARGS, "Reciprocal mesh points with map"},
    {"grid_points_by_rotations", py_get_grid_points_by_rotations, METH_VARARGS,
     "Rotated grid points are returned"},
    {"BZ_grid_points_by_rotations", py_get_BZ_grid_points_by_rotations,
     METH_VARARGS, "Rotated grid points in BZ are returned"},
    {"BZ_grid_address", py_relocate_BZ_grid_address, METH_VARARGS,
     "Relocate grid addresses inside Brillouin zone"},
    {"delaunay_reduce", py_delaunay_reduce, METH_VARARGS, "Delaunay reduction"},
    {"niggli_reduce", py_niggli_reduce, METH_VARARGS, "Niggli reduction"},
    {"hall_number_from_symmetry", py_get_hall_number_from_symmetry,
     METH_VARARGS, "Space group type is searched from symmetry operations."},
    {"error_message", py_get_error_message, METH_VARARGS, "Error message"},

    {NULL, NULL, 0, NULL}};

static int _spglib_traverse(PyObject *m, visitproc visit, void *arg) {
    Py_VISIT(((struct module_state *)PyModule_GetState(m))->error);
    return 0;
}

static int _spglib_clear(PyObject *m) {
    Py_CLEAR(((struct module_state *)PyModule_GetState(m))->error);
    return 0;
}

static struct PyModuleDef moduledef = {PyModuleDef_HEAD_INIT,
                                       "_spglib",
                                       NULL,
                                       sizeof(struct module_state),
                                       _spglib_methods,
                                       NULL,
                                       _spglib_traverse,
                                       _spglib_clear,
                                       NULL};

// Define macro to make sure symbol is exported
// When compiling define API for export
#if defined(_MSC_VER)
    // On windows the API must be explicitly marked for export/import
    #define EXPORT __declspec(dllexport)
#else
    // On Unix this is not necessary
    #define EXPORT __attribute__((visibility("default")))
#endif

EXPORT PyObject *PyInit__spglib(void) {
    // Basic numpy import check. ABI compatibility is performed there
    // TODO: Switch to PyArray_ImportNumPyAPI when numpy 1.x support is dropped
    import_array();

    PyObject *module = PyModule_Create(&moduledef);

    if (module == NULL) return NULL;

    struct module_state *st = (struct module_state *)PyModule_GetState(module);

    st->error = PyErr_NewException("_spglib.Error", NULL, NULL);
    if (st->error == NULL) {
        Py_DECREF(module);
        return NULL;
    }
    return module;
}

static PyObject *py_get_version_tuple(PyObject *self, PyObject *args) {
    PyObject *tuple = PyTuple_New(3);
    PyTuple_SetItem(tuple, 0, PyLong_FromLong((long)spg_get_major_version()));
    PyTuple_SetItem(tuple, 1, PyLong_FromLong((long)spg_get_minor_version()));
    PyTuple_SetItem(tuple, 2, PyLong_FromLong((long)spg_get_micro_version()));

    return tuple;
}

static PyObject *py_get_version_string(PyObject *self, PyObject *args) {
    return PyUnicode_FromString(spg_get_version());
}

static PyObject *py_get_version_full(PyObject *self, PyObject *args) {
    return PyUnicode_FromString(spg_get_version_full());
}

static PyObject *py_get_commit(PyObject *self, PyObject *args) {
    return PyUnicode_FromString(spg_get_commit());
}

PyObject *build_python_list_from_dataset(SpglibDataset *dataset) {
    int len_list = 21;
    PyObject *array, *vec, *mat, *rot, *trans, *wyckoffs, *equiv_atoms;
    PyObject *crystallographic_orbits;
    PyObject *site_symmetry_symbols, *primitive_lattice, *mapping_to_primitive;
    PyObject *std_lattice, *std_types, *std_positions,
        *std_mapping_to_primitive;

    PyObject *std_rotation;

    int i, j, k, n;
    array = PyList_New(len_list);
    n = 0;

    /* Space group number, international symbol, hall symbol */
    PyList_SetItem(array, n, PyLong_FromLong((long)dataset->spacegroup_number));
    n++;
    PyList_SetItem(array, n, PyLong_FromLong((long)dataset->hall_number));
    n++;
    PyList_SetItem(array, n,
                   PyUnicode_FromString(dataset->international_symbol));
    n++;
    PyList_SetItem(array, n, PyUnicode_FromString(dataset->hall_symbol));
    n++;
    PyList_SetItem(array, n, PyUnicode_FromString(dataset->choice));
    n++;

    /* Transformation matrix */
    mat = PyList_New(3);
    for (i = 0; i < 3; i++) {
        vec = PyList_New(3);
        for (j = 0; j < 3; j++) {
            PyList_SetItem(
                vec, j,
                PyFloat_FromDouble(dataset->transformation_matrix[i][j]));
        }
        PyList_SetItem(mat, i, vec);
    }
    PyList_SetItem(array, n, mat);
    n++;

    /* Origin shift */
    vec = PyList_New(3);
    for (i = 0; i < 3; i++) {
        PyList_SetItem(vec, i, PyFloat_FromDouble(dataset->origin_shift[i]));
    }
    PyList_SetItem(array, n, vec);
    n++;

    /* Rotation matrices */
    rot = PyList_New(dataset->n_operations);
    for (i = 0; i < dataset->n_operations; i++) {
        mat = PyList_New(3);
        for (j = 0; j < 3; j++) {
            vec = PyList_New(3);
            for (k = 0; k < 3; k++) {
                PyList_SetItem(
                    vec, k, PyLong_FromLong((long)dataset->rotations[i][j][k]));
            }
            PyList_SetItem(mat, j, vec);
        }
        PyList_SetItem(rot, i, mat);
    }
    PyList_SetItem(array, n, rot);
    n++;

    /* Translation vectors */
    trans = PyList_New(dataset->n_operations);
    for (i = 0; i < dataset->n_operations; i++) {
        vec = PyList_New(3);
        for (j = 0; j < 3; j++) {
            PyList_SetItem(vec, j,
                           PyFloat_FromDouble(dataset->translations[i][j]));
        }
        PyList_SetItem(trans, i, vec);
    }
    PyList_SetItem(array, n, trans);
    n++;

    /* Wyckoff letters, Equivalent atoms */
    wyckoffs = PyList_New(dataset->n_atoms);
    site_symmetry_symbols = PyList_New(dataset->n_atoms);
    crystallographic_orbits = PyList_New(dataset->n_atoms);
    equiv_atoms = PyList_New(dataset->n_atoms);
    mapping_to_primitive = PyList_New(dataset->n_atoms);
    for (i = 0; i < dataset->n_atoms; i++) {
        PyList_SetItem(wyckoffs, i,
                       PyLong_FromLong((long)dataset->wyckoffs[i]));
        PyList_SetItem(site_symmetry_symbols, i,
                       PyUnicode_FromString(dataset->site_symmetry_symbols[i]));
        PyList_SetItem(equiv_atoms, i,
                       PyLong_FromLong((long)dataset->equivalent_atoms[i]));
        PyList_SetItem(
            crystallographic_orbits, i,
            PyLong_FromLong((long)dataset->crystallographic_orbits[i]));
        PyList_SetItem(mapping_to_primitive, i,
                       PyLong_FromLong((long)dataset->mapping_to_primitive[i]));
    }
    PyList_SetItem(array, n, wyckoffs);
    n++;
    PyList_SetItem(array, n, site_symmetry_symbols);
    n++;
    PyList_SetItem(array, n, crystallographic_orbits);
    n++;
    PyList_SetItem(array, n, equiv_atoms);
    n++;

    primitive_lattice = PyList_New(3);
    for (i = 0; i < 3; i++) {
        vec = PyList_New(3);
        for (j = 0; j < 3; j++) {
            PyList_SetItem(
                vec, j, PyFloat_FromDouble(dataset->primitive_lattice[i][j]));
        }
        PyList_SetItem(primitive_lattice, i, vec);
    }
    PyList_SetItem(array, n, primitive_lattice);
    n++;

    PyList_SetItem(array, n, mapping_to_primitive);
    n++;

    std_lattice = PyList_New(3);
    for (i = 0; i < 3; i++) {
        vec = PyList_New(3);
        for (j = 0; j < 3; j++) {
            PyList_SetItem(vec, j,
                           PyFloat_FromDouble(dataset->std_lattice[i][j]));
        }
        PyList_SetItem(std_lattice, i, vec);
    }
    PyList_SetItem(array, n, std_lattice);
    n++;

    /* Standardized unit cell */
    std_types = PyList_New(dataset->n_std_atoms);
    std_positions = PyList_New(dataset->n_std_atoms);
    std_mapping_to_primitive = PyList_New(dataset->n_std_atoms);
    for (i = 0; i < dataset->n_std_atoms; i++) {
        vec = PyList_New(3);
        for (j = 0; j < 3; j++) {
            PyList_SetItem(vec, j,
                           PyFloat_FromDouble(dataset->std_positions[i][j]));
        }
        PyList_SetItem(std_types, i,
                       PyLong_FromLong((long)dataset->std_types[i]));
        PyList_SetItem(std_positions, i, vec);
        PyList_SetItem(
            std_mapping_to_primitive, i,
            PyLong_FromLong((long)dataset->std_mapping_to_primitive[i]));
    }
    PyList_SetItem(array, n, std_types);
    n++;
    PyList_SetItem(array, n, std_positions);
    n++;

    std_rotation = PyList_New(3);
    for (i = 0; i < 3; i++) {
        vec = PyList_New(3);
        for (j = 0; j < 3; j++) {
            PyList_SetItem(
                vec, j, PyFloat_FromDouble(dataset->std_rotation_matrix[i][j]));
        }
        PyList_SetItem(std_rotation, i, vec);
    }
    PyList_SetItem(array, n, std_rotation);
    n++;

    PyList_SetItem(array, n, std_mapping_to_primitive);
    n++;

    /* Point group */
    /* PyList_SetItem(array, n, PyLong_FromLong((long)
     * dataset->pointgroup_number)); */
    /* n++; */
    PyList_SetItem(array, n, PyUnicode_FromString(dataset->pointgroup_symbol));
    n++;

    assert(n == len_list);
    return array;
}

static PyObject *py_get_dataset(PyObject *self, PyObject *args) {
    int hall_number;
    double symprec, angle_tolerance;
    PyArrayObject *py_lattice;
    PyArrayObject *py_positions;
    PyArrayObject *py_atom_types;
    PyObject *array;
    double (*lat)[3];
    double (*pos)[3];
    int num_atom;
    int *typat;
    SpglibDataset *dataset;

    if (!PyArg_ParseTuple(args, "OOOidd", &py_lattice, &py_positions,
                          &py_atom_types, &hall_number, &symprec,
                          &angle_tolerance)) {
        return NULL;
    }

    lat = (double (*)[3])PyArray_DATA(py_lattice);
    pos = (double (*)[3])PyArray_DATA(py_positions);
    num_atom = PyArray_DIMS(py_positions)[0];
    typat = (int *)PyArray_DATA(py_atom_types);

    if ((dataset = spgat_get_dataset_with_hall_number(
             lat, pos, typat, num_atom, hall_number, symprec,
             angle_tolerance)) == NULL) {
        Py_RETURN_NONE;
    }

    array = build_python_list_from_dataset(dataset);
    spg_free_dataset(dataset);

    return array;
}

static PyObject *py_get_layerdataset(PyObject *self, PyObject *args) {
    int aperiodic_dir;
    double symprec;
    PyArrayObject *py_lattice;
    PyArrayObject *py_positions;
    PyArrayObject *py_atom_types;

    PyObject *array;

    double (*lat)[3];
    double (*pos)[3];
    int num_atom, len_list;
    int *typat;
    SpglibDataset *dataset;

    if (!PyArg_ParseTuple(args, "OOOid", &py_lattice, &py_positions,
                          &py_atom_types, &aperiodic_dir, &symprec)) {
        return NULL;
    }

    if (!PyArray_IS_C_CONTIGUOUS(py_lattice)) {
        PyErr_SetString(PyExc_RuntimeError, "Lattice vector not C_CONTIGUOUS");
        return NULL;
    }
    if (!PyArray_IS_C_CONTIGUOUS(py_positions)) {
        PyErr_SetString(PyExc_RuntimeError,
                        "Positions vector not C_CONTIGUOUS");
        return NULL;
    }
    if (!PyArray_IS_C_CONTIGUOUS(py_atom_types)) {
        PyErr_SetString(PyExc_RuntimeError,
                        "Atom types vector not C_CONTIGUOUS");
        return NULL;
    }

    lat = (double (*)[3])PyArray_DATA(py_lattice);
    pos = (double (*)[3])PyArray_DATA(py_positions);
    num_atom = PyArray_DIMS(py_positions)[0];
    typat = (int *)PyArray_DATA(py_atom_types);

    dataset = spg_get_layer_dataset(lat, pos, typat, num_atom, aperiodic_dir,
                                    symprec);

    if (!dataset) {
        Py_RETURN_NONE;
    }

    // NOTE: This code is still experimental. It parses the information
    // from layer dataset exactly as it would from normal dataset.
    // It should be checked that all of this information makes sense.
    array = build_python_list_from_dataset(dataset);
    spg_free_dataset(dataset);

    return array;
}

static PyObject *py_get_magnetic_dataset(PyObject *self, PyObject *args) {
    int tensor_rank, is_axial;
    double symprec, angle_tolerance, mag_symprec;
    PyArrayObject *py_lattice, *py_positions, *py_atom_types, *py_magmoms;

    int num_atom;
    double (*lat)[3];
    double (*pos)[3];
    int *typeat;
    double *tensors;
    SpglibMagneticDataset *dataset;

    PyObject *array, *rot, *trans, *timerev, *equiv_atoms, *mat, *vec;
    PyObject *std_lattice, *std_types, *std_positions, *std_tensors;
    PyObject *std_rotation;
    PyObject *primitive_lattice;
    int len_list, n, i, j, k, n_tensors;

    if (!PyArg_ParseTuple(args, "OOOOiiddd", &py_lattice, &py_positions,
                          &py_atom_types, &py_magmoms, &tensor_rank, &is_axial,
                          &symprec, &angle_tolerance, &mag_symprec)) {
        return NULL;
    }

    lat = (double (*)[3])PyArray_DATA(py_lattice);
    pos = (double (*)[3])PyArray_DATA(py_positions);
    num_atom = PyArray_DIMS(py_positions)[0];
    typeat = (int *)PyArray_DATA(py_atom_types);
    tensors = (double *)PyArray_DATA(py_magmoms);

    if ((dataset = spgms_get_magnetic_dataset(
             lat, pos, typeat, tensors, tensor_rank, num_atom, is_axial,
             symprec, angle_tolerance, mag_symprec)) == NULL) {
        Py_RETURN_NONE;
    }

    len_list = 19;
    array = PyList_New(len_list);
    n = 0;

    /* Magnetic space-group type */
    PyList_SetItem(array, n++, PyLong_FromLong((long)dataset->uni_number));
    PyList_SetItem(array, n++, PyLong_FromLong((long)dataset->msg_type));
    PyList_SetItem(array, n++, PyLong_FromLong((long)dataset->hall_number));
    PyList_SetItem(array, n++, PyLong_FromLong((long)dataset->tensor_rank));

    /* Magnetic symmetry operations */
    PyList_SetItem(array, n++, PyLong_FromLong((long)dataset->n_operations));

    /* Rotation */
    rot = PyList_New(dataset->n_operations);
    for (i = 0; i < dataset->n_operations; i++) {
        mat = PyList_New(3);
        for (j = 0; j < 3; j++) {
            vec = PyList_New(3);
            for (k = 0; k < 3; k++) {
                PyList_SetItem(
                    vec, k, PyLong_FromLong((long)dataset->rotations[i][j][k]));
            }
            PyList_SetItem(mat, j, vec);
        }
        PyList_SetItem(rot, i, mat);
    }
    PyList_SetItem(array, n++, rot);

    /* Translation vectors */
    trans = PyList_New(dataset->n_operations);
    for (i = 0; i < dataset->n_operations; i++) {
        vec = PyList_New(3);
        for (j = 0; j < 3; j++) {
            PyList_SetItem(vec, j,
                           PyFloat_FromDouble(dataset->translations[i][j]));
        }
        PyList_SetItem(trans, i, vec);
    }
    PyList_SetItem(array, n++, trans);

    /* Time reversals */
    timerev = PyList_New(dataset->n_operations);
    for (i = 0; i < dataset->n_operations; i++) {
        PyList_SetItem(timerev, i,
                       PyLong_FromLong((long)dataset->time_reversals[i]));
    }
    PyList_SetItem(array, n++, timerev);

    /* Equivalent atoms */
    PyList_SetItem(array, n++, PyLong_FromLong((long)dataset->n_atoms));
    equiv_atoms = PyList_New(dataset->n_atoms);
    for (i = 0; i < dataset->n_atoms; i++) {
        PyList_SetItem(equiv_atoms, i,
                       PyLong_FromLong((long)dataset->equivalent_atoms[i]));
    }
    PyList_SetItem(array, n++, equiv_atoms);

    /* Transformation matrix to standardized setting */
    mat = PyList_New(3);
    for (i = 0; i < 3; i++) {
        vec = PyList_New(3);
        for (j = 0; j < 3; j++) {
            PyList_SetItem(
                vec, j,
                PyFloat_FromDouble(dataset->transformation_matrix[i][j]));
        }
        PyList_SetItem(mat, i, vec);
    }
    PyList_SetItem(array, n++, mat);

    /* Origin shift */
    vec = PyList_New(3);
    for (i = 0; i < 3; i++) {
        PyList_SetItem(vec, i, PyFloat_FromDouble(dataset->origin_shift[i]));
    }
    PyList_SetItem(array, n++, vec);

    /* Standardized crystal structure */
    PyList_SetItem(array, n++, PyLong_FromLong((long)dataset->n_std_atoms));

    std_lattice = PyList_New(3);
    for (i = 0; i < 3; i++) {
        vec = PyList_New(3);
        for (j = 0; j < 3; j++) {
            PyList_SetItem(vec, j,
                           PyFloat_FromDouble(dataset->std_lattice[i][j]));
        }
        PyList_SetItem(std_lattice, i, vec);
    }
    PyList_SetItem(array, n++, std_lattice);

    std_types = PyList_New(dataset->n_std_atoms);
    std_positions = PyList_New(dataset->n_std_atoms);
    for (i = 0; i < dataset->n_std_atoms; i++) {
        vec = PyList_New(3);
        for (j = 0; j < 3; j++) {
            PyList_SetItem(vec, j,
                           PyFloat_FromDouble(dataset->std_positions[i][j]));
        }
        PyList_SetItem(std_types, i,
                       PyLong_FromLong((long)dataset->std_types[i]));
        PyList_SetItem(std_positions, i, vec);
    }
    PyList_SetItem(array, n++, std_types);
    PyList_SetItem(array, n++, std_positions);

    if (tensor_rank == 0) {
        n_tensors = dataset->n_std_atoms;
    } else if (tensor_rank == 1) {
        n_tensors = 3 * dataset->n_std_atoms;
    } else {
        Py_RETURN_NONE;
    }
    std_tensors = PyList_New(n_tensors);
    for (i = 0; i < n_tensors; i++) {
        PyList_SetItem(std_tensors, i,
                       PyFloat_FromDouble(dataset->std_tensors[i]));
    }
    PyList_SetItem(array, n++, std_tensors);

    std_rotation = PyList_New(3);
    for (i = 0; i < 3; i++) {
        vec = PyList_New(3);
        for (j = 0; j < 3; j++) {
            PyList_SetItem(
                vec, j, PyFloat_FromDouble(dataset->std_rotation_matrix[i][j]));
        }
        PyList_SetItem(std_rotation, i, vec);
    }
    PyList_SetItem(array, n++, std_rotation);

    /* Intermediate datum in symmetry search */
    primitive_lattice = PyList_New(3);
    for (i = 0; i < 3; i++) {
        vec = PyList_New(3);
        for (j = 0; j < 3; j++) {
            PyList_SetItem(
                vec, j, PyFloat_FromDouble(dataset->primitive_lattice[i][j]));
        }
        PyList_SetItem(primitive_lattice, i, vec);
    }
    PyList_SetItem(array, n++, primitive_lattice);

    assert(n == len_list);

    spg_free_magnetic_dataset(dataset);

    return array;
}

static PyObject *py_get_symmetry_from_database(PyObject *self, PyObject *args) {
    int hall_number;
    PyArrayObject *py_rotations;
    PyArrayObject *py_translations;

    int (*rot)[3][3];
    double (*trans)[3];
    int num_sym;

    if (!PyArg_ParseTuple(args, "OOi", &py_rotations, &py_translations,
                          &hall_number)) {
        return NULL;
    }

    if (PyArray_DIMS(py_rotations)[0] < 192 ||
        PyArray_DIMS(py_translations)[0] < 192) {
        Py_RETURN_NONE;
    }

    rot = (int (*)[3][3])PyArray_DATA(py_rotations);
    trans = (double (*)[3])PyArray_DATA(py_translations);

    num_sym = spg_get_symmetry_from_database(rot, trans, hall_number);

    return PyLong_FromLong((long)num_sym);
}

static PyObject *py_get_magnetic_symmetry_from_database(PyObject *self,
                                                        PyObject *args) {
    PyArrayObject *py_rotations;
    PyArrayObject *py_translations;
    PyArrayObject *py_time_reversals;
    int uni_number;
    int hall_number;

    int (*rot)[3][3];
    double (*trans)[3];
    int *timerev;
    int num_sym;

    if (!PyArg_ParseTuple(args, "OOOii", &py_rotations, &py_translations,
                          &py_time_reversals, &uni_number, &hall_number)) {
        return NULL;
    }

    rot = (int (*)[3][3])PyArray_DATA(py_rotations);
    trans = (double (*)[3])PyArray_DATA(py_translations);
    timerev = (int *)PyArray_DATA(py_time_reversals);

    num_sym = spg_get_magnetic_symmetry_from_database(rot, trans, timerev,
                                                      uni_number, hall_number);

    return PyLong_FromLong((long)num_sym);
}

static PyObject *py_get_spacegroup_type(PyObject *self, PyObject *args) {
    int n, hall_number;
    PyObject *array;
    SpglibSpacegroupType spg_type;

    if (!PyArg_ParseTuple(args, "i", &hall_number)) {
        return NULL;
    }

    spg_type = spg_get_spacegroup_type(hall_number);
    if (spg_type.number == 0) {
        Py_RETURN_NONE;
    }

    array = PyList_New(12);
    n = 0;
    PyList_SetItem(array, n, PyLong_FromLong((long)spg_type.number));
    n++;
    PyList_SetItem(array, n,
                   PyUnicode_FromString(spg_type.international_short));
    n++;
    PyList_SetItem(array, n, PyUnicode_FromString(spg_type.international_full));
    n++;
    PyList_SetItem(array, n, PyUnicode_FromString(spg_type.international));
    n++;
    PyList_SetItem(array, n, PyUnicode_FromString(spg_type.schoenflies));
    n++;
    PyList_SetItem(array, n, PyLong_FromLong((long)spg_type.hall_number));
    n++;
    PyList_SetItem(array, n, PyUnicode_FromString(spg_type.hall_symbol));
    n++;
    PyList_SetItem(array, n, PyUnicode_FromString(spg_type.choice));
    n++;
    PyList_SetItem(array, n,
                   PyUnicode_FromString(spg_type.pointgroup_international));
    n++;
    PyList_SetItem(array, n,
                   PyUnicode_FromString(spg_type.pointgroup_schoenflies));
    n++;
    PyList_SetItem(
        array, n,
        PyLong_FromLong((long)spg_type.arithmetic_crystal_class_number));
    n++;
    PyList_SetItem(
        array, n,
        PyUnicode_FromString(spg_type.arithmetic_crystal_class_symbol));
    n++;

    return array;
}

static PyObject *py_get_spacegroup_type_from_symmetry(PyObject *self,
                                                      PyObject *args) {
    PyArrayObject *py_rotations;
    PyArrayObject *py_translations;
    PyArrayObject *py_lattice;
    double symprec;

    int (*rot)[3][3];
    double (*trans)[3];
    double (*lat)[3];
    int num_sym, n;
    PyObject *array;
    SpglibSpacegroupType spg_type;

    if (!PyArg_ParseTuple(args, "OOOd", &py_rotations, &py_translations,
                          &py_lattice, &symprec)) {
        return NULL;
    }

    rot = (int (*)[3][3])PyArray_DATA(py_rotations);
    trans = (double (*)[3])PyArray_DATA(py_translations);
    lat = (double (*)[3])PyArray_DATA(py_lattice);
    num_sym = PyArray_DIMS(py_rotations)[0];

    spg_type = spg_get_spacegroup_type_from_symmetry(rot, trans, num_sym, lat,
                                                     symprec);
    if (spg_type.number == 0) {
        Py_RETURN_NONE;
    }

    array = PyList_New(12);
    n = 0;
    PyList_SetItem(array, n, PyLong_FromLong((long)spg_type.number));
    n++;
    PyList_SetItem(array, n,
                   PyUnicode_FromString(spg_type.international_short));
    n++;
    PyList_SetItem(array, n, PyUnicode_FromString(spg_type.international_full));
    n++;
    PyList_SetItem(array, n, PyUnicode_FromString(spg_type.international));
    n++;
    PyList_SetItem(array, n, PyUnicode_FromString(spg_type.schoenflies));
    n++;
    PyList_SetItem(array, n, PyLong_FromLong((long)spg_type.hall_number));
    n++;
    PyList_SetItem(array, n, PyUnicode_FromString(spg_type.hall_symbol));
    n++;
    PyList_SetItem(array, n, PyUnicode_FromString(spg_type.choice));
    n++;
    PyList_SetItem(array, n,
                   PyUnicode_FromString(spg_type.pointgroup_international));
    n++;
    PyList_SetItem(array, n,
                   PyUnicode_FromString(spg_type.pointgroup_schoenflies));
    n++;
    PyList_SetItem(
        array, n,
        PyLong_FromLong((long)spg_type.arithmetic_crystal_class_number));
    n++;
    PyList_SetItem(
        array, n,
        PyUnicode_FromString(spg_type.arithmetic_crystal_class_symbol));
    n++;

    return array;
}

static PyObject *py_get_magnetic_spacegroup_type(PyObject *self,
                                                 PyObject *args) {
    int n, uni_number;
    PyObject *array;
    SpglibMagneticSpacegroupType msg_type;

    if (!PyArg_ParseTuple(args, "i", &uni_number)) {
        return NULL;
    }

    msg_type = spg_get_magnetic_spacegroup_type(uni_number);
    if (msg_type.number == 0) {
        Py_RETURN_NONE;
    }

    array = PyList_New(6);
    n = 0;
    PyList_SetItem(array, n++, PyLong_FromLong((long)msg_type.uni_number));
    PyList_SetItem(array, n++, PyLong_FromLong((long)msg_type.litvin_number));
    PyList_SetItem(array, n++, PyUnicode_FromString(msg_type.bns_number));
    PyList_SetItem(array, n++, PyUnicode_FromString(msg_type.og_number));
    PyList_SetItem(array, n++, PyLong_FromLong((long)msg_type.number));
    PyList_SetItem(array, n++, PyLong_FromLong((long)msg_type.type));

    return array;
}

static PyObject *py_get_magnetic_spacegroup_type_from_symmetry(PyObject *self,
                                                               PyObject *args) {
    PyArrayObject *py_rotations;
    PyArrayObject *py_translations;
    PyArrayObject *py_time_reversals;
    PyArrayObject *py_lattice;
    double symprec;

    int (*rotations)[3][3];
    double (*translations)[3];
    int *time_reversals;
    double (*lattice)[3];
    int num_operations;

    SpglibMagneticSpacegroupType msg_type;

    int n;
    PyObject *array;

    if (!PyArg_ParseTuple(args, "OOOOd", &py_rotations, &py_translations,
                          &py_time_reversals, &py_lattice, &symprec)) {
        return NULL;
    }
    rotations = (int (*)[3][3])PyArray_DATA(py_rotations);
    translations = (double (*)[3])PyArray_DATA(py_translations);
    time_reversals = (int *)PyArray_DATA(py_time_reversals);
    lattice = (double (*)[3])PyArray_DATA(py_lattice);
    num_operations = PyArray_DIMS(py_rotations)[0];

    msg_type = spg_get_magnetic_spacegroup_type_from_symmetry(
        rotations, translations, time_reversals, num_operations, lattice,
        symprec);
    if (msg_type.number == 0) {
        Py_RETURN_NONE;
    }

    array = PyList_New(6);
    n = 0;
    PyList_SetItem(array, n++, PyLong_FromLong((long)msg_type.uni_number));
    PyList_SetItem(array, n++, PyLong_FromLong((long)msg_type.litvin_number));
    PyList_SetItem(array, n++, PyUnicode_FromString(msg_type.bns_number));
    PyList_SetItem(array, n++, PyUnicode_FromString(msg_type.og_number));
    PyList_SetItem(array, n++, PyLong_FromLong((long)msg_type.number));
    PyList_SetItem(array, n++, PyLong_FromLong((long)msg_type.type));

    return array;
}

static PyObject *py_get_pointgroup(PyObject *self, PyObject *args) {
    PyArrayObject *py_rotations;

    int i, j;
    int trans_mat[3][3];
    char symbol[6];
    PyObject *array, *mat, *vec;
    int (*rot)[3][3];
    int num_rot;
    int ptg_num;

    if (!PyArg_ParseTuple(args, "O", &py_rotations)) {
        return NULL;
    }

    rot = (int (*)[3][3])PyArray_DATA(py_rotations);
    num_rot = PyArray_DIMS(py_rotations)[0];
    ptg_num = spg_get_pointgroup(symbol, trans_mat, rot, num_rot);

    /* Transformation matrix */
    mat = PyList_New(3);
    for (i = 0; i < 3; i++) {
        vec = PyList_New(3);
        for (j = 0; j < 3; j++) {
            PyList_SetItem(vec, j, PyLong_FromLong((long)trans_mat[i][j]));
        }
        PyList_SetItem(mat, i, vec);
    }

    array = PyList_New(3);
    PyList_SetItem(array, 0, PyUnicode_FromString(symbol));
    PyList_SetItem(array, 1, PyLong_FromLong((long)ptg_num));
    PyList_SetItem(array, 2, mat);

    return array;
}

static PyObject *py_standardize_cell(PyObject *self, PyObject *args) {
    int num_atom, to_primitive, no_idealize;
    double symprec, angle_tolerance;
    PyArrayObject *py_lattice;
    PyArrayObject *py_positions;
    PyArrayObject *py_atom_types;

    double (*lat)[3];
    double (*pos)[3];
    int *typat;
    int num_atom_std;

    if (!PyArg_ParseTuple(args, "OOOiiidd", &py_lattice, &py_positions,
                          &py_atom_types, &num_atom, &to_primitive,
                          &no_idealize, &symprec, &angle_tolerance)) {
        return NULL;
    }

    lat = (double (*)[3])PyArray_DATA(py_lattice);
    pos = (double (*)[3])PyArray_DATA(py_positions);
    typat = (int *)PyArray_DATA(py_atom_types);

    num_atom_std =
        spgat_standardize_cell(lat, pos, typat, num_atom, to_primitive,
                               no_idealize, symprec, angle_tolerance);

    return PyLong_FromLong((long)num_atom_std);
}

static PyObject *py_refine_cell(PyObject *self, PyObject *args) {
    int num_atom;
    double symprec, angle_tolerance;
    PyArrayObject *py_lattice;
    PyArrayObject *py_positions;
    PyArrayObject *py_atom_types;

    double (*lat)[3];
    double (*pos)[3];
    int *typat;
    int num_atom_std;

    if (!PyArg_ParseTuple(args, "OOOidd", &py_lattice, &py_positions,
                          &py_atom_types, &num_atom, &symprec,
                          &angle_tolerance)) {
        return NULL;
    }

    lat = (double (*)[3])PyArray_DATA(py_lattice);
    pos = (double (*)[3])PyArray_DATA(py_positions);
    typat = (int *)PyArray_DATA(py_atom_types);

    num_atom_std =
        spgat_refine_cell(lat, pos, typat, num_atom, symprec, angle_tolerance);

    return PyLong_FromLong((long)num_atom_std);
}

static PyObject *py_find_primitive(PyObject *self, PyObject *args) {
    double symprec, angle_tolerance;
    PyArrayObject *py_lattice;
    PyArrayObject *py_positions;
    PyArrayObject *py_atom_types;

    double (*lat)[3];
    double (*pos)[3];
    int num_atom;
    int *types;
    int num_atom_prim;

    if (!PyArg_ParseTuple(args, "OOOdd", &py_lattice, &py_positions,
                          &py_atom_types, &symprec, &angle_tolerance)) {
        return NULL;
    }

    lat = (double (*)[3])PyArray_DATA(py_lattice);
    pos = (double (*)[3])PyArray_DATA(py_positions);
    num_atom = PyArray_DIMS(py_positions)[0];
    types = (int *)PyArray_DATA(py_atom_types);

    num_atom_prim = spgat_find_primitive(lat, pos, types, num_atom, symprec,
                                         angle_tolerance);

    return PyLong_FromLong((long)num_atom_prim);
}

static PyObject *py_get_symmetry(PyObject *self, PyObject *args) {
    double symprec, angle_tolerance;
    PyArrayObject *py_lattice;
    PyArrayObject *py_positions;
    PyArrayObject *py_rotations;
    PyArrayObject *py_translations;
    PyArrayObject *py_atom_types;

    double (*lat)[3];
    double (*pos)[3];
    int *types;
    int num_atom;
    int (*rot)[3][3];
    double (*trans)[3];
    int num_sym_from_array_size;
    int num_sym;

    if (!PyArg_ParseTuple(args, "OOOOOdd", &py_rotations, &py_translations,
                          &py_lattice, &py_positions, &py_atom_types, &symprec,
                          &angle_tolerance)) {
        return NULL;
    }

    lat = (double (*)[3])PyArray_DATA(py_lattice);
    pos = (double (*)[3])PyArray_DATA(py_positions);
    types = (int *)PyArray_DATA(py_atom_types);
    num_atom = PyArray_DIMS(py_positions)[0];
    rot = (int (*)[3][3])PyArray_DATA(py_rotations);
    trans = (double (*)[3])PyArray_DATA(py_translations);
    num_sym_from_array_size = PyArray_DIMS(py_rotations)[0];

    /* num_sym has to be larger than num_sym_from_array_size. */
    num_sym = spgat_get_symmetry(rot, trans, num_sym_from_array_size, lat, pos,
                                 types, num_atom, symprec, angle_tolerance);
    return PyLong_FromLong((long)num_sym);
}

static PyObject *py_get_symmetry_with_collinear_spin(PyObject *self,
                                                     PyObject *args) {
    double symprec, angle_tolerance;
    PyArrayObject *py_lattice;
    PyArrayObject *py_positions;
    PyArrayObject *py_rotations;
    PyArrayObject *py_translations;
    PyArrayObject *py_atom_types;
    PyArrayObject *py_magmoms;
    PyArrayObject *py_equiv_atoms;

    double (*lat)[3];
    double (*pos)[3];
    double *spins;
    int *types;
    int num_atom;
    int (*rot)[3][3];
    double (*trans)[3];
    int *equiv_atoms;
    int num_sym_from_array_size;
    int num_sym;

    if (!PyArg_ParseTuple(args, "OOOOOOOdd", &py_rotations, &py_translations,
                          &py_equiv_atoms, &py_lattice, &py_positions,
                          &py_atom_types, &py_magmoms, &symprec,
                          &angle_tolerance)) {
        return NULL;
    }

    lat = (double (*)[3])PyArray_DATA(py_lattice);
    pos = (double (*)[3])PyArray_DATA(py_positions);
    spins = (double *)PyArray_DATA(py_magmoms);
    types = (int *)PyArray_DATA(py_atom_types);
    num_atom = PyArray_DIMS(py_positions)[0];
    rot = (int (*)[3][3])PyArray_DATA(py_rotations);
    trans = (double (*)[3])PyArray_DATA(py_translations);
    equiv_atoms = (int *)PyArray_DATA(py_equiv_atoms);
    num_sym_from_array_size = PyArray_DIMS(py_rotations)[0];

    /* num_sym has to be larger than num_sym_from_array_size. */
    num_sym = spgat_get_symmetry_with_collinear_spin(
        rot, trans, equiv_atoms, num_sym_from_array_size, lat, pos, types,
        spins, num_atom, symprec, angle_tolerance);
    return PyLong_FromLong((long)num_sym);
}

static PyObject *py_get_symmetry_with_site_tensors(PyObject *self,
                                                   PyObject *args) {
    double symprec, angle_tolerance, mag_symprec;
    PyArrayObject *py_lattice;
    PyArrayObject *py_positions;
    PyArrayObject *py_rotations;
    PyArrayObject *py_translations;
    PyArrayObject *py_atom_types;
    PyArrayObject *py_tensors;
    PyArrayObject *py_equiv_atoms;
    PyArrayObject *py_primitive_lattice;
    PyArrayObject *py_spin_flips;

    int with_time_reversal, is_axial;

    double (*lat)[3];
    double (*pos)[3];
    double *tensors;
    int *types;
    int num_atom;
    int (*rot)[3][3];
    double (*trans)[3];
    int *equiv_atoms;
    double (*primitive_lattice)[3];
    int *spin_flips;
    int num_sym_from_array_size;
    int num_sym;
    int tensor_rank;

    if (!PyArg_ParseTuple(
            args, "OOOOOOOOOiiddd", &py_rotations, &py_translations,
            &py_equiv_atoms, &py_primitive_lattice, &py_spin_flips, &py_lattice,
            &py_positions, &py_atom_types, &py_tensors, &with_time_reversal,
            &is_axial, &symprec, &angle_tolerance, &mag_symprec)) {
        return NULL;
    }

    lat = (double (*)[3])PyArray_DATA(py_lattice);
    pos = (double (*)[3])PyArray_DATA(py_positions);
    tensors = (double *)PyArray_DATA(py_tensors);
    types = (int *)PyArray_DATA(py_atom_types);
    num_atom = PyArray_DIMS(py_positions)[0];
    rot = (int (*)[3][3])PyArray_DATA(py_rotations);
    trans = (double (*)[3])PyArray_DATA(py_translations);
    equiv_atoms = (int *)PyArray_DATA(py_equiv_atoms);
    primitive_lattice = (double (*)[3])PyArray_DATA(py_primitive_lattice);
    num_sym_from_array_size = PyArray_DIMS(py_rotations)[0];
    tensor_rank = PyArray_NDIM(py_tensors) - 1;
    if (tensor_rank == 0 || tensor_rank == 1) {
        spin_flips = (int *)PyArray_DATA(py_spin_flips);
    } else {
        spin_flips = NULL;
    }

    /* num_sym has to be larger than num_sym_from_array_size. */
    num_sym = spgms_get_symmetry_with_site_tensors(
        rot, trans, equiv_atoms, primitive_lattice, spin_flips,
        num_sym_from_array_size, lat, pos, types, tensors, tensor_rank,
        num_atom, with_time_reversal, is_axial, symprec, angle_tolerance,
        mag_symprec);
    return PyLong_FromLong((long)num_sym);
}

static PyObject *py_get_grid_point_from_address(PyObject *self,
                                                PyObject *args) {
    PyArrayObject *py_grid_address;
    PyArrayObject *py_mesh;

    int *grid_address;
    int *mesh;
    size_t gp;

    if (!PyArg_ParseTuple(args, "OO", &py_grid_address, &py_mesh)) {
        return NULL;
    }

    grid_address = (int *)PyArray_DATA(py_grid_address);
    mesh = (int *)PyArray_DATA(py_mesh);

    gp = spg_get_dense_grid_point_from_address(grid_address, mesh);

    return PyLong_FromSize_t(gp);
}

static PyObject *py_get_ir_reciprocal_mesh(PyObject *self, PyObject *args) {
    double symprec;
    PyArrayObject *py_grid_address;
    PyArrayObject *py_grid_mapping_table;
    PyArrayObject *py_mesh;
    PyArrayObject *py_is_shift;
    int is_time_reversal;
    PyArrayObject *py_lattice;
    PyArrayObject *py_positions;
    PyArrayObject *py_atom_types;

    double (*lat)[3];
    double (*pos)[3];
    int *types;
    int *mesh;
    int *is_shift;
    int num_atom;
    int (*grid_address)[3];
    int *grid_mapping_table_int;
    size_t *grid_mapping_table_size_t;
    int num_ir_int;
    size_t num_ir_size_t;

    if (!PyArg_ParseTuple(args, "OOOOiOOOd", &py_grid_address,
                          &py_grid_mapping_table, &py_mesh, &py_is_shift,
                          &is_time_reversal, &py_lattice, &py_positions,
                          &py_atom_types, &symprec)) {
        return NULL;
    }

    lat = (double (*)[3])PyArray_DATA(py_lattice);
    pos = (double (*)[3])PyArray_DATA(py_positions);
    types = (int *)PyArray_DATA(py_atom_types);
    mesh = (int *)PyArray_DATA(py_mesh);
    is_shift = (int *)PyArray_DATA(py_is_shift);
    num_atom = PyArray_DIMS(py_positions)[0];
    grid_address = (int (*)[3])PyArray_DATA(py_grid_address);
    if (PyArray_TYPE(py_grid_mapping_table) == NPY_UINTP) {
        grid_mapping_table_size_t =
            (size_t *)PyArray_DATA(py_grid_mapping_table);
        num_ir_size_t = spg_get_dense_ir_reciprocal_mesh(
            grid_address, grid_mapping_table_size_t, mesh, is_shift,
            is_time_reversal, lat, pos, types, num_atom, symprec);
        return PyLong_FromSize_t(num_ir_size_t);
    }
    if (PyArray_TYPE(py_grid_mapping_table) == NPY_INT) {
        grid_mapping_table_int = (int *)PyArray_DATA(py_grid_mapping_table);
        /* num_sym has to be larger than num_sym_from_array_size. */
        num_ir_int = spg_get_ir_reciprocal_mesh(
            grid_address, grid_mapping_table_int, mesh, is_shift,
            is_time_reversal, lat, pos, types, num_atom, symprec);
        return PyLong_FromLong((long)num_ir_int);
    }

    Py_RETURN_NONE;
}

static PyObject *py_get_stabilized_reciprocal_mesh(PyObject *self,
                                                   PyObject *args) {
    PyArrayObject *py_grid_address;
    PyArrayObject *py_grid_mapping_table;
    PyArrayObject *py_mesh;
    PyArrayObject *py_is_shift;
    int is_time_reversal;
    PyArrayObject *py_rotations;
    PyArrayObject *py_qpoints;

    int (*grid_address)[3];
    int *mesh;
    int *is_shift;
    int (*rot)[3][3];
    int num_rot;
    double (*q)[3];
    int num_q;

    int *grid_mapping_table_int;
    size_t *grid_mapping_table_size_t;
    int num_ir_int;
    size_t num_ir_size_t;

    if (!PyArg_ParseTuple(args, "OOOOiOO", &py_grid_address,
                          &py_grid_mapping_table, &py_mesh, &py_is_shift,
                          &is_time_reversal, &py_rotations, &py_qpoints)) {
        return NULL;
    }

    grid_address = (int (*)[3])PyArray_DATA(py_grid_address);
    mesh = (int *)PyArray_DATA(py_mesh);
    is_shift = (int *)PyArray_DATA(py_is_shift);
    rot = (int (*)[3][3])PyArray_DATA(py_rotations);
    num_rot = PyArray_DIMS(py_rotations)[0];
    q = (double (*)[3])PyArray_DATA(py_qpoints);
    num_q = PyArray_DIMS(py_qpoints)[0];

    if (PyArray_TYPE(py_grid_mapping_table) == NPY_UINTP) {
        grid_mapping_table_size_t =
            (size_t *)PyArray_DATA(py_grid_mapping_table);
        num_ir_size_t = spg_get_dense_stabilized_reciprocal_mesh(
            grid_address, grid_mapping_table_size_t, mesh, is_shift,
            is_time_reversal, num_rot, rot, num_q, q);
        return PyLong_FromSize_t(num_ir_size_t);
    }
    if (PyArray_TYPE(py_grid_mapping_table) == NPY_INT) {
        grid_mapping_table_int = (int *)PyArray_DATA(py_grid_mapping_table);
        num_ir_int = spg_get_stabilized_reciprocal_mesh(
            grid_address, grid_mapping_table_int, mesh, is_shift,
            is_time_reversal, num_rot, rot, num_q, q);
        return PyLong_FromLong((long)num_ir_int);
    }

    Py_RETURN_NONE;
}

static PyObject *py_get_grid_points_by_rotations(PyObject *self,
                                                 PyObject *args) {
    PyArrayObject *py_rot_grid_points;
    PyArrayObject *py_address_orig;
    PyArrayObject *py_rot_reciprocal;
    PyArrayObject *py_mesh;
    PyArrayObject *py_is_shift;

    size_t *rot_grid_points;
    int *address_orig;
    int (*rot_reciprocal)[3][3];
    int num_rot;
    int *mesh;
    int *is_shift;

    if (!PyArg_ParseTuple(args, "OOOOO", &py_rot_grid_points, &py_address_orig,
                          &py_rot_reciprocal, &py_mesh, &py_is_shift)) {
        return NULL;
    }

    rot_grid_points = (size_t *)PyArray_DATA(py_rot_grid_points);
    address_orig = (int *)PyArray_DATA(py_address_orig);
    rot_reciprocal = (int (*)[3][3])PyArray_DATA(py_rot_reciprocal);
    num_rot = PyArray_DIMS(py_rot_reciprocal)[0];
    mesh = (int *)PyArray_DATA(py_mesh);
    is_shift = (int *)PyArray_DATA(py_is_shift);

    spg_get_dense_grid_points_by_rotations(
        rot_grid_points, address_orig, num_rot, rot_reciprocal, mesh, is_shift);
    Py_RETURN_NONE;
}

static PyObject *py_get_BZ_grid_points_by_rotations(PyObject *self,
                                                    PyObject *args) {
    PyArrayObject *py_rot_grid_points;
    PyArrayObject *py_address_orig;
    PyArrayObject *py_rot_reciprocal;
    PyArrayObject *py_mesh;
    PyArrayObject *py_is_shift;
    PyArrayObject *py_bz_map;

    size_t *rot_grid_points;
    int *address_orig;
    int (*rot_reciprocal)[3][3];
    int num_rot;
    int *mesh;
    int *is_shift;
    size_t *bz_map;

    if (!PyArg_ParseTuple(args, "OOOOOO", &py_rot_grid_points, &py_address_orig,
                          &py_rot_reciprocal, &py_mesh, &py_is_shift,
                          &py_bz_map)) {
        return NULL;
    }

    rot_grid_points = (size_t *)PyArray_DATA(py_rot_grid_points);
    address_orig = (int *)PyArray_DATA(py_address_orig);
    rot_reciprocal = (int (*)[3][3])PyArray_DATA(py_rot_reciprocal);
    num_rot = PyArray_DIMS(py_rot_reciprocal)[0];
    mesh = (int *)PyArray_DATA(py_mesh);
    is_shift = (int *)PyArray_DATA(py_is_shift);
    bz_map = (size_t *)PyArray_DATA(py_bz_map);

    spg_get_dense_BZ_grid_points_by_rotations(rot_grid_points, address_orig,
                                              num_rot, rot_reciprocal, mesh,
                                              is_shift, bz_map);
    Py_RETURN_NONE;
}

static PyObject *py_relocate_BZ_grid_address(PyObject *self, PyObject *args) {
    PyArrayObject *py_bz_grid_address;
    PyArrayObject *py_bz_map;
    PyArrayObject *py_grid_address;
    PyArrayObject *py_mesh;
    PyArrayObject *py_is_shift;
    PyArrayObject *py_reciprocal_lattice;

    int (*bz_grid_address)[3];
    size_t *bz_map;
    int (*grid_address)[3];
    int *mesh;
    int *is_shift;
    double (*reciprocal_lattice)[3];
    size_t num_ir_gp;

    if (!PyArg_ParseTuple(args, "OOOOOO", &py_bz_grid_address, &py_bz_map,
                          &py_grid_address, &py_mesh, &py_reciprocal_lattice,
                          &py_is_shift)) {
        return NULL;
    }

    bz_grid_address = (int (*)[3])PyArray_DATA(py_bz_grid_address);
    bz_map = (size_t *)PyArray_DATA(py_bz_map);
    grid_address = (int (*)[3])PyArray_DATA(py_grid_address);
    mesh = (int *)PyArray_DATA(py_mesh);
    is_shift = (int *)PyArray_DATA(py_is_shift);
    reciprocal_lattice = (double (*)[3])PyArray_DATA(py_reciprocal_lattice);

    num_ir_gp = spg_relocate_dense_BZ_grid_address(
        bz_grid_address, bz_map, grid_address, mesh, reciprocal_lattice,
        is_shift);

    return PyLong_FromSize_t(num_ir_gp);
}

static PyObject *py_delaunay_reduce(PyObject *self, PyObject *args) {
    PyArrayObject *py_lattice;
    double symprec;

    double (*lattice)[3];
    int result;

    if (!PyArg_ParseTuple(args, "Od", &py_lattice, &symprec)) {
        return NULL;
    }

    lattice = (double (*)[3])PyArray_DATA(py_lattice);

    result = spg_delaunay_reduce(lattice, symprec);

    return PyLong_FromLong((long)result);
}

static PyObject *py_niggli_reduce(PyObject *self, PyObject *args) {
    PyArrayObject *py_lattice;
    double eps;

    double (*lattice)[3];
    int result;

    if (!PyArg_ParseTuple(args, "Od", &py_lattice, &eps)) {
        return NULL;
    }

    lattice = (double (*)[3])PyArray_DATA(py_lattice);

    result = spg_niggli_reduce(lattice, eps);

    return PyLong_FromLong((long)result);
}

/* Deprecated at v2.0 */
static PyObject *py_get_hall_number_from_symmetry(PyObject *self,
                                                  PyObject *args) {
    double symprec;
    PyArrayObject *py_rotations;
    PyArrayObject *py_translations;

    int (*rot)[3][3];
    double (*trans)[3];
    int num_sym;
    int hall_number;

    if (!PyArg_ParseTuple(args, "OOd", &py_rotations, &py_translations,
                          &symprec)) {
        return NULL;
    }

    rot = (int (*)[3][3])PyArray_DATA(py_rotations);
    trans = (double (*)[3])PyArray_DATA(py_translations);
    num_sym = PyArray_DIMS(py_rotations)[0];

    hall_number =
        spg_get_hall_number_from_symmetry(rot, trans, num_sym, symprec);

    return PyLong_FromLong((long)hall_number);
}

static PyObject *py_get_error_message(PyObject *self, PyObject *args) {
    SpglibError error;

    if (!PyArg_ParseTuple(args, "")) {
        return NULL;
    }

    error = spg_get_error_code();

    return PyUnicode_FromString(spg_get_error_message(error));
}
