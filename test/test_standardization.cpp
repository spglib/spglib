#include <gtest/gtest.h>

extern "C" {
#include "spglib.h"
}

static int sub_spg_standardize_cell(double lattice[3][3], double position[][3],
                                    int types[], const int num_atom,
                                    const double symprec,
                                    const int to_primitive,
                                    const int no_idealize);

TEST(test_standardization, test_spg_standardize_cell_BCC) {
    double lattice[3][3] = {{3.97, 0, 0}, {0, 4.03, 0}, {0, 0, 4.0}};
    double position[][3] = {{0.002, 0, 0}, {0.5, 0.5001, 0.5}};
    int types[] = {1, 1};
    int j, k;
    int num_atom = 2;
    double symprec = 1e-1;

    /* lattice, position, and types are overwritten. */
    printf("*** spg_standardize_cell (BCC unitcell --> primitive) ***:\n");
    printf("------------------------------------------------------\n");
    for (j = 0; j < 2; j++) {
        for (k = 0; k < 2; k++) {
            ASSERT_EQ(sub_spg_standardize_cell(lattice, position, types,
                                               num_atom, symprec, j, k),
                      0);
            printf("------------------------------------------------------\n");
        }
    }
}

TEST(test_standardization, test_spg_standardize_cell_corundum) {
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
    int i, j, k;
    int num_atom = 30;
    double symprec = 1e-5;

    for (i = 0; i < 12; i++) {
        types[i] = 1;
    }
    for (i = 12; i < 30; i++) {
        types[i] = 2;
    }

    /* lattice, position, and types are overwritten. */
    printf("*** spg_standardize_cell (Corundum) ***:\n");
    printf("------------------------------------------------------\n");
    for (j = 0; j < 2; j++) {
        for (k = 0; k < 2; k++) {
            ASSERT_EQ(sub_spg_standardize_cell(lattice, position, types,
                                               num_atom, symprec, j, k),
                      0);
            printf("------------------------------------------------------\n");
        }
    }
}

// ****************************************************************************
// Local functions
// ****************************************************************************

static int sub_spg_standardize_cell(double lattice[3][3], double position[][3],
                                    int types[], const int num_atom,
                                    const double symprec,
                                    const int to_primitive,
                                    const int no_idealize) {
    int i, num_primitive_atom, retval;
    double lat[3][3];
    double(*pos)[3];
    int *typ;

    pos = (double(*)[3])malloc(sizeof(double[3]) * num_atom);
    typ = (int *)malloc(sizeof(int) * num_atom);

    for (i = 0; i < 3; i++) {
        lat[i][0] = lattice[i][0];
        lat[i][1] = lattice[i][1];
        lat[i][2] = lattice[i][2];
    }

    for (i = 0; i < num_atom; i++) {
        pos[i][0] = position[i][0];
        pos[i][1] = position[i][1];
        pos[i][2] = position[i][2];
        typ[i] = types[i];
    }

    /* lattice, position, and types are overwritten. */
    num_primitive_atom = spg_standardize_cell(
        lat, pos, typ, num_atom, to_primitive, no_idealize, symprec);

    if (num_primitive_atom) {
        printf("VASP POSCAR format: ");
        if (to_primitive == 0) {
            printf("to_primitive=0 and ");
        } else {
            printf("to_primitive=1 and ");
        }

        if (no_idealize == 0) {
            printf("no_idealize=0\n");
        } else {
            printf("no_idealize=1\n");
        }
        printf("1.0\n");
        for (i = 0; i < 3; i++) {
            printf("%f %f %f\n", lat[0][i], lat[1][i], lat[2][i]);
        }
        printf("%d\n", num_primitive_atom);
        printf("Direct\n");
        for (i = 0; i < num_primitive_atom; i++) {
            printf("%f %f %f\n", pos[i][0], pos[i][1], pos[i][2]);
        }

        retval = 0;
    } else {
        retval = 1;
    }

    free(typ);
    typ = NULL;
    free(pos);
    pos = NULL;

    return retval;
}
