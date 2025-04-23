#include <gtest/gtest.h>

extern "C" {
#include "spglib.h"
}

TEST(Kpoints, test_spg_get_ir_reciprocal_mesh) {
    double lattice[3][3] = {{4, 0, 0}, {0, 4, 0}, {0, 0, 3}};
    double position[][3] = {
        {0, 0, 0},     {0.5, 0.5, 0.5}, {0.3, 0.3, 0},
        {0.7, 0.7, 0}, {0.2, 0.8, 0.5}, {0.8, 0.2, 0.5},
    };
    int num_ir;
    int types[] = {1, 1, 2, 2, 2, 2};
    int num_atom = 6;
    int m = 40;
    int mesh[3];
    int is_shift[] = {1, 1, 1};
    int (*grid_address)[3];
    int *grid_mapping_table;

    mesh[0] = m;
    mesh[1] = m;
    mesh[2] = m;
    grid_address = (int (*)[3])malloc(sizeof(int[3]) * m * m * m);
    grid_mapping_table = (int *)malloc(sizeof(int) * m * m * m);

    printf("*** spg_get_ir_reciprocal_mesh of Rutile structure ***:\n");

    num_ir = spg_get_ir_reciprocal_mesh(grid_address, grid_mapping_table, mesh,
                                        is_shift, 1, lattice, position, types,
                                        num_atom, 1e-5);
    ASSERT_EQ(num_ir, 4200);

    free(grid_address);
    grid_address = NULL;
    free(grid_mapping_table);
    grid_mapping_table = NULL;
}

TEST(Kpoints, test_spg_get_stabilized_reciprocal_mesh) {
    SpglibDataset *dataset;
    double lattice[3][3] = {{4, 0, 0}, {0, 4, 0}, {0, 0, 3}};
    double position[][3] = {
        {0, 0, 0},     {0.5, 0.5, 0.5}, {0.3, 0.3, 0},
        {0.7, 0.7, 0}, {0.2, 0.8, 0.5}, {0.8, 0.2, 0.5},
    };
    int num_ir;
    int types[] = {1, 1, 2, 2, 2, 2};
    int num_atom = 6;
    int m = 40;
    int mesh[3];
    int is_shift[] = {1, 1, 1};
    int (*grid_address)[3];
    int *grid_mapping_table;
    double q[] = {0, 0.5, 0.5};

    mesh[0] = m;
    mesh[1] = m;
    mesh[2] = m;

    /* Memory spaces have to be allocated to pointers */
    /* to avoid Invalid read/write error by valgrind. */
    grid_address = (int (*)[3])malloc(sizeof(int[3]) * m * m * m);
    grid_mapping_table = (int *)malloc(sizeof(int) * m * m * m);

    dataset = spg_get_dataset(lattice, position, types, num_atom, 1e-5);
    ASSERT_TRUE(dataset != NULL);

    printf("*** spg_get_stabilized_reciprocal_mesh of Rutile structure ***:\n");

    num_ir = spg_get_stabilized_reciprocal_mesh(
        grid_address, grid_mapping_table, mesh, is_shift, 1,
        dataset->n_operations, dataset->rotations, 1, (double (*)[3])q);
    ASSERT_EQ(num_ir, 8000);

    spg_free_dataset(dataset);
    dataset = NULL;
    free(grid_address);
    grid_address = NULL;
    free(grid_mapping_table);
    grid_mapping_table = NULL;
}

