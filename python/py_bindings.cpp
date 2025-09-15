// Copyright (C) 2025 Spglib team
// SPDX-License-Identifier: BSD-3-Clause

#include <spglib.h>

#include "py_bindings.h"

using namespace spglib;

py::tuple spglib::version_tuple() {
    py::tuple version(3);
    version[0] = spg_get_major_version();
    version[1] = spg_get_minor_version();
    version[2] = spg_get_micro_version();
    return version;
}
py::str spglib::version_string() { return spg_get_version(); }
py::str spglib::version_full() { return spg_get_version_full(); }
py::str spglib::commit() { return spg_get_commit(); }

py::list build_python_list_from_dataset(SpglibDataset *dataset) {
    py::list array{21};
    array[0] = dataset->spacegroup_number;
    array[1] = dataset->hall_number;
    array[2] = dataset->international_symbol;
    array[3] = dataset->hall_symbol;
    array[4] = dataset->choice;
    {
        array_double transformation_matrix({3, 3});
        array_double origin_shift(3);
        for (auto i = 0; i < 3; i++) {
            for (auto j = 0; j < 3; j++)
                transformation_matrix.mutable_at(i, j) =
                    dataset->transformation_matrix[i][j];
            origin_shift.mutable_at(i) = dataset->origin_shift[i];
        }
        array[5] = transformation_matrix;
        array[6] = origin_shift;
    }
    {
        array_int rotations({dataset->n_operations, 3, 3});
        array_double translations({dataset->n_operations, 3});
        for (auto ind_oper = 0; ind_oper < dataset->n_operations; ind_oper++)
            for (auto i = 0; i < 3; i++) {
                for (auto j = 0; j < 3; j++)
                    rotations.mutable_at(ind_oper, i, j) =
                        dataset->rotations[ind_oper][i][j];
                translations.mutable_at(ind_oper, i) =
                    dataset->translations[ind_oper][i];
            }
        array[7] = rotations;
        array[8] = translations;
    }
    {
        array_int wyckoffs(dataset->n_atoms);
        py::list site_symmetry_symbols(dataset->n_atoms);
        array_int crystallographic_orbits(dataset->n_atoms);
        array_int equiv_atoms(dataset->n_atoms);
        array_double primitive_lattice({3, 3});
        array_int mapping_to_primitive(dataset->n_atoms);
        array_double std_lattice({3, 3});
        for (auto ind_atom = 0; ind_atom < dataset->n_atoms; ind_atom++) {
            wyckoffs.mutable_at(ind_atom) = dataset->wyckoffs[ind_atom];
            site_symmetry_symbols[ind_atom] =
                dataset->site_symmetry_symbols[ind_atom];
            crystallographic_orbits.mutable_at(ind_atom) =
                dataset->crystallographic_orbits[ind_atom];
            equiv_atoms.mutable_at(ind_atom) =
                dataset->equivalent_atoms[ind_atom];
            mapping_to_primitive.mutable_at(ind_atom) =
                dataset->mapping_to_primitive[ind_atom];
        }
        for (auto i = 0; i < 3; i++)
            for (auto j = 0; j < 3; j++) {
                primitive_lattice.mutable_at(i, j) =
                    dataset->primitive_lattice[i][j];
                std_lattice.mutable_at(i, j) = dataset->std_lattice[i][j];
            }
        array[9] = wyckoffs;
        array[10] = site_symmetry_symbols;
        array[11] = crystallographic_orbits;
        array[12] = equiv_atoms;
        array[13] = primitive_lattice;
        array[14] = mapping_to_primitive;
        array[15] = std_lattice;
    }
    {
        array_int std_types(dataset->n_std_atoms);
        array_double std_positions({dataset->n_std_atoms, 3});
        array_double std_rotations({3, 3});
        array_int std_mapping_to_primitive(dataset->n_std_atoms);
        for (auto ind_atom = 0; ind_atom < dataset->n_std_atoms; ind_atom++) {
            std_types.mutable_at(ind_atom) = dataset->std_types[ind_atom];
            for (auto i = 0; i < 3; i++)
                std_positions.mutable_at(ind_atom, i) =
                    dataset->std_positions[ind_atom][i];
            std_mapping_to_primitive.mutable_at(ind_atom) =
                dataset->std_mapping_to_primitive[ind_atom];
        }
        for (auto i = 0; i < 3; i++)
            for (auto j = 0; j < 3; j++)
                std_rotations.mutable_at(i, j) =
                    dataset->std_rotation_matrix[i][j];
        array[16] = std_types;
        array[17] = std_positions;
        array[18] = std_rotations;
        array[19] = std_mapping_to_primitive;
    }
    array[20] = dataset->pointgroup_symbol;
    return array;
}

