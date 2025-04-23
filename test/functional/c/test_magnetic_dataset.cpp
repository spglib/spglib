#include <gtest/gtest.h>

extern "C" {
#include "spglib.h"
#include "utils.h"
}

TEST(MagneticDataset, test_spg_get_magnetic_dataset) {
    /* Rutile structure (P4_2/mnm) */
    /* Generators: -y+1/2,x+1/2,z+1/2; -x+1/2,y+1/2,-z+1/2; -x,-y,-z */
    double lattice[3][3] = {{5, 0, 0}, {0, 5, 0}, {0, 0, 3}};
    double position[][3] = {
        /* Ti (2a) */
        {0, 0, 0},
        {0.5, 0.5, 0.5},
        /* O (4f) */
        {0.3, 0.3, 0},
        {0.7, 0.7, 0},
        {0.2, 0.8, 0.5},
        {0.8, 0.2, 0.5},
    };
    int types[] = {1, 1, 2, 2, 2, 2};
    double spins[6];
    int num_atom = 6;
    SpglibMagneticDataset *dataset;

    /* Type-I, 136.495: -P 4n 2n */
    {
        printf("*** spg_get_magnetic_dataset (type-I, ferro) ***:\n");
        spins[0] = 0.3;
        spins[1] = 0.3;
        spins[2] = 0;
        spins[3] = 0;
        spins[4] = 0;
        spins[5] = 0;
        dataset = spg_get_magnetic_dataset(lattice, position, types, spins,
                                           0 /* tensor_rank */, num_atom,
                                           0 /* is_axial */, 1e-5);
        EXPECT_EQ(dataset->msg_type, 1);
        EXPECT_EQ(dataset->uni_number, 1155);
        EXPECT_EQ(spg_get_error_code(), SpglibError::SPGLIB_SUCCESS);
        show_spg_magnetic_dataset(dataset);

        spg_free_magnetic_dataset(dataset);
    }

    /* Type-II, "136.496": -P 4n 2n 1' */
    {
        printf("*** spg_get_magnetic_dataset (type-II, gray) ***:\n");
        spins[0] = 0;
        spins[1] = 0;
        spins[2] = 0;
        spins[3] = 0;
        spins[4] = 0;
        spins[5] = 0;
        dataset = spg_get_magnetic_dataset(lattice, position, types, spins,
                                           0 /* tensor_rank */, num_atom,
                                           0 /* is_axial */, 1e-5);
        EXPECT_EQ(dataset->msg_type, 2);
        EXPECT_EQ(dataset->uni_number, 1156);
        EXPECT_EQ(spg_get_error_code(), SpglibError::SPGLIB_SUCCESS);
        show_spg_magnetic_dataset(dataset);

        spg_free_magnetic_dataset(dataset);
    }

    /* Type-III, "136.498": -P 4n' 2n' */
    {
        printf("*** spg_get_magnetic_dataset (type-III, antiferro) ***:\n");
        spins[0] = 0.7;
        spins[1] = -0.7;
        spins[2] = 0;
        spins[3] = 0;
        spins[4] = 0;
        spins[5] = 0;
        dataset = spg_get_magnetic_dataset(lattice, position, types, spins,
                                           0 /* tensor_rank */, num_atom,
                                           0 /* is_axial */, 1e-5);
        EXPECT_EQ(dataset->msg_type, 3);
        EXPECT_EQ(dataset->uni_number, 1158);
        EXPECT_EQ(spg_get_error_code(), SpglibError::SPGLIB_SUCCESS);
        show_spg_magnetic_dataset(dataset);

        spg_free_magnetic_dataset(dataset);
    }
}

