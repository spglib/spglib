#include <gtest/gtest.h>

extern "C" {
#include "cell.h"
#include "symmetry.h"
#include "utils.h"
}

TEST(Symmetry, test_get_lattice_symmetry_layer) {
    Cell *cell;
    Symmetry *symmetry;

    cell = nullptr;
    symmetry = nullptr;

    int const size = 1;
    double lattice[3][3] = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1},
    };
    double positions[1][3] = {{0, 0, 0}};
    int const types[1] = {0};
    double const symprec = 1e-5;
    double const angle_tolerance = -1;

    cell = cel_alloc_cell(size, NOSPIN);
    EXPECT_EQ(spg_get_error_code(), SPGLIB_SUCCESS);
    ASSERT_NE(cell, nullptr);
    for (int aperiodic_axis = 0; aperiodic_axis < 3; aperiodic_axis++) {
        cel_set_layer_cell(cell, lattice, positions, types, aperiodic_axis);

        // Bravais group of the two-dimensional lattice is 4/mmm
        symmetry = sym_get_operation(cell, symprec, angle_tolerance);
        EXPECT_EQ(symmetry->size, 16);

        sym_free_symmetry(symmetry);
        symmetry = nullptr;
    }

    cel_free_cell(cell);
    cell = nullptr;
}
