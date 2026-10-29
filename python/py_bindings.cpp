// Copyright (C) 2025 Spglib team
// SPDX-License-Identifier: BSD-3-Clause

#include <spglib.h>

#include <cmath>

#include "py_bindings.h"

using namespace spglib;

auto unkown_error_msg = "Unknown Spglib error, please report upstream.";

class Spglib_classic_exception : public spglib::SpglibError {
    static char const *_get_current_error_msg() {
        auto msg = spg_get_error_message(spg_get_error_code());
        if (msg == nullptr) msg = unkown_error_msg;
        return msg;
    }

   public:
    Spglib_classic_exception() : SpglibError{_get_current_error_msg()} {}
};

void try_throw_error() {
    auto msg = spg_get_error_message(spg_get_error_code());
    if (msg == nullptr) msg = unkown_error_msg;
    throw spglib::SpglibError(msg);
}

Lattice::Lattice(array_double &&_array)
    : array{std::forward<array_double>(_array)} {
    if (array.ndim() != 2) throw SpglibError("Lattice ndim is not 2");
    if (array.shape(0) != 3 || array.shape(1) != 3)
        throw SpglibError("Lattice is not a 3x3 matrix");
    for (auto i = 0; i < array.size(); ++i) {
        if (!std::isfinite(array.data()[i]))
            throw SpglibError("Lattice contains non-finite values");
    }
}
double (*Lattice::data())[3] {
    return reinterpret_cast<double (*)[3]>(array.mutable_data());
}
double const (*Lattice::data() const)[3] {
    return reinterpret_cast<double const(*)[3]>(array.data());
}
Rotations::Rotations(array_int &&_array)
    : array{std::forward<array_int>(_array)},
      n_operations(static_cast<int>(array.shape(0))) {
    if (array.ndim() != 3) throw SpglibError("Rotations ndim is not 3");
    if (array.shape(1) != 3 || array.shape(2) != 3)
        throw SpglibError("Lattice is not a nx3x3 matrix");
}
int (*Rotations::data())[3][3] {
    return reinterpret_cast<int (*)[3][3]>(array.mutable_data());
}
int const (*Rotations::data() const)[3][3] {
    return reinterpret_cast<int const(*)[3][3]>(array.data());
}
Translations::Translations(array_double &&_array)
    : array{std::forward<array_double>(_array)},
      n_operations(static_cast<int>(array.shape(0))) {
    if (array.ndim() != 2) throw SpglibError("Rotations ndim is not 3");
    if (array.shape(1) != 3) throw SpglibError("Lattice is not a nx3 matrix");
}
double (*Translations::data())[3] {
    return reinterpret_cast<double (*)[3]>(array.mutable_data());
}
double const (*Translations::data() const)[3] {
    return reinterpret_cast<double const(*)[3]>(array.data());
}
Symmetries::Symmetries(Rotations &&_rotations, Translations &&_translations)
    : rotations{std::forward<Rotations>(_rotations)},
      translations{std::forward<Translations>(_translations)},
      n_operations{rotations.n_operations} {
    if (rotations.n_operations != translations.n_operations)
        throw SpglibError(
            "Number of Rotations and Translations is inconsistent");
}
Positions::Positions(array_double &&_array)
    : array{std::forward<array_double>(_array)},
      n_atoms(static_cast<int>(array.shape(0))) {
    if (array.ndim() != 2) throw SpglibError("Rotations ndim is not 2");
    if (array.shape(1) != 3) throw SpglibError("Lattice is not a nx3 matrix");
    for (auto i = 0; i < array.size(); ++i) {
        if (!std::isfinite(array.data()[i]))
            throw SpglibError("Positions contain non-finite values");
    }
}
double (*Positions::data())[3] {
    return reinterpret_cast<double (*)[3]>(array.mutable_data());
}
double const (*Positions::data() const)[3] {
    return reinterpret_cast<double const(*)[3]>(array.data());
}
AtomTypes::AtomTypes(array_int &&_array)
    : array(std::forward<array_int>(_array)),
      n_atoms(static_cast<int>(array.shape(0))) {
    if (array.ndim() != 1) throw SpglibError("AtomTypes ndim is not 1");
}
int *AtomTypes::data() { return array.mutable_data(); }
int const *AtomTypes::data() const { return array.data(); }
Magmoms::Magmoms(array_double &&_array)
    : array(std::forward<array_double>(_array)),
      n_atoms(static_cast<int>(array.shape(0))) {
    if (array.ndim() == 1) {
        // Allowed
    } else if (array.ndim() != 2) {
        if (array.shape(1) != 3)
            throw SpglibError("Lattice is not a nx3 matrix");
    } else
        throw SpglibError("Magmoms ndim is not 1 or 2");
}
double *Magmoms::data() { return array.mutable_data(); }
double const *Magmoms::data() const { return array.data(); }
Atoms::Atoms(Positions &&_positions, AtomTypes &&_types)
    : positions{std::forward<Positions>(_positions)},
      types{std::forward<AtomTypes>(_types)},
      n_atoms(positions.n_atoms) {
    if (positions.n_atoms != types.n_atoms)
        throw SpglibError("Number of Positions and Types is inconsistent");
}