TEST(MagneticDataset, test_spg_get_magnetic_dataset_type4) {
    /* double Rutile structure (P4_2/mnm) */
    double lattice[3][3] = {{5, 0, 0}, {0, 5, 0}, {0, 0, 6}};
    double position[][3] = {
        /* Ti (2a) */
        {0, 0, 0},
        {0.500001, 0.5, 0.25}, /* Test with small displacement */
        /* O (4f) */
        {0.3, 0.3, 0},
        {0.7, 0.7, 0},
        {0.2, 0.8, 0.25},
        {0.8, 0.2, 0.25},
        /* Ti (2a) */
        {0, 0, 0.5},
        {0.5, 0.5, 0.75},
        /* O (4f) */
        {0.3, 0.3, 0.5},
        {0.7, 0.7, 0.5},
        {0.2, 0.8, 0.75},
        {0.8, 0.2, 0.75},
    };
    int types[] = {1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2};
    double spins[] = {0.300001, 0.299999, 0, 0, 0, 0, -0.3, -0.3, 0, 0, 0, 0};
    int num_atom = 12;
    SpglibMagneticDataset *dataset;

    /* "136.504": -P 4n 2n 1c' */
    dataset = spg_get_magnetic_dataset(lattice, position, types, spins,
                                       0 /* tensor_rank */, num_atom,
                                       0 /* is_axial */, 1e-5);
    EXPECT_EQ(dataset->msg_type, 4);
    EXPECT_EQ(dataset->uni_number, 932);
    EXPECT_EQ(spg_get_error_code(), SpglibError::SPGLIB_SUCCESS);
    show_spg_magnetic_dataset(dataset);

    spg_free_magnetic_dataset(dataset);
}

TEST(MagneticDataset, test_spg_get_magnetic_dataset_high_mag_symprec) {
    // https://github.com/spglib/spglib/issues/348
    double lattice[3][3] = {{4, 0, 0}, {0, 4, 0}, {0, 0, 18}};
    double positions[][3] = {
        {0, 0, 0.57},
        {0.5, 0.5, 0.43},
        {0.5, 1, 0.5},
        {0, 0.5, 0.5},
    };
    int types[] = {1, 1, 2, 2};
    double tensors[] = {0, 0, -1, 1};
    int num_atoms = 4;
    double symprec = 3.0;

    auto dataset = spg_get_magnetic_dataset(lattice, positions, types, tensors,
                                            0, num_atoms, 0, symprec);
    // MSG identification is failed with the too high mag_symprec
    EXPECT_EQ(dataset, nullptr);

    auto error = spg_get_error_code();
    EXPECT_EQ(error, SpglibError::SPGERR_ATOMS_TOO_CLOSE);
}

TEST(MagneticDataset, test_spgms_get_magnetic_dataset_high_mag_symprec) {
    // https://github.com/spglib/spglib/issues/249
    double lattice[3][3] = {
        {0.00000000, -5.00000000, -2.50000000},
        {0.00000000, 0.00000000, 4.33012702},
        {-4.05000000, 0.00000000, 0.00000000},
    };
    double positions[][3] = {
        {0.50000000, 0.33333333, 0.33333333},
        {0.50000000, 0.66666667, 0.66666667},
        {0.00000000, 0.00000000, 0.00000000},
        {0.00000000, 0.00000000, 0.50000000},
        {0.00000000, 0.50000000, 0.50000000},
        {0.00000000, 0.50000000, 0.00000000},
    };
    int types[] = {1, 1, 1, 2, 2, 2};
    double tensors[] = {
        -0.00200000, 0.00200000,  1.90000000,  0.00200000,  0.00000000,
        1.91100000,  -0.00100000, 0.00200000,  -2.23300000, -0.00000000,
        -0.00000000, -0.06000000, 0.00000000,  0.00000000,  -0.03200000,
        0.00000000,  -0.00000000, -0.02900000,
    };
    int num_atoms = 6;
    // 96  = 48 * 2
    // = (max number of order of point group) * (spin degrees of freedom)
    int max_size = num_atoms * 96;
    double symprec = 1e-5;
    double mag_symprec = 1e-1;  // with high mag_symprec

    int size;
    int equivalent_atoms[6];
    double primitive_lattice[3][3];
    int (*rotations)[3][3];
    double (*translations)[3];
    int *spin_flips;
    int *time_reversals;

    rotations = (int (*)[3][3])malloc(sizeof(int[3][3]) * max_size);
    translations = (double (*)[3])malloc(sizeof(double[3]) * max_size);
    spin_flips = (int *)malloc(sizeof(int *) * max_size);
    time_reversals = (int *)malloc(sizeof(int *) * max_size);

    size = spgms_get_symmetry_with_site_tensors(
        rotations, translations, equivalent_atoms, primitive_lattice,
        spin_flips, max_size, lattice, positions, types, tensors,
        1 /* tensor_rank */, num_atoms, 1 /* with_time_reversal */,
        1 /* is_axial */, symprec, -1 /* angle_tolerance */, mag_symprec);
    // spgms_get_symmetry_with_site_tensors should return one or more symmetry
    // operations
    ASSERT_TRUE(size > 0);

    SpglibMagneticDataset *dataset;
    dataset =
        spgms_get_magnetic_dataset(lattice, positions, types, tensors, 1,
                                   num_atoms, 1, symprec, -1, mag_symprec);
    // MSG identification is failed with the too high mag_symprec
    ASSERT_TRUE(dataset == NULL);

    free(rotations);
    free(translations);
    free(spin_flips);
    free(time_reversals);
    // No need to free SpglibMagneticDataset here
}

