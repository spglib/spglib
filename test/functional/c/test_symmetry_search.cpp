#include <gtest/gtest.h>

extern "C" {
#include "spglib.h"
#include "utils.h"
}

TEST(SymmetrySearch, test_spg_get_symmetry) {
    double lattice[3][3] = {{4, 0, 0}, {0, 4, 0}, {0, 0, 3}};
    double position[][3] = {
        {0, 0, 0},        {0.5, 0.5, 0.25}, {0.3, 0.3, 0},    {0.7, 0.7, 0},
        {0.2, 0.8, 0.25}, {0.8, 0.2, 0.25}, {0, 0, 0.5},      {0.5, 0.5, 0.75},
        {0.3, 0.3, 0.5},  {0.7, 0.7, 0.5},  {0.2, 0.8, 0.75}, {0.8, 0.2, 0.75}};
    int types[] = {1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2};
    int num_atom = 12;
    int i, j, size, max_size;
    double origin_shift[3] = {0.1, 0.1, 0};

    int (*rotation)[3][3];
    double (*translation)[3];

    max_size = num_atom * 48;
    rotation = (int (*)[3][3])malloc(sizeof(int[3][3]) * max_size);
    translation = (double (*)[3])malloc(sizeof(double[3]) * max_size);

    for (i = 0; i < num_atom; i++) {
        for (j = 0; j < 3; j++) {
            position[i][j] += origin_shift[j];
        }
    }

    printf("*** spg_get_symmetry (Rutile two unit cells) ***:\n");
    size = spg_get_symmetry(rotation, translation, max_size, lattice, position,
                            types, num_atom, 1e-5);
    ASSERT_EQ(size, 32);
    show_symmetry_operations(rotation, translation, size);

    free(rotation);
    rotation = NULL;
    free(translation);
    translation = NULL;
}

TEST(SymmetrySearch, test_spg_get_dataset) {
    double lattice[3][3] = {{4, 0, 0}, {0, 4, 0}, {0, 0, 3}};
    double origin_shift[3] = {0.1, 0.1, 0};
    double position[][3] = {
        {0, 0, 0},        {0.5, 0.5, 0.25}, {0.3, 0.3, 0},    {0.7, 0.7, 0},
        {0.2, 0.8, 0.25}, {0.8, 0.2, 0.25}, {0, 0, 0.5},      {0.5, 0.5, 0.75},
        {0.3, 0.3, 0.5},  {0.7, 0.7, 0.5},  {0.2, 0.8, 0.75}, {0.8, 0.2, 0.75}};
    int types[] = {1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2};
    int num_atom = 12;

    double lattice_2[3][3] = {{3.7332982433264039, -1.8666491216632011, 0},
                              {0, 3.2331311186244847, 0},
                              {0, 0, 6.0979971306362799}};
    double origin_shift_2[3] = {0.1, 0.1, 0};
    double position_2[][3] = {
        {0, 0, 0},
        {1.0 / 3, 2.0 / 3, 0.4126},
        {1.0 / 3, 2.0 / 3, 0.776},
        {2.0 / 3, 1.0 / 3, 0.2542},
    };
    int types_2[] = {1, 2, 3, 3};
    int num_atom_2 = 4;

    ASSERT_EQ(
        show_spg_dataset(lattice, origin_shift, position, num_atom, types), 0);
    ASSERT_EQ(show_spg_dataset(lattice_2, origin_shift_2, position_2,
                               num_atom_2, types_2),
              0);
}

TEST(SymmetrySearch, test_spg_get_multiplicity) {
    double lattice[3][3] = {{4, 0, 0}, {0, 4, 0}, {0, 0, 4}};
    double position[][3] = {{0, 0, 0}, {0.5, 0.5, 0.5}};
    int types[] = {1, 2};
    int num_atom = 2;
    int size;

    size = spg_get_multiplicity(lattice, position, types, num_atom, 1e-5);
    ASSERT_EQ(size, 48);
}