spglib::SpglibError::SpglibError(std::string_view _msg) : msg{_msg} {}
char const *spglib::SpglibError::what() const noexcept { return msg.c_str(); }
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

static auto wyckoffs_index_to_letter =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

py::dict Dataset_to_dict(SpglibDataset *dataset) {
    py::dict dict{};
    dict["number"] = dataset->spacegroup_number;
    dict["hall_number"] = dataset->hall_number;
    dict["international"] = dataset->international_symbol;
    dict["hall"] = dataset->hall_symbol;
    dict["choice"] = dataset->choice;
    {
        array_double transformation_matrix({3, 3});
        array_double origin_shift(3);
        for (auto i = 0; i < 3; i++) {
            for (auto j = 0; j < 3; j++)
                transformation_matrix.mutable_at(i, j) =
                    dataset->transformation_matrix[i][j];
            origin_shift.mutable_at(i) = dataset->origin_shift[i];
        }
        dict["transformation_matrix"] = transformation_matrix;
        dict["origin_shift"] = origin_shift;
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
        dict["rotations"] = rotations;
        dict["translations"] = translations;
    }
    {
        py::list wyckoffs(dataset->n_atoms);
        py::list site_symmetry_symbols(dataset->n_atoms);
        array_int crystallographic_orbits(dataset->n_atoms);
        array_int equiv_atoms(dataset->n_atoms);
        array_double primitive_lattice({3, 3});
        array_int mapping_to_primitive(dataset->n_atoms);
        array_double std_lattice({3, 3});
        for (auto ind_atom = 0; ind_atom < dataset->n_atoms; ind_atom++) {
            wyckoffs[ind_atom] =
                wyckoffs_index_to_letter[dataset->wyckoffs[ind_atom]];
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
                // Transposed
                primitive_lattice.mutable_at(i, j) =
                    dataset->primitive_lattice[j][i];
                // Transposed
                std_lattice.mutable_at(i, j) = dataset->std_lattice[j][i];
            }
        dict["wyckoffs"] = wyckoffs;
        dict["site_symmetry_symbols"] = site_symmetry_symbols;
        dict["crystallographic_orbits"] = crystallographic_orbits;
        dict["equivalent_atoms"] = equiv_atoms;
        dict["primitive_lattice"] = primitive_lattice;
        dict["mapping_to_primitive"] = mapping_to_primitive;
        dict["std_lattice"] = std_lattice;
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
        dict["std_types"] = std_types;
        dict["std_positions"] = std_positions;
        dict["std_rotation_matrix"] = std_rotations;
        dict["std_mapping_to_primitive"] = std_mapping_to_primitive;
    }
    dict["pointgroup"] = dataset->pointgroup_symbol;
    return dict;
}