py::list build_python_list_from_magnetic_dataset(SpglibMagneticDataset *dataset,
                                                 int tensor_rank) {
    py::list array{19};
    array[0] = dataset->uni_number;
    array[1] = dataset->msg_type;
    array[2] = dataset->hall_number;
    array[3] = dataset->tensor_rank;
    array[4] = dataset->n_operations;
    {
        array_int rotations({dataset->n_operations, 3, 3});
        array_double translations({dataset->n_operations, 3});
        array_int time_reversal({dataset->n_operations});
        for (auto ind_oper = 0; ind_oper < dataset->n_operations; ind_oper++) {
            for (auto i = 0; i < 3; i++) {
                for (auto j = 0; j < 3; j++)
                    rotations.mutable_at(ind_oper, i, j) =
                        dataset->rotations[ind_oper][i][j];
                translations.mutable_at(ind_oper, i) =
                    dataset->translations[ind_oper][i];
            }
            time_reversal.mutable_at(ind_oper) =
                dataset->time_reversals[ind_oper];
        }
        array[5] = rotations;
        array[6] = translations;
        array[7] = time_reversal;
    }
    array[8] = dataset->n_atoms;
    {
        array_int equiv_atoms(dataset->n_atoms);
        for (auto ind_atom = 0; ind_atom < dataset->n_atoms; ind_atom++)
            equiv_atoms.mutable_at(ind_atom) =
                dataset->equivalent_atoms[ind_atom];
        array[9] = equiv_atoms;
    }
    {
        array_double transformation_matrix({3, 3});
        array_double origin_shift(3);
        for (auto i = 0; i < 3; i++) {
            for (auto j = 0; j < 3; j++)
                transformation_matrix.mutable_at(i, j) =
                    dataset->transformation_matrix[i][j];
            origin_shift.mutable_at(i) = dataset->origin_shift[i];
        }
        array[10] = transformation_matrix;
        array[11] = origin_shift;
    }
    array[12] = dataset->n_std_atoms;
    {
        array_double std_lattice({3, 3});
        for (auto i = 0; i < 3; i++)
            for (auto j = 0; j < 3; j++)
                std_lattice.mutable_at(i, j) = dataset->std_lattice[i][j];
        array[13] = std_lattice;
    }
    {
        array_int std_types(dataset->n_std_atoms);
        array_double std_positions({dataset->n_std_atoms, 3});
        for (auto ind_atom = 0; ind_atom < dataset->n_std_atoms; ind_atom++) {
            std_types.mutable_at(ind_atom) = dataset->std_types[ind_atom];
            for (auto i = 0; i < 3; i++)
                std_positions.mutable_at(ind_atom, i) =
                    dataset->std_positions[ind_atom][i];
        }
        array[14] = std_types;
        array[15] = std_positions;
    }
    {
        int n_tensors = dataset->n_std_atoms;
        if (tensor_rank == 1) n_tensors *= 3;
        array_double std_tensors{n_tensors};
        for (auto ind_tensor = 0; ind_tensor < n_tensors; ind_tensor++)
            std_tensors.mutable_at(ind_tensor) =
                dataset->std_tensors[ind_tensor];
        array[16] = std_tensors;
    }
    {
        array_double std_rotations({3, 3});
        array_double primitive_lattice({3, 3});
        for (auto i = 0; i < 3; i++)
            for (auto j = 0; j < 3; j++) {
                std_rotations.mutable_at(i, j) =
                    dataset->std_rotation_matrix[i][j];
                primitive_lattice.mutable_at(i, j) =
                    dataset->primitive_lattice[i][j];
            }
        array[17] = std_rotations;
        array[18] = primitive_lattice;
    }
    return array;
}

