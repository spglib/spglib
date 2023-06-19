#include <gtest/gtest.h>

extern "C" {
#include "spglib.h"
}

TEST(SpacegroupTypeSearch, test_spg_get_international) {
    double lattice[3][3] = {{4, 0, 0}, {0, 4, 0}, {0, 0, 3}};
    double position[][3] = {
        {0, 0, 0},     {0.5, 0.5, 0.5}, {0.3, 0.3, 0},
        {0.7, 0.7, 0}, {0.2, 0.8, 0.5}, {0.8, 0.2, 0.5},
    };
    int types[] = {1, 1, 2, 2, 2, 2};
    int num_spg;
    int num_atom = 6;
    char symbol[21];
    char symbol_expect[] = "P4_2/mnm";

    num_spg =
        spg_get_international(symbol, lattice, position, types, num_atom, 1e-5);
    ASSERT_EQ(num_spg, 136);
    ASSERT_STREQ(symbol, symbol_expect);
}

TEST(SpacegroupTypeSearch, test_spg_get_schoenflies) {
    double lattice[3][3] = {{4, 0, 0}, {0, 4, 0}, {0, 0, 3}};
    double position[][3] = {
        {0, 0, 0},     {0.5, 0.5, 0.5}, {0.3, 0.3, 0},
        {0.7, 0.7, 0}, {0.2, 0.8, 0.5}, {0.8, 0.2, 0.5},
    };
    int types[] = {1, 1, 2, 2, 2, 2};
    int num_atom = 6;
    int num_spg;
    char symbol[7];
    char symbol_expect[] = "D4h^14";

    num_spg =
        spg_get_schoenflies(symbol, lattice, position, types, num_atom, 1e-5);
    ASSERT_EQ(num_spg, 136);
    ASSERT_STREQ(symbol, symbol_expect);
}