TEST(Kpoints, test_spg_relocate_BZ_grid_address) {
    double rec_lattice[3][3] = {{-0.17573761, 0.17573761, 0.17573761},
                                {0.17573761, -0.17573761, 0.17573761},
                                {0.17573761, 0.17573761, -0.17573761}};
    int rotations[][3][3] = {{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}};

    int (*bz_grid_address)[3], (*grid_address)[3];
    int *grid_mapping_table, *bz_map;

    int num_ir, num_q;
    int m = 40;
    int mesh[3];
    int is_shift[] = {0, 0, 0};
    double q[] = {0, 0, 0};

    mesh[0] = m;
    mesh[1] = m;
    mesh[2] = m;

    /* Memory spaces have to be allocated to pointers */
    /* to avoid Invalid read/write error by valgrind. */
    bz_grid_address =
        (int (*)[3])malloc(sizeof(int[3]) * (m + 1) * (m + 1) * (m + 1));
    bz_map = (int *)malloc(sizeof(int) * m * m * m * 8);
    grid_address = (int (*)[3])malloc(sizeof(int[3]) * m * m * m);
    grid_mapping_table = (int *)malloc(sizeof(int) * m * m * m);

    num_ir = spg_get_stabilized_reciprocal_mesh(
        grid_address, grid_mapping_table, mesh, is_shift, 1, 1, rotations, 1,
        (double (*)[3])q);
    ASSERT_TRUE(num_ir > 0);

    printf("*** spg_relocate_BZ_grid_address of NaCl structure ***:\n");

    num_q = spg_relocate_BZ_grid_address(bz_grid_address, bz_map, grid_address,
                                         mesh, rec_lattice, is_shift);
    // printf("Number of k-points of NaCl Brillouin zone\n");
    // printf("with Gamma-centered 40x40x40 Monkhorst-Pack mesh is 65861.\n");
    ASSERT_EQ(num_q, 65861);

    free(bz_grid_address);
    bz_grid_address = NULL;
    free(bz_map);
    bz_map = NULL;
    free(grid_address);
    grid_address = NULL;
    free(grid_mapping_table);
    grid_mapping_table = NULL;
}

TEST(Kpoints, test_spg_relocate_dense_BZ_grid_address) {
    double rec_lattice[3][3] = {{-0.17573761, 0.17573761, 0.17573761},
                                {0.17573761, -0.17573761, 0.17573761},
                                {0.17573761, 0.17573761, -0.17573761}};
    int rotations[][3][3] = {{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}};

    int (*bz_grid_address)[3], (*grid_address)[3];
    size_t *grid_mapping_table, *bz_map;

    size_t num_ir, num_q;
    int m = 40;
    int mesh[3];
    int is_shift[] = {0, 0, 0};
    double q[] = {0, 0, 0};

    mesh[0] = m;
    mesh[1] = m;
    mesh[2] = m;

    /* Memory spaces have to be allocated to pointers */
    /* to avoid Invalid read/write error by valgrind. */
    bz_grid_address =
        (int (*)[3])malloc(sizeof(int[3]) * (m + 1) * (m + 1) * (m + 1));
    bz_map = (size_t *)malloc(sizeof(size_t) * m * m * m * 8);
    grid_address = (int (*)[3])malloc(sizeof(int[3]) * m * m * m);
    grid_mapping_table = (size_t *)malloc(sizeof(size_t) * m * m * m);

    num_ir = spg_get_dense_stabilized_reciprocal_mesh(
        grid_address, grid_mapping_table, mesh, is_shift, 1, 1, rotations, 1,
        (double (*)[3])q);
    ASSERT_TRUE(num_ir > 0);

    printf("*** spg_relocate_dense_BZ_grid_address of NaCl structure ***:\n");

    num_q = spg_relocate_dense_BZ_grid_address(
        bz_grid_address, bz_map, grid_address, mesh, rec_lattice, is_shift);

    // printf("Number of k-points of NaCl Brillouin zone\n");
    // printf("with Gamma-centered 40x40x40 Monkhorst-Pack mesh\n");
    ASSERT_EQ(num_q, 65861);

    free(bz_grid_address);
    bz_grid_address = NULL;
    free(bz_map);
    bz_map = NULL;
    free(grid_address);
    grid_address = NULL;
    free(grid_mapping_table);
    grid_mapping_table = NULL;
}