TEST(MagneticDataset, test_spg_get_magnetic_dataset_non_collinear) {
    double lattice[3][3] = {{10, 0, 0}, {0, 10, 0}, {0, 0, 10}};
    double position[][3] = {
        {0, 0, 0},
    };
    int types[] = {1};
    double spins[3] = {1, 0, 0};
    int num_atom = 1;
    SpglibMagneticDataset *dataset;
    dataset = spg_get_magnetic_dataset(lattice, position, types, spins, 1,
                                       num_atom, 1, 1e-5);

    EXPECT_NE(dataset, nullptr);
    EXPECT_EQ(dataset->n_operations, 16);
    EXPECT_EQ(spg_get_error_code(), SpglibError::SPGLIB_SUCCESS);
    if (HasFailure()) show_spg_magnetic_dataset(dataset);
    spg_free_magnetic_dataset(dataset);
}

TEST(MagneticDataset, test_with_broken_symmetry) {
    // https://github.com/spglib/spglib/issues/194
    // Part of "mp-806965" in the Materials Project database
    double lattice[][3] = {
        // column-wise!
        {5.24191, -0.003459, -2.618402},
        {0, 5.600534, -1.87898},
        {0, 0, 11.148141},
    };
    double positions[][3] = {
        {0.829407, 0.834674, 0.662821},
        {0.665078, 0.665214, 0.316142},
        {0.001174, 0.996383, 0.002809},
        {0.181127, 0.169218, 0.349259},
    };
    int types[] = {0, 0, 0, 0};
    double tensors[] = {1.927, 1.947, 1.928, 1.949};
    int num_atoms = 4;
    // 96  = 48 * 2
    // = (max number of order of point group) * (spin degrees of freedom)
    int max_size = num_atoms * 96;

    double symprec = 0.1;  // with very high symprec
    double mag_symprec = symprec;

    int i, size;
    int equivalent_atoms[4];
    double primitive_lattice[3][3];
    int (*rotations)[3][3];
    double (*translations)[3];
    int *spin_flips;
    int *time_reversals;

    rotations = (int (*)[3][3])malloc(sizeof(int[3][3]) * max_size);
    translations = (double (*)[3])malloc(sizeof(double[3]) * max_size);
    spin_flips = (int *)malloc(sizeof(int *) * max_size);
    time_reversals = (int *)malloc(sizeof(int *) * max_size);

    // Check magnetic symmetry search
    // spg_get_symmetry returns four operations, but
    // spgms_get_symmetry_with_site_tensors only returns three of them. This is
    // due to too high symprec: detected operations in `sym_get_operation`
    // follow `symprec`, but refined operations in
    // `ref_get_exact_structure_and_symmetry` does not.
    size = spgms_get_symmetry_with_site_tensors(
        rotations, translations, equivalent_atoms, primitive_lattice,
        spin_flips, max_size, lattice, positions, types, tensors,
        0 /* tensor_rank */, num_atoms, 1 /* with_time_reversal */,
        0 /* is_axial */, symprec, -1 /* angle_tolerance */, mag_symprec);

    for (i = 0; i < size; i++) {
        time_reversals[i] = (1 - spin_flips[i]) / 2;
    }
    show_magnetic_symmetry_operations(rotations, translations, time_reversals,
                                      size);
    ASSERT_TRUE(size >= 1);

    // Check magnetic dataset construction
    // Since detected magnetic symmetry operations do not form a group due to
    // high symprec, we fail to get magnetic dataset for now.
    SpglibMagneticDataset *dataset;
    dataset = spg_get_magnetic_dataset(lattice, positions, types, tensors,
                                       0 /* tensor_rank */, num_atoms,
                                       0 /* is_axial */, symprec);
    ASSERT_TRUE(dataset == NULL);

    free(rotations);
    free(translations);
    free(spin_flips);
    free(time_reversals);
    // No need to free SpglibMagneticDataset here
}