py::list build_python_list_from_spacegroup_type(
    SpglibSpacegroupType &spg_type) {
    py::list array{12};
    array[0] = spg_type.number;
    array[1] = spg_type.international_short;
    array[2] = spg_type.international_full;
    array[3] = spg_type.international;
    array[4] = spg_type.schoenflies;
    array[5] = spg_type.hall_number;
    array[6] = spg_type.hall_symbol;
    array[7] = spg_type.choice;
    array[8] = spg_type.pointgroup_international;
    array[9] = spg_type.pointgroup_schoenflies;
    array[10] = spg_type.arithmetic_crystal_class_number;
    array[11] = spg_type.arithmetic_crystal_class_symbol;
    return array;
}

py::list build_python_list_from_magnetic_spacegroup_type(
    SpglibMagneticSpacegroupType &spg_type) {
    py::list array{6};
    array[0] = spg_type.uni_number;
    array[1] = spg_type.litvin_number;
    array[2] = spg_type.bns_number;
    array[3] = spg_type.og_number;
    array[4] = spg_type.number;
    array[5] = spg_type.type;
    return array;
}

std::optional<py::list> spglib::dataset(
    array_double lattice, array_double positions, array_int atom_types,
    py::int_ hall_number, py::float_ symprec, py::float_ angle_tolerance) {
    SpglibDataset *dataset;
    if ((dataset = spgat_get_dataset_with_hall_number(
             (double (*)[3])lattice.data(), (double (*)[3])positions.data(),
             atom_types.data(), atom_types.size(), hall_number, symprec,
             angle_tolerance)) == nullptr)
        return {};
    auto array = build_python_list_from_dataset(dataset);
    spg_free_dataset(dataset);
    return array;
}
std::optional<py::list> spglib::layer_dataset(array_double lattice,
                                              array_double positions,
                                              array_int atom_types,
                                              py::int_ aperiodic_dir,
                                              py::float_ symprec) {
    SpglibDataset *dataset;
    if ((dataset = spg_get_layer_dataset(
             (double (*)[3])lattice.data(), (double (*)[3])positions.data(),
             atom_types.data(), atom_types.size(), aperiodic_dir, symprec)) ==
        nullptr)
        return {};
    auto array = build_python_list_from_dataset(dataset);
    spg_free_dataset(dataset);
    return array;
}
std::optional<py::list> spglib::magnetic_dataset(
    array_double lattice, array_double positions, array_int atom_types,
    array_double magmoms, py::int_ tensor_rank, py::bool_ is_axial,
    py::float_ symprec, py::float_ angle_tolerance, py::float_ mag_symprec) {
    SpglibMagneticDataset *dataset;
    if ((dataset = spgms_get_magnetic_dataset(
             (double (*)[3])lattice.data(), (double (*)[3])positions.data(),
             atom_types.data(), magmoms.data(), tensor_rank, positions.shape(0),
             is_axial * 1, symprec, angle_tolerance, mag_symprec)) == nullptr)
        return {};
    switch (int(tensor_rank)) {
        case 0:
        case 1:
            break;
        default:
            // Invalid, return None
            return {};
    }
    auto array = build_python_list_from_magnetic_dataset(dataset, tensor_rank);
    spg_free_magnetic_dataset(dataset);
    return array;
}
std::optional<py::list> spglib::spacegroup_type(py::int_ hall_number) {
    auto spg_type = spg_get_spacegroup_type(hall_number);
    if (spg_type.number == 0) return {};
    return build_python_list_from_spacegroup_type(spg_type);
}
std::optional<py::list> spglib::spacegroup_type_from_symmetry(
    array_int rotations, array_double translations, array_double lattice,
    py::float_ symprec) {
    auto spg_type = spg_get_spacegroup_type_from_symmetry(
        (int (*)[3][3])rotations.data(), (double (*)[3])translations.data(),
        rotations.shape(0), (double (*)[3])lattice.data(), symprec);
    if (spg_type.number == 0) return {};
    return build_python_list_from_spacegroup_type(spg_type);
}
std::optional<py::list> spglib::magnetic_spacegroup_type(py::int_ uni_number) {
    auto msg_type = spg_get_magnetic_spacegroup_type(uni_number);
    if (msg_type.number == 0) return {};
    return build_python_list_from_magnetic_spacegroup_type(msg_type);
}
std::optional<py::list> spglib::magnetic_spacegroup_type_from_symmetry(
    array_int rotations, array_double translations, array_int time_reversals,
    array_double lattice, py::float_ symprec) {
    auto msg_type = spg_get_magnetic_spacegroup_type_from_symmetry(
        (int (*)[3][3])rotations.data(), (double (*)[3])translations.data(),
        (int *)time_reversals.data(), time_reversals.size(),
        (double (*)[3])lattice.data(), symprec);
    if (msg_type.number == 0) return {};
    return build_python_list_from_magnetic_spacegroup_type(msg_type);
}
std::optional<py::int_> spglib::symmetry_from_database(
    array_int rotations, array_double translations, py::int_ hall_number) {
    if (rotations.shape(0) < 192 || translations.shape(0) < 192) return {};
    return spg_get_symmetry_from_database((int (*)[3][3])rotations.data(),
                                          (double (*)[3])translations.data(),
                                          hall_number);
}
std::optional<py::int_> spglib::magnetic_symmetry_from_database(
    array_int rotations, array_double translations, array_int time_reversals,
    py::int_ uni_number, py::int_ hall_number) {
    if (rotations.shape(0) < 384 || translations.shape(0) < 384 ||
        time_reversals.shape(0) < 384)
        return {};
    return spg_get_magnetic_symmetry_from_database(
        (int (*)[3][3])rotations.data(), (double (*)[3])translations.data(),
        (int *)time_reversals.data(), uni_number, hall_number);
}
std::optional<py::tuple> spglib::pointgroup(array_int rotations) {
    char symbol[6];
    array_int transf_matrix({3, 3});
    auto ptg_num =
        spg_get_pointgroup(symbol, (int (*)[3])transf_matrix.mutable_data(),
                           (int (*)[3][3])rotations.data(), rotations.shape(0));

    py::list array(3);
    array[0] = symbol;
    array[1] = ptg_num;
    array[2] = transf_matrix;
    return array;
}
std::optional<py::int_> spglib::standardize_cell(
    array_double lattice, array_double positions, array_int atom_types,
    py::int_ num_atom, py::int_ to_primative, py::int_ no_idealize,
    py::float_ symprec, py::float_ angle_tolerance) {
    return spgat_standardize_cell(
        (double (*)[3])lattice.mutable_data(),
        (double (*)[3])positions.mutable_data(), atom_types.mutable_data(),
        num_atom, to_primative, no_idealize, symprec, angle_tolerance);
}
std::optional<py::int_> spglib::refine_cell(
    array_double lattice, array_double positions, array_int atom_types,
    py::int_ num_atom, py::float_ symprec, py::float_ angle_tolerance) {
    return spgat_refine_cell((double (*)[3])lattice.mutable_data(),
                             (double (*)[3])positions.mutable_data(),
                             atom_types.mutable_data(), num_atom, symprec,
                             angle_tolerance);
}
std::optional<py::int_> spglib::symmetry(
    array_int rotations, array_double translations, array_double lattice,
    array_double positions, array_int atom_types, py::float_ symprec,
    py::float_ angle_tolerance) {
    return spgat_get_symmetry(
        (int (*)[3][3])rotations.mutable_data(),
        (double (*)[3])translations.mutable_data(), rotations.shape(0),
        (double (*)[3])lattice.data(), (double (*)[3])positions.data(),
        atom_types.data(), atom_types.size(), symprec, angle_tolerance);
}
std::optional<py::int_> spglib::symmetry_with_collinear_spin(
    array_int rotations, array_double translations, array_int equiv_atoms,
    array_double lattice, array_double positions, array_int atom_types,
    array_double magmoms, py::float_ symprec, py::float_ angle_tolerance) {
    return spgat_get_symmetry_with_collinear_spin(
        (int (*)[3][3])rotations.mutable_data(),
        (double (*)[3])translations.mutable_data(), equiv_atoms.mutable_data(),
        equiv_atoms.size(), (double (*)[3])lattice.data(),
        (double (*)[3])positions.data(), atom_types.data(), magmoms.data(),
        atom_types.size(), symprec, angle_tolerance);
}
std::optional<py::int_> spglib::symmetry_with_site_tensors(
    array_int rotations, array_double translations, array_int equiv_atoms,
    array_double primitive_lattice, array_int spin_flips, array_double lattice,
    array_double positions, array_int atom_types, array_double tensors,
    py::int_ with_time_reversal, py::int_ is_axial, py::float_ symprec,
    py::float_ angle_tolerance, py::float_ mag_symprec) {
    int tensor_rank = tensors.ndim() - 1;
    int *spin_flips_ptr;
    switch (tensor_rank) {
        case 0:
        case 1:
            spin_flips_ptr = spin_flips.mutable_data();
            break;
        default:
            spin_flips_ptr = nullptr;
    }
    return spgms_get_symmetry_with_site_tensors(
        (int (*)[3][3])rotations.mutable_data(),
        (double (*)[3])translations.mutable_data(), equiv_atoms.mutable_data(),
        (double (*)[3])primitive_lattice.mutable_data(), spin_flips_ptr,
        rotations.shape(0), (double (*)[3])lattice.data(),
        (double (*)[3])positions.data(), atom_types.data(), tensors.data(),
        tensor_rank, atom_types.size(), with_time_reversal, is_axial, symprec,
        angle_tolerance, mag_symprec);
}
std::optional<py::int_> spglib::primitive(array_double lattice,
                                          array_double positions,
                                          array_int atom_types,
                                          py::float_ symprec,
                                          py::float_ angle_tolerance) {
    return spgat_find_primitive((double (*)[3])lattice.mutable_data(),
                                (double (*)[3])positions.mutable_data(),
                                atom_types.mutable_data(), atom_types.size(),
                                symprec, angle_tolerance);
}
std::optional<py::int_> spglib::grid_point_from_address(array_int grid_address,
                                                        array_int mesh) {
    return spg_get_dense_grid_point_from_address(grid_address.data(),
                                                 mesh.data());
}
std::optional<py::int_> spglib::ir_reciprocal_mesh(
    array_int grid_address, array_int grid_mapping_table, array_int mesh,
    array_int is_shift, py::int_ is_time_reversal, array_double lattice,
    array_double positions, array_int atom_types, py::float_ symprec) {
    return spg_get_ir_reciprocal_mesh(
        (int (*)[3])grid_address.mutable_data(),
        grid_mapping_table.mutable_data(), mesh.data(), is_shift.data(),
        is_time_reversal, (double (*)[3])lattice.data(),
        (double (*)[3])positions.data(), atom_types.data(), atom_types.size(),
        symprec);
}
std::optional<py::int_> spglib::ir_reciprocal_mesh(
    array_int grid_address, array_size_t grid_mapping_table, array_int mesh,
    array_int is_shift, py::int_ is_time_reversal, array_double lattice,
    array_double positions, array_int atom_types, py::float_ symprec) {
    return spg_get_dense_ir_reciprocal_mesh(
        (int (*)[3])grid_address.mutable_data(),
        grid_mapping_table.mutable_data(), mesh.data(), is_shift.data(),
        is_time_reversal, (double (*)[3])lattice.data(),
        (double (*)[3])positions.data(), atom_types.data(), atom_types.size(),
        symprec);
}
std::optional<py::int_> spglib::stabilized_reciprocal_mesh(
    array_int grid_address, array_int grid_mapping_table, array_int mesh,
    array_int is_shift, py::int_ is_time_reversal, array_int rotations,
    array_double qpoints) {
    return spg_get_stabilized_reciprocal_mesh(
        (int (*)[3])grid_address.mutable_data(),
        grid_mapping_table.mutable_data(), mesh.data(), is_shift.data(),
        is_time_reversal, rotations.shape(0), (int (*)[3][3])rotations.data(),
        qpoints.shape(0), (double (*)[3])qpoints.data());
}
std::optional<py::int_> spglib::stabilized_reciprocal_mesh(
    array_int grid_address, array_size_t grid_mapping_table, array_int mesh,
    array_int is_shift, py::int_ is_time_reversal, array_int rotations,
    array_double qpoints) {
    return spg_get_dense_stabilized_reciprocal_mesh(
        (int (*)[3])grid_address.mutable_data(),
        grid_mapping_table.mutable_data(), mesh.data(), is_shift.data(),
        is_time_reversal, rotations.shape(0), (int (*)[3][3])rotations.data(),
        qpoints.shape(0), (double (*)[3])qpoints.data());
}
void spglib::grid_points_by_rotations(array_size_t rot_grid_points,
                                      array_int address_orig,
                                      array_int rot_reciprocal, array_int mesh,
                                      array_int is_shift) {
    spg_get_dense_grid_points_by_rotations(
        rot_grid_points.mutable_data(), address_orig.data(),
        rot_reciprocal.shape(0), (int (*)[3][3])rot_reciprocal.data(),
        mesh.data(), is_shift.data());
}
void spglib::BZ_grid_points_by_rotations(array_size_t rot_grid_points,
                                         array_int address_orig,
                                         array_int rot_reciprocal,
                                         array_int mesh, array_int is_shift,
                                         array_size_t bz_map) {
    spg_get_dense_BZ_grid_points_by_rotations(
        rot_grid_points.mutable_data(), address_orig.data(),
        rot_reciprocal.shape(0), (int (*)[3][3])rot_reciprocal.data(),
        mesh.data(), is_shift.data(), bz_map.data());
}
std::optional<py::int_> spglib::BZ_grid_address(
    array_int bz_grid_address, array_size_t bz_map, array_int grid_address,
    array_int mesh, array_double reciprocal_lattice, array_int is_shift) {
    return spg_relocate_dense_BZ_grid_address(
        (int (*)[3])bz_grid_address.mutable_data(), bz_map.mutable_data(),
        (int (*)[3])grid_address.data(), mesh.data(),
        (double (*)[3])reciprocal_lattice.data(), is_shift.data());
}
std::optional<py::int_> spglib::delaunay_reduce(array_double lattice,
                                                py::float_ symprec) {
    return spg_delaunay_reduce((double (*)[3])lattice.mutable_data(), symprec);
}
std::optional<py::int_> spglib::niggli_reduce(array_double lattice,
                                              py::float_ eps) {
    return spg_niggli_reduce((double (*)[3])lattice.mutable_data(), eps);
}
std::optional<py::int_> spglib::hall_number_from_symmetry(
    array_int rotations, array_double translations, py::float_ symprec) {
    return spg_get_hall_number_from_symmetry((int (*)[3][3])rotations.data(),
                                             (double (*)[3])translations.data(),
                                             rotations.shape(0), symprec);
}
py::str spglib::error_message() {
    return spg_get_error_message(spg_get_error_code());
}