py::dict MagneticDataset_to_dict(SpglibMagneticDataset *dataset,
                                 int tensor_rank) {
    py::dict dict{};
    dict["uni_number"] = dataset->uni_number;
    dict["msg_type"] = dataset->msg_type;
    dict["hall_number"] = dataset->hall_number;
    dict["tensor_rank"] = dataset->tensor_rank;
    dict["n_operations"] = dataset->n_operations;
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
        dict["rotations"] = rotations;
        dict["translations"] = translations;
        dict["time_reversals"] = time_reversal;
    }
    dict["n_atoms"] = dataset->n_atoms;
    {
        array_int equiv_atoms(dataset->n_atoms);
        for (auto ind_atom = 0; ind_atom < dataset->n_atoms; ind_atom++)
            equiv_atoms.mutable_at(ind_atom) =
                dataset->equivalent_atoms[ind_atom];
        dict["equivalent_atoms"] = equiv_atoms;
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
        dict["transformation_matrix"] = transformation_matrix;
        dict["origin_shift"] = origin_shift;
    }
    dict["n_std_atoms"] = dataset->n_std_atoms;
    {
        array_double std_lattice({3, 3});
        for (auto i = 0; i < 3; i++)
            for (auto j = 0; j < 3; j++)
                // Transposed
                std_lattice.mutable_at(i, j) = dataset->std_lattice[j][i];
        dict["std_lattice"] = std_lattice;
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
        dict["std_types"] = std_types;
        dict["std_positions"] = std_positions;
    }
    {
        int n_tensors = dataset->n_std_atoms;
        if (tensor_rank == 1) n_tensors *= 3;
        array_double std_tensors{n_tensors};
        for (auto ind_tensor = 0; ind_tensor < n_tensors; ind_tensor++)
            std_tensors.mutable_at(ind_tensor) =
                dataset->std_tensors[ind_tensor];
        if (tensor_rank == 1) std_tensors = std_tensors.reshape({-1, 3});
        dict["std_tensors"] = std_tensors;
    }
    {
        array_double std_rotations({3, 3});
        array_double primitive_lattice({3, 3});
        for (auto i = 0; i < 3; i++)
            for (auto j = 0; j < 3; j++) {
                std_rotations.mutable_at(i, j) =
                    dataset->std_rotation_matrix[i][j];
                // Transposed
                primitive_lattice.mutable_at(i, j) =
                    dataset->primitive_lattice[j][i];
            }
        dict["std_rotation_matrix"] = std_rotations;
        dict["primitive_lattice"] = primitive_lattice;
    }
    return dict;
}

py::dict SpacegroupType_to_dict(SpglibSpacegroupType &spg_type) {
    py::dict dict{};
    dict["number"] = spg_type.number;
    dict["international_short"] = spg_type.international_short;
    dict["international_full"] = spg_type.international_full;
    dict["international"] = spg_type.international;
    dict["schoenflies"] = spg_type.schoenflies;
    dict["hall_number"] = spg_type.hall_number;
    dict["hall_symbol"] = spg_type.hall_symbol;
    dict["choice"] = spg_type.choice;
    dict["pointgroup_international"] = spg_type.pointgroup_international;
    dict["pointgroup_schoenflies"] = spg_type.pointgroup_schoenflies;
    dict["arithmetic_crystal_class_number"] =
        spg_type.arithmetic_crystal_class_number;
    dict["arithmetic_crystal_class_symbol"] =
        spg_type.arithmetic_crystal_class_symbol;
    return dict;
}

py::dict MagneticSpacegroupType_to_dict(
    SpglibMagneticSpacegroupType &spg_type) {
    py::dict dict{};
    dict["uni_number"] = spg_type.uni_number;
    dict["litvin_number"] = spg_type.litvin_number;
    dict["bns_number"] = spg_type.bns_number;
    dict["og_number"] = spg_type.og_number;
    dict["number"] = spg_type.number;
    dict["type"] = spg_type.type;
    return dict;
}

