#include <gtest/gtest.h>

extern "C" {
#include "spglib.h"
#include "utils.h"
}

TEST(RefineCell, test_spg_refine_cell_BCC) {
    double lattice[3][3] = {{0, 2, 2}, {2, 0, 2}, {2, 2, 0}};

    /* 4 times larger memory space must be prepared. */
    double position[4][3];
    int types[4];

    int num_atom_bravais;
    int num_atom = 1;
    double symprec = 1e-5;

    position[0][0] = 0;
    position[0][1] = 0;
    position[0][2] = 0;
    types[0] = 1;

    /* lattice, position, and types are overwritten. */
    num_atom_bravais =
        spg_refine_cell(lattice, position, types, num_atom, symprec);
    ASSERT_EQ(num_atom_bravais, 4);
    show_cell(lattice, position, types, num_atom_bravais);
}
