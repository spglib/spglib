#include "c_fixtures.hpp"

#include <gtest/gtest.h>

#include <iostream>

void Dataset::SetUp() {
    input = GetParam();
    // First print the parameters
    input.print();

    // Have to copy arrays to get the appropriate C type data
    std::vector<double[3]> positions(input.atoms.size());
    std::vector<int> types(input.atoms.size());
    // Can't figure how to push_back vector of native array
    auto i = 0;
    for (auto& atom : input.atoms) {
        for (auto j = 0; j < 3; j++) {
            positions[i][j] = atom.position[j];
        }
        types[i] = atom.type_number;
        i++;
    }
    dataset = spg_get_dataset(input.lattice, positions.data(), types.data(),
                              (int)input.atoms.size(), input.tolerance);
    ASSERT_EQ(spg_get_error_code(), SPGLIB_SUCCESS);
}
void Dataset::TearDown() {
    if (dataset != nullptr) {
        spg_free_dataset(dataset);
        dataset = nullptr;
    }
}
Dataset::~Dataset() {
    if (dataset != nullptr) {
        spg_free_dataset(dataset);
        dataset = nullptr;
    }
}

DatasetInput::Atoms::Atoms(const double position[3], int type_number)
    : position{}, type_number{type_number} {
    for (auto i = 0; i < 3; i++) {
        this->position[i] = position[i];
    }
}
DatasetInput::Atoms::Atoms(const std::array<double, 3>& position,
                           int type_number)
    : position{position}, type_number{type_number} {}
DatasetInput::Atoms::Atoms(const std::array<double, 3>&& position,
                           int type_number)
    : position{position}, type_number{type_number} {}
DatasetInput::DatasetInput(const double lattice[3][3],
                           std::vector<Atoms>&& atoms, double tolerance,
                           double angle_tolerance)
    : lattice{},
      atoms{atoms},
      tolerance{tolerance},
      angle_tolerance{angle_tolerance} {
    for (auto i = 0; i < 3; i++) {
        for (auto j = 0; j < 3; j++) {
            this->lattice[i][j] = lattice[i][j];
        }
    }
}
DatasetInput::DatasetInput(const std::array<std::array<double, 3>, 3>& lattice,
                           std::vector<Atoms>&& atoms, double tolerance,
                           double angle_tolerance)
    : lattice{},
      atoms{atoms},
      tolerance{tolerance},
      angle_tolerance{angle_tolerance} {
    for (auto i = 0; i < 3; i++) {
        for (auto j = 0; j < 3; j++) {
            this->lattice[i][j] = lattice[i][j];
        }
    }
}
DatasetInput::DatasetInput(const std::array<std::array<double, 3>, 3>& lattice,
                           std::vector<Atoms>& atoms, double tolerance,
                           double angle_tolerance)
    : lattice{},
      atoms{atoms},
      tolerance{tolerance},
      angle_tolerance{angle_tolerance} {
    for (auto i = 0; i < 3; i++) {
        for (auto j = 0; j < 3; j++) {
            this->lattice[i][j] = lattice[i][j];
        }
    }
}
void DatasetInput::print() {
    std::cout << "Lattice parameter:" << std::endl;

    for (auto i = 0; i < 3; i++) {
        std::cout << "[ " << lattice[0][i] << ", " << lattice[1][i] << ", "
                  << lattice[2][i] << " ]" << std::endl;
    }
    std::cout << "Number of atoms: " << atoms.size() << std::endl;
    std::cout << "Tolerance: " << tolerance << std::endl;
    std::cout << "Angle tolerance: " << angle_tolerance << std::endl;
    std::cout << "Hall number: "
              << (hall_number.has_value() ? std::to_string(hall_number.value())
                                          : "Not present")
              << std::endl;
    std::cout << "Atomic positions:\n";
    for (auto& atom : atoms) {
        std::cout << "[ " << atom.position[0] << ", " << atom.position[1]
                  << ", " << atom.position[2]
                  << " ] type_number = " << atom.type_number << std::endl;
    }
}