py::dict spglib::dataset(Lattice const &lattice, Positions const &positions,
                         AtomTypes const &atom_types, py::int_ hall_number,
                         py::float_ symprec, py::float_ angle_tolerance) {
    auto dataset = spgat_get_dataset_with_hall_number(
        lattice.data(), positions.data(), atom_types.data(), atom_types.n_atoms,
        hall_number, symprec, angle_tolerance);
    if (dataset == nullptr) throw Spglib_classic_exception();
    auto array = Dataset_to_dict(dataset);
    spg_free_dataset(dataset);
    return array;
}
py::dict spglib::layer_dataset(Lattice const &lattice,
                               Positions const &positions,
                               AtomTypes const &atom_types,
                               py::int_ aperiodic_dir, py::float_ symprec) {
    auto dataset = spg_get_layer_dataset(lattice.data(), positions.data(),
                                         atom_types.data(), atom_types.n_atoms,
                                         aperiodic_dir, symprec);
    if (dataset == nullptr) throw Spglib_classic_exception();
    auto array = Dataset_to_dict(dataset);
    spg_free_dataset(dataset);
    return array;
}
py::dict spglib::magnetic_dataset(Lattice const &lattice,
                                  Positions const &positions,
                                  AtomTypes const &atom_types,
                                  array_double magmoms, py::int_ tensor_rank,
                                  py::bool_ is_axial, py::float_ symprec,
                                  py::float_ angle_tolerance,
                                  py::float_ mag_symprec) {
    auto dataset = spgms_get_magnetic_dataset(
        lattice.data(), positions.data(), atom_types.data(), magmoms.data(),
        tensor_rank, positions.n_atoms, is_axial * 1, symprec, angle_tolerance,
        mag_symprec);
    if (dataset == nullptr) throw Spglib_classic_exception();
    switch (int(tensor_rank)) {
        case 0:
        case 1:
            break;
        default:
            spg_free_magnetic_dataset(dataset);
            auto msg = std::string("Unexpected tensor_rank value: ");
            msg += tensor_rank;
            throw SpglibError(msg);
    }
    auto array = MagneticDataset_to_dict(dataset, tensor_rank);
    spg_free_magnetic_dataset(dataset);
    return array;
}
py::dict spglib::spacegroup_type(py::int_ hall_number) {
    auto spg_type = spg_get_spacegroup_type(hall_number);
    if (spg_type.number == 0) throw Spglib_classic_exception();
    return SpacegroupType_to_dict(spg_type);
}
py::dict spglib::spacegroup_type_from_symmetry(Rotations const &rotations,
                                               Translations const &translations,
                                               Lattice const &lattice,
                                               py::float_ symprec) {
    auto spg_type = spg_get_spacegroup_type_from_symmetry(
        rotations.data(), translations.data(), rotations.n_operations,
        lattice.data(), symprec);
    if (spg_type.number == 0) throw Spglib_classic_exception();
    return SpacegroupType_to_dict(spg_type);
}
py::dict spglib::magnetic_spacegroup_type(py::int_ uni_number) {
    auto msg_type = spg_get_magnetic_spacegroup_type(uni_number);
    if (msg_type.number == 0) throw Spglib_classic_exception();
    return MagneticSpacegroupType_to_dict(msg_type);
}
py::dict spglib::magnetic_spacegroup_type_from_symmetry(
    Rotations const &rotations, Translations const &translations,
    array_int time_reversals, Lattice const &lattice, py::float_ symprec) {
    auto msg_type = spg_get_magnetic_spacegroup_type_from_symmetry(
        rotations.data(), translations.data(), (int *)time_reversals.data(),
        time_reversals.size(), lattice.data(), symprec);
    if (msg_type.number == 0) throw Spglib_classic_exception();
    return MagneticSpacegroupType_to_dict(msg_type);
}
py::int_ spglib::symmetry_from_database(Rotations &rotations,
                                        Translations &translations,
                                        py::int_ hall_number) {
    if (rotations.n_operations < 192 || translations.n_operations < 192)
        throw Spglib_classic_exception();
    auto val = spg_get_symmetry_from_database(rotations.data(),
                                              translations.data(), hall_number);
    if (val == 0) throw Spglib_classic_exception();
    return val;
}
py::int_ spglib::magnetic_symmetry_from_database(Rotations &rotations,
                                                 Translations &translations,
                                                 array_int time_reversals,
                                                 py::int_ uni_number,
                                                 py::int_ hall_number) {
    if (rotations.n_operations < 384 || translations.n_operations < 384 ||
        time_reversals.shape(0) < 384)
        throw Spglib_classic_exception();
    auto val = spg_get_magnetic_symmetry_from_database(
        rotations.data(), translations.data(), (int *)time_reversals.data(),
        uni_number, hall_number);
    if (val == 0) throw Spglib_classic_exception();
    return val;
}
py::tuple spglib::pointgroup(array_int rotations) {
    char symbol[6];
    array_int transf_matrix({3, 3});
    auto ptg_num =
        spg_get_pointgroup(symbol, (int (*)[3])transf_matrix.mutable_data(),
                           (int (*)[3][3])rotations.data(), rotations.shape(0));
    if (ptg_num == 0) throw Spglib_classic_exception();
    py::list array(3);
    array[0] = symbol;
    array[1] = ptg_num;
    array[2] = transf_matrix;
    return array;
}
py::int_ spglib::standardize_cell(Lattice &lattice, Positions &positions,
                                  array_int atom_types, py::int_ num_atom,
                                  py::int_ to_primative, py::int_ no_idealize,
                                  py::float_ symprec,
                                  py::float_ angle_tolerance) {
    auto val = spgat_standardize_cell(
        lattice.data(), positions.data(), atom_types.mutable_data(), num_atom,
        to_primative, no_idealize, symprec, angle_tolerance);
    if (val == 0) throw Spglib_classic_exception();
    return val;
}
py::int_ spglib::refine_cell(Lattice &lattice, Positions &positions,
                             AtomTypes &atom_types, py::int_ num_atom,
                             py::float_ symprec, py::float_ angle_tolerance) {
    auto val =
        spgat_refine_cell(lattice.data(), positions.data(), atom_types.data(),
                          num_atom, symprec, angle_tolerance);
    if (val > 0)
        // Valid value
        return val;
    throw Spglib_classic_exception();
}
py::int_ spglib::symmetry(Rotations &rotations, Translations &translations,
                          Lattice const &lattice, Positions const &positions,
                          AtomTypes const &atom_types, py::float_ symprec,
                          py::float_ angle_tolerance) {
    auto val = spgat_get_symmetry(rotations.data(), translations.data(),
                                  rotations.n_operations, lattice.data(),
                                  positions.data(), atom_types.data(),
                                  atom_types.n_atoms, symprec, angle_tolerance);
    if (val == 0) throw Spglib_classic_exception();
    return val;
}
py::int_ spglib::symmetry_with_collinear_spin(
    Rotations &rotations, Translations &translations, array_int equiv_atoms,
    Lattice const &lattice, Positions const &positions,
    AtomTypes const &atom_types, array_double magmoms, py::float_ symprec,
    py::float_ angle_tolerance) {
    auto val = spgat_get_symmetry_with_collinear_spin(
        rotations.data(), translations.data(), equiv_atoms.mutable_data(),
        equiv_atoms.size(), lattice.data(), positions.data(), atom_types.data(),
        magmoms.data(), atom_types.n_atoms, symprec, angle_tolerance);
    if (val == 0) throw Spglib_classic_exception();
    return val;
}
py::int_ spglib::symmetry_with_site_tensors(
    Rotations &rotations, Translations &translations, array_int equiv_atoms,
    Lattice &primitive_lattice, array_int spin_flips, Lattice const &lattice,
    Positions const &positions, AtomTypes const &atom_types,
    array_double tensors, py::int_ with_time_reversal, py::int_ is_axial,
    py::float_ symprec, py::float_ angle_tolerance, py::float_ mag_symprec) {
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
    auto val = spgms_get_symmetry_with_site_tensors(
        rotations.data(), translations.data(), equiv_atoms.mutable_data(),
        primitive_lattice.data(), spin_flips_ptr, rotations.n_operations,
        lattice.data(), positions.data(), atom_types.data(), tensors.data(),
        tensor_rank, atom_types.n_atoms, with_time_reversal, is_axial, symprec,
        angle_tolerance, mag_symprec);
    if (val == 0) throw Spglib_classic_exception();
    return val;
}
py::int_ spglib::primitive(Lattice &lattice, Positions &positions,
                           AtomTypes &atom_types, py::float_ symprec,
                           py::float_ angle_tolerance) {
    auto val = spgat_find_primitive(lattice.data(), positions.data(),
                                    atom_types.data(), atom_types.n_atoms,
                                    symprec, angle_tolerance);
    if (val == 0) throw Spglib_classic_exception();
    return val;
}
py::int_ spglib::grid_point_from_address(array_int grid_address,
                                         array_int mesh) {
    // TODO: Throw if input is unexpected
    // Otherwise does not seem to have errors associated.
    // Also this is not generally exposed, maybe get rid of it?
    return spg_get_dense_grid_point_from_address(grid_address.data(),
                                                 mesh.data());
}
py::int_ spglib::ir_reciprocal_mesh(
    array_int grid_address, array_int grid_mapping_table, array_int mesh,
    array_int is_shift, py::int_ is_time_reversal, Lattice const &lattice,
    Positions const &positions, AtomTypes const &atom_types,
    py::float_ symprec) {
    auto val = spg_get_ir_reciprocal_mesh(
        (int (*)[3])grid_address.mutable_data(),
        grid_mapping_table.mutable_data(), mesh.data(), is_shift.data(),
        is_time_reversal, lattice.data(), positions.data(), atom_types.data(),
        atom_types.n_atoms, symprec);
    if (val > 0)
        // Valid value
        return val;
    throw Spglib_classic_exception();
}
py::int_ spglib::ir_reciprocal_mesh(
    array_int grid_address, array_size_t grid_mapping_table, array_int mesh,
    array_int is_shift, py::int_ is_time_reversal, Lattice const &lattice,
    Positions const &positions, AtomTypes const &atom_types,
    py::float_ symprec) {
    auto val = spg_get_dense_ir_reciprocal_mesh(
        (int (*)[3])grid_address.mutable_data(),
        grid_mapping_table.mutable_data(), mesh.data(), is_shift.data(),
        is_time_reversal, lattice.data(), positions.data(), atom_types.data(),
        atom_types.n_atoms, symprec);
    if (val == 0) throw Spglib_classic_exception();
    return val;
}
py::int_ spglib::stabilized_reciprocal_mesh(array_int grid_address,
                                            array_int grid_mapping_table,
                                            array_int mesh, array_int is_shift,
                                            py::int_ is_time_reversal,
                                            Rotations const &rotations,
                                            array_double qpoints) {
    auto val = spg_get_stabilized_reciprocal_mesh(
        (int (*)[3])grid_address.mutable_data(),
        grid_mapping_table.mutable_data(), mesh.data(), is_shift.data(),
        is_time_reversal, rotations.n_operations, rotations.data(),
        qpoints.shape(0), (double (*)[3])qpoints.data());
    if (val > 0)
        // Valid value, did not error
        return val;
    throw Spglib_classic_exception();
}
py::int_ spglib::stabilized_reciprocal_mesh(array_int grid_address,
                                            array_size_t grid_mapping_table,
                                            array_int mesh, array_int is_shift,
                                            py::int_ is_time_reversal,
                                            Rotations const &rotations,
                                            array_double qpoints) {
    auto val = spg_get_dense_stabilized_reciprocal_mesh(
        (int (*)[3])grid_address.mutable_data(),
        grid_mapping_table.mutable_data(), mesh.data(), is_shift.data(),
        is_time_reversal, rotations.n_operations, rotations.data(),
        qpoints.shape(0), (double (*)[3])qpoints.data());
    if (val == 0) throw Spglib_classic_exception();
    return val;
}
void spglib::grid_points_by_rotations(array_size_t rot_grid_points,
                                      array_int address_orig,
                                      Rotations const &rot_reciprocal,
                                      array_int mesh, array_int is_shift) {
    // TODO: Throw if input is unexpected
    // Otherwise does not seem to have errors associated.
    spg_get_dense_grid_points_by_rotations(
        rot_grid_points.mutable_data(), address_orig.data(),
        rot_reciprocal.n_operations, rot_reciprocal.data(), mesh.data(),
        is_shift.data());
}
void spglib::BZ_grid_points_by_rotations(array_size_t rot_grid_points,
                                         array_int address_orig,
                                         Rotations const &rot_reciprocal,
                                         array_int mesh, array_int is_shift,
                                         array_size_t bz_map) {
    // TODO: Throw if input is unexpected
    // Otherwise does not seem to have errors associated.
    spg_get_dense_BZ_grid_points_by_rotations(
        rot_grid_points.mutable_data(), address_orig.data(),
        rot_reciprocal.n_operations, rot_reciprocal.data(), mesh.data(),
        is_shift.data(), bz_map.data());
}
py::int_ spglib::BZ_grid_address(array_int bz_grid_address, array_size_t bz_map,
                                 array_int grid_address, array_int mesh,
                                 Lattice const &reciprocal_lattice,
                                 array_int is_shift) {
    // TODO: Throw if input is unexpected
    // Otherwise does not seem to have errors associated.
    return spg_relocate_dense_BZ_grid_address(
        (int (*)[3])bz_grid_address.mutable_data(), bz_map.mutable_data(),
        (int (*)[3])grid_address.data(), mesh.data(), reciprocal_lattice.data(),
        is_shift.data());
}
py::int_ spglib::delaunay_reduce(Lattice &lattice, py::float_ symprec) {
    auto val = spg_delaunay_reduce(lattice.data(), symprec);
    if (val == 0) throw Spglib_classic_exception();
    return val;
}
py::int_ spglib::niggli_reduce(Lattice &lattice, py::float_ eps) {
    auto val = spg_niggli_reduce(lattice.data(), eps);
    if (val == 0) throw Spglib_classic_exception();
    return val;
}
py::int_ spglib::hall_number_from_symmetry(Rotations const &rotations,
                                           Translations const &translations,
                                           py::float_ symprec) {
    auto val = spg_get_hall_number_from_symmetry(
        rotations.data(), translations.data(), rotations.n_operations, symprec);
    if (val == 0) throw Spglib_classic_exception();
    return val;
}
