// Copyright (C) 2025 Spglib team
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include <optional>

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
namespace spglib {
using array_double =
    py::array_t<double, py::array::c_style | py::array::forcecast>;
using array_int = py::array_t<int, py::array::c_style | py::array::forcecast>;
using array_uintp =
    py::array_t<uintptr_t, py::array::c_style | py::array::forcecast>;
using array_size_t =
    py::array_t<size_t, py::array::c_style | py::array::forcecast>;
py::tuple version_tuple();
py::str version_string();
py::str version_full();
py::str commit();
std::optional<py::list> dataset(array_double lattice, array_double positions,
                                array_int atom_types, py::int_ hall_number,
                                py::float_ symprec, py::float_ angle_tolerance);
std::optional<py::list> layer_dataset(array_double lattice,
                                      array_double positions,
                                      array_int atom_types,
                                      py::int_ aperiodic_dir,
                                      py::float_ symprec);
std::optional<py::list> magnetic_dataset(
    array_double lattice, array_double positions, array_int atom_types,
    array_double magmoms, py::int_ tensor_rank, py::bool_ is_axial,
    py::float_ symprec, py::float_ angle_tolerance, py::float_ mag_symprec);
std::optional<py::list> spacegroup_type(py::int_ hall_number);
std::optional<py::list> spacegroup_type_from_symmetry(array_int rotations,
                                                      array_double translations,
                                                      array_double lattice,
                                                      py::float_ symprec);
std::optional<py::list> magnetic_spacegroup_type(py::int_ uni_number);
std::optional<py::list> magnetic_spacegroup_type_from_symmetry(
    array_int rotations, array_double translations, array_int time_reversals,
    array_double lattice, py::float_ symprec);
std::optional<py::int_> symmetry_from_database(array_int rotations,
                                               array_double translations,
                                               py::int_ hall_number);
std::optional<py::int_> magnetic_symmetry_from_database(
    array_int rotations, array_double translations, array_int time_reversals,
    py::int_ uni_number, py::int_ hall_number);
std::optional<py::tuple> pointgroup(array_int rotations);
std::optional<py::int_> standardize_cell(
    array_double lattice, array_double positions, array_int atom_types,
    py::int_ num_atom, py::int_ to_primative, py::int_ no_idealize,
    py::float_ symprec, py::float_ angle_tolerance);
std::optional<py::int_> refine_cell(array_double lattice,
                                    array_double positions,
                                    array_int atom_types, py::int_ num_atom,
                                    py::float_ symprec,
                                    py::float_ angle_tolerance);
std::optional<py::int_> symmetry(array_int rotations, array_double translations,
                                 array_double lattice, array_double positions,
                                 array_int atom_types, py::float_ symprec,
                                 py::float_ angle_tolerance);
std::optional<py::int_> symmetry_with_collinear_spin(
    array_int rotations, array_double translations, array_int equiv_atoms,
    array_double lattice, array_double positions, array_int atom_types,
    array_double magmoms, py::float_ symprec, py::float_ angle_tolerance);
std::optional<py::int_> symmetry_with_site_tensors(
    array_int rotations, array_double translations, array_int equiv_atoms,
    array_double primitive_lattice, array_int spin_flips, array_double lattice,
    array_double positions, array_int atom_types, array_double tensors,
    py::int_ with_time_reversal, py::int_ is_axial, py::float_ symprec,
    py::float_ angle_tolerance, py::float_ mag_symprec);
std::optional<py::int_> primitive(array_double lattice, array_double positions,
                                  array_int atom_types, py::float_ symprec,
                                  py::float_ angle_tolerance);
std::optional<py::int_> grid_point_from_address(array_int grid_address,
                                                array_int mesh);
std::optional<py::int_> ir_reciprocal_mesh(
    array_int grid_address, array_int grid_mapping_table, array_int mesh,
    array_int is_shift, py::int_ is_time_reversal, array_double lattice,
    array_double positions, array_int atom_types, py::float_ symprec);
std::optional<py::int_> ir_reciprocal_mesh(
    array_int grid_address, array_size_t grid_mapping_table, array_int mesh,
    array_int is_shift, py::int_ is_time_reversal, array_double lattice,
    array_double positions, array_int atom_types, py::float_ symprec);
std::optional<py::int_> stabilized_reciprocal_mesh(
    array_int grid_address, array_int grid_mapping_table, array_int mesh,
    array_int is_shift, py::int_ is_time_reversal, array_int rotations,
    array_double qpoints);
std::optional<py::int_> stabilized_reciprocal_mesh(
    array_int grid_address, array_size_t grid_mapping_table, array_int mesh,
    array_int is_shift, py::int_ is_time_reversal, array_int rotations,
    array_double qpoints);
void grid_points_by_rotations(array_size_t rot_grid_points,
                              array_int address_orig, array_int rot_reciprocal,
                              array_int mesh, array_int is_shift);
void BZ_grid_points_by_rotations(array_size_t rot_grid_points,
                                 array_int address_orig,
                                 array_int rot_reciprocal, array_int mesh,
                                 array_int is_shift, array_size_t bz_map);
std::optional<py::int_> BZ_grid_address(array_int bz_grid_address,
                                        array_size_t bz_map,
                                        array_int grid_address, array_int mesh,
                                        array_double reciprocal_lattice,
                                        array_int is_shift);
std::optional<py::int_> delaunay_reduce(array_double lattice,
                                        py::float_ symprec);
std::optional<py::int_> niggli_reduce(array_double lattice, py::float_ eps);
std::optional<py::int_> hall_number_from_symmetry(array_int rotations,
                                                  array_double translations,
                                                  py::float_ symprec);
py::str error_message();
}  // namespace spglib
