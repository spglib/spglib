#include <gtest/gtest.h>

extern "C" {
#include "spglib.h"
#include "utils.h"
}

TEST(FindPrimitiveCell, test_spg_find_primitive_BCC) {
    double lattice[3][3] = {{4, 0, 0}, {0, 4, 0}, {0, 0, 4}};
    double position[][3] = {{0, 0, 0}, {0.5, 0.5, 0.5}};
    int types[] = {1, 1};
    int num_atom = 2;
    int num_primitive_atom;
    double symprec = 1e-5;

    /* lattice, position, and types are overwritten. */
    num_primitive_atom =
        spg_find_primitive(lattice, position, types, num_atom, symprec);
    ASSERT_EQ(num_primitive_atom, 1);
    show_cell(lattice, position, types, num_primitive_atom);
}

TEST(FindPrimitiveCell, test_spg_find_primitive_corundum) {
    double lattice[3][3] = {{4.8076344022756095, -2.4038172011378047, 0},
                            {0, 4.1635335244786962, 0},
                            {0, 0, 13.1172699198127543}};
    double position[][3] = {
        {0.0000000000000000, 0.0000000000000000, 0.3521850942289043},
        {0.6666666666666643, 0.3333333333333357, 0.6855184275622400},
        {0.3333333333333357, 0.6666666666666643, 0.0188517608955686},
        {0.0000000000000000, 0.0000000000000000, 0.6478149057711028},
        {0.6666666666666643, 0.3333333333333357, 0.9811482391044314},
        {0.3333333333333357, 0.6666666666666643, 0.3144815724377600},
        {0.0000000000000000, 0.0000000000000000, 0.1478149057710957},
        {0.6666666666666643, 0.3333333333333357, 0.4811482391044314},
        {0.3333333333333357, 0.6666666666666643, 0.8144815724377600},
        {0.0000000000000000, 0.0000000000000000, 0.8521850942288972},
        {0.6666666666666643, 0.3333333333333357, 0.1855184275622400},
        {0.3333333333333357, 0.6666666666666643, 0.5188517608955686},
        {0.3061673906454899, 0.0000000000000000, 0.2500000000000000},
        {0.9728340573121541, 0.3333333333333357, 0.5833333333333357},
        {0.6395007239788255, 0.6666666666666643, 0.9166666666666643},
        {0.6938326093545102, 0.0000000000000000, 0.7500000000000000},
        {0.3604992760211744, 0.3333333333333357, 0.0833333333333357},
        {0.0271659426878458, 0.6666666666666643, 0.4166666666666643},
        {0.0000000000000000, 0.3061673906454899, 0.2500000000000000},
        {0.6666666666666643, 0.6395007239788255, 0.5833333333333357},
        {0.3333333333333357, 0.9728340573121541, 0.9166666666666643},
        {0.0000000000000000, 0.6938326093545102, 0.7500000000000000},
        {0.6666666666666643, 0.0271659426878458, 0.0833333333333357},
        {0.3333333333333357, 0.3604992760211744, 0.4166666666666643},
        {0.6938326093545102, 0.6938326093545102, 0.2500000000000000},
        {0.3604992760211744, 0.0271659426878458, 0.5833333333333357},
        {0.0271659426878458, 0.3604992760211744, 0.9166666666666643},
        {0.3061673906454899, 0.3061673906454899, 0.7500000000000000},
        {0.9728340573121541, 0.6395007239788255, 0.0833333333333357},
        {0.6395007239788255, 0.9728340573121541, 0.4166666666666643},
    };
    int types[30];
    int i, num_primitive_atom;
    int num_atom = 30;
    double symprec = 1e-5;

    for (i = 0; i < 12; i++) {
        types[i] = 1;
    }
    for (i = 12; i < 30; i++) {
        types[i] = 2;
    }

    /* lattice, position, and types are overwritten. */
    num_primitive_atom =
        spg_find_primitive(lattice, position, types, num_atom, symprec);
    ASSERT_EQ(num_primitive_atom, 10);
    show_cell(lattice, position, types, num_primitive_atom);
}

TEST(FindPrimitiveCell, test_spg_refine_cell_BCC) {
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