TEST(MagneticDataset, test_with_slightly_distorted_positions) {
    // https://github.com/spglib/spglib/issues/381
    double lattice[3][3] = {
        {4.33379984, -2.16689992, 0.},
        {0., 3.75318076, 2.50211874},
        {0., 0., 27.27326917},
    };
    double positions[][3] = {
        {0, 0, 0},
        {0.5 + 1e-5, 0, 0.5},
    };
    int types[] = {0, 0};
    double tensors[] = {1, -1};
    int num_atoms = 2;
    double symprec = 1e-4;

    SpglibMagneticDataset *dataset;
    dataset = spg_get_magnetic_dataset(lattice, positions, types, tensors,
                                       0 /* tensor_rank */, num_atoms,
                                       0 /* is_axial */, symprec);
    EXPECT_EQ(dataset->uni_number,
              1332);  // should be the same as no distortion case
    EXPECT_EQ(spg_get_error_code(), SpglibError::SPGLIB_SUCCESS);

    spg_free_magnetic_dataset(dataset);
}

TEST(MagneticDataset, test_failure_with_slightly_distorted_positions) {
    // This structure was randomly generated
    double lattice[3][3] = {
        {4.79573126, 4.79573126, -1.60862362},
        {-1.38874873, 1.38874873, 0.},
        {0., 0., 5.31736928},
    };
    double positions[][3] = {
        {0.51669908, 0.51669908, 0.22675739},
        {0.5166947, 0.5166947, 0.72675256},
        {0.19685353, 0.19685353, 0.56683556},
        {0.83653907, 0.83653907, 0.88667445},
        {0.83653365, 0.83653365, 0.38666685},
        {0.19685074, 0.19685074, 0.0668282},
    };
    int types[] = {0, 0, 1, 1, 1, 1};
    double tensors[] = {0, 0, 0, 0, 0, 0};
    int num_atoms = 6;
    // prm_get_primitive_symmetry seems to change behavior with symprec=1e-4 and
    // 1e-5 for this distorted structure
    double symprec = 1e-4;

    SpglibMagneticDataset *dataset;
    dataset = spg_get_magnetic_dataset(lattice, positions, types, tensors,
                                       0 /* tensor_rank */, num_atoms,
                                       0 /* is_axial */, symprec);
    EXPECT_EQ(spg_get_error_code(),
              SpglibError::SPGERR_SPACEGROUP_SEARCH_FAILED);

    if (dataset) spg_free_magnetic_dataset(dataset);
}

TEST(MagneticDataset, test_with_right_handed_magnetic_lattice) {
    // In some cases, a detected magnetic lattice would be right-handed.
    // Thus, we need to take absolute value of the determinant to get the size
    // of the magnetic cell.
    double lattice[3][3] = {
        {-0.00000003, 5.55312336, -2.77656167},
        {6.41219485, -3.20609746, -1.60304871},
        {4.53409959, 4.53409960, 2.26704980},
    };
    double positions[][3] = {
        {0.00000001, 0.00000013, 0.99999984},
        {0.24999988, 0.25000009, 0.00000017},
        {0.25000002, 0.00000008, 0.49999990},
        {0.00000007, 0.25000009, 0.49999962},
        {0.99999989, 0.49999996, 0.00000008},
        {0.24999994, 0.74999990, 0.00000034},
        {0.24999999, 0.50000000, 0.49999993},
        {0.99999997, 0.74999989, 0.50000032},
        {0.49999999, 0.99999997, 0.00000011},
        {0.74999993, 0.25000009, 0.99999994},
        {0.74999993, 0.99999991, 0.50000025},
        {0.50000002, 0.24999999, 0.49999999},
        {0.50000013, 0.49999997, 0.99999990},
        {0.75000014, 0.74999993, 0.99999988},
        {0.75000004, 0.50000006, 0.49999988},
        {0.50000006, 0.74999994, 0.49999986},
    };
    int types[] = {1, 4, 4, 4, 1, 4, 4, 4, 1, 4, 4, 4, 1, 4, 4, 4};
    double tensors[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int num_atoms = 16;

    double symprec = 1e-5;

    SpglibMagneticDataset *dataset;
    dataset = spg_get_magnetic_dataset(lattice, positions, types, tensors,
                                       0 /* tensor_rank */, num_atoms,
                                       0 /* is_axial */, symprec);
    EXPECT_EQ(spg_get_error_code(), SpglibError::SPGLIB_SUCCESS);

    if (dataset) spg_free_magnetic_dataset(dataset);
}

// TODO: test get_magnetic_dataset with distorted positions
