#pragma once

#include <gtest/gtest.h>

#include <array>
#include <optional>
#include <vector>

extern "C" {
#include "spglib.h"
}

struct DatasetInput {
    struct Atoms {
        std::array<double, 3> position;
        int type_number;
        Atoms(const double position[3], int type_number);
        Atoms(const std::array<double, 3>& position, int type_number);
        Atoms(const std::array<double, 3>&& position, int type_number);
    };
    double lattice[3][3]{};
    std::vector<Atoms> atoms;
    double tolerance{};
    double angle_tolerance{};
    std::optional<int> hall_number;
    DatasetInput(const double lattice[3][3], std::vector<Atoms>&& atoms,
                 double tolerance = 0.0, double angle_tolerance = 0.0);
    DatasetInput(const std::array<std::array<double, 3>, 3>& lattice,
                 std::vector<Atoms>&& atoms, double tolerance = 0.0,
                 double angle_tolerance = 0.0);
    DatasetInput(const std::array<std::array<double, 3>, 3>& lattice,
                 std::vector<Atoms>& atoms, double tolerance = 0.0,
                 double angle_tolerance = 0.0);
    DatasetInput() = default;
    void print();
};

class Dataset : public testing::TestWithParam<DatasetInput> {
   protected:
    void TearDown() override;
    void SetUp() override;
    ~Dataset() override;
    SpglibDataset* dataset = nullptr;
    DatasetInput input;
};
