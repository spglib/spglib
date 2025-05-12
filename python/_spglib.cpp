// Copyright (C) 2025 Spglib team
// SPDX-License-Identifier: BSD-3-Clause

#include <pybind11/pybind11.h>

#include "py_bindings.h"

PYBIND11_MODULE(_spglib, module) {
    using namespace py::literals;
    using namespace spglib;
    module.doc() = "Spglib compiled bindings.";
    module.def("version_tuple", spglib::version_tuple, "");
    module.def("version_string", spglib::version_string, "");
    module.def("version_full", spglib::version_full, "");
    module.def("commit", spglib::commit, "");
    module.def("dataset", spglib::dataset, "");
    module.def("layer_dataset", spglib::layer_dataset, "");
    module.def("magnetic_dataset", spglib::magnetic_dataset, "");
    module.def("spacegroup_type", spglib::spacegroup_type, "");
    module.def("spacegroup_type_from_symmetry",
               spglib::spacegroup_type_from_symmetry, "");
    module.def("magnetic_spacegroup_type", spglib::magnetic_spacegroup_type,
               "");
    module.def("magnetic_spacegroup_type_from_symmetry",
               spglib::magnetic_spacegroup_type_from_symmetry, "");
    module.def("symmetry_from_database", spglib::symmetry_from_database, "");
    module.def("magnetic_symmetry_from_database",
               spglib::magnetic_symmetry_from_database, "");
    module.def("pointgroup", spglib::pointgroup, "");
    module.def("standardize_cell", spglib::standardize_cell, "");
    module.def("refine_cell", spglib::refine_cell, "");
    module.def("symmetry", spglib::symmetry, "");
    module.def("symmetry_with_collinear_spin",
               spglib::symmetry_with_collinear_spin, "");
    module.def("symmetry_with_site_tensors", spglib::symmetry_with_site_tensors,
               "");
    module.def("primitive", spglib::primitive, "");
    module.def("grid_point_from_address", spglib::grid_point_from_address, "");
    module.def(
        "ir_reciprocal_mesh",
        py::overload_cast<array_int, array_int, array_int, array_int, py::int_,
                          array_double, array_double, array_int, py::float_>(
            spglib::ir_reciprocal_mesh),
        "");
    module.def(
        "ir_reciprocal_mesh",
        py::overload_cast<array_int, array_size_t, array_int, array_int,
                          py::int_, array_double, array_double, array_int,
                          py::float_>(spglib::ir_reciprocal_mesh),
        "");
    module.def("stabilized_reciprocal_mesh",
               py::overload_cast<array_int, array_int, array_int, array_int,
                                 py::int_, array_int, array_double>(
                   spglib::stabilized_reciprocal_mesh),
               "");
    module.def("stabilized_reciprocal_mesh",
               py::overload_cast<array_int, array_size_t, array_int, array_int,
                                 py::int_, array_int, array_double>(
                   spglib::stabilized_reciprocal_mesh),
               "");
    module.def("grid_points_by_rotations", spglib::grid_points_by_rotations,
               "");
    module.def("BZ_grid_points_by_rotations",
               spglib::BZ_grid_points_by_rotations, "");
    module.def("BZ_grid_address", spglib::BZ_grid_address, "");
    module.def("delaunay_reduce", spglib::delaunay_reduce, "");
    module.def("niggli_reduce", spglib::niggli_reduce, "");
    module.def("hall_number_from_symmetry", spglib::hall_number_from_symmetry,
               "");
    module.def("error_message", spglib::error_message, "");
}
