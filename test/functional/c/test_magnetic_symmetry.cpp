#include <gtest/gtest.h>

extern "C" {
#include "spglib.h"
#include "utils.h"
}

TEST(MagneticSymmetry, test_spg_get_magnetic_symmetry_from_database) {
    int rotations[384][3][3];
    double translations[384][3];
    int time_reversals[384];
    int size;

    /* bns_number: 146.12, uni_number 1242 */
    /* hall_number: 433 -> 146:h */
    /* hall_number: 434 -> 146:r */
    size = spg_get_magnetic_symmetry_from_database(rotations, translations,
                                                   time_reversals, 1242, 434);
    ASSERT_TRUE(size > 0);

    show_magnetic_symmetry_operations(rotations, translations, time_reversals,
                                      size);
}

TEST(MagneticSymmetry, test_spg_get_symmetry_with_collinear_spin) {
    double lattice[3][3] = {{4, 0, 0}, {0, 4, 0}, {0, 0, 4}};
    double position[][3] = {{0, 0, 0}, {0.5, 0.5, 0.5}};
    int types[] = {1, 1};
    int equivalent_atoms[2];
    double spins[2];
    int num_atom = 2;
    int size, max_size;

    int (*rotation)[3][3];
    double (*translation)[3];

    max_size = num_atom * 48;
    rotation = (int (*)[3][3])malloc(sizeof(int[3][3]) * max_size);
    translation = (double (*)[3])malloc(sizeof(double[3]) * max_size);

    printf("*** spg_get_symmetry_with_spin (BCC ferro) ***:\n");
    {
        spins[0] = 0.6;
        spins[1] = 0.6;
        size = spg_get_symmetry_with_collinear_spin(
            rotation, translation, equivalent_atoms, max_size, lattice,
            position, types, spins, num_atom, 1e-5);
        ASSERT_TRUE(size > 0);

        show_symmetry_operations(rotation, translation, size);
    }

    printf("*** Example of spg_get_symmetry_with_spin (BCC antiferro) ***:\n");
    {
        spins[0] = 0.6;
        spins[1] = -0.6;
        size = spg_get_symmetry_with_collinear_spin(
            rotation, translation, equivalent_atoms, max_size, lattice,
            position, types, spins, num_atom, 1e-5);
        ASSERT_TRUE(size > 0);

        show_symmetry_operations(rotation, translation, size);
    }

    printf("*** spg_get_symmetry_with_spin (BCC broken spin) ***:\n");
    {
        spins[0] = 0.6;
        spins[1] = 1.2;
        size = spg_get_symmetry_with_collinear_spin(
            rotation, translation, equivalent_atoms, max_size, lattice,
            position, types, spins, num_atom, 1e-5);
        ASSERT_TRUE(size > 0);

        show_symmetry_operations(rotation, translation, size);
    }

    free(rotation);
    rotation = NULL;
    free(translation);
    translation = NULL;
}

TEST(MagneticSymmetry, test_spg_get_symmetry_with_site_tensors) {
    /* MAGNDATA #0.1: LaMnO3 */
    /* BNS: Pn'ma' (62.448), MHall: -P 2ac' 2n' (546) */
    int max_size, size, i;
    double lattice[][3] = {{5.7461, 0, 0}, {0, 7.6637, 0}, {0, 0, 5.5333}};
    /* clang-format off */
    double position[][3] = {
        {0.051300, 0.250000, 0.990500}, /* La */
        {0.948700, 0.750000, 0.009500},
        {0.551300, 0.250000, 0.509500},
        {0.448700, 0.750000, 0.490500},
        {0.000000, 0.000000, 0.500000}, /* Mn */
        {0.000000, 0.500000, 0.500000},
        {0.500000, 0.500000, 0.000000},
        {0.500000, 0.000000, 0.000000},
        {0.484900, 0.250000, 0.077700}, /* O1 */
        {0.515100, 0.750000, 0.922300},
        {0.984900, 0.250000, 0.422300},
        {0.015100, 0.750000, 0.577700},
        {0.308500, 0.040800, 0.722700}, /* O2 */
        {0.691500, 0.540800, 0.277300},
        {0.691500, 0.959200, 0.277300},
        {0.308500, 0.459200, 0.722700},
        {0.808500, 0.459200, 0.777300},
        {0.191500, 0.959200, 0.222700},
        {0.191500, 0.540800, 0.222700},
        {0.808500, 0.040800, 0.777300},
    };
    int types[] = {1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
    double tensors[] = {
        0, 0, 0, /* La */
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        3.87, 0, 0, /* Mn */
        -3.87, 0, 0,
        -3.87, 0, 0,
        3.87, 0, 0,
        0, 0, 0, /* O1 */
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0, /* O2 */
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
        0, 0, 0,
    };
    /* clang-format on */
    int num_atom = 20;

    int equivalent_atoms[20];
    double primitive_lattice[3][3];
    int (*rotation)[3][3];
    double (*translation)[3];
    int *spin_flips;

    // 96  = 48 * 2
    // = (max number of order of point group) * (spin degrees of freedom)
    max_size = num_atom * 96;
    rotation = (int (*)[3][3])malloc(sizeof(int[3][3]) * max_size);
    translation = (double (*)[3])malloc(sizeof(double[3]) * max_size);
    spin_flips = (int *)malloc(sizeof(int *) * max_size);

    /* Find equivalent_atoms, primitive_lattice, spin_flips */
    size = spg_get_symmetry_with_site_tensors(
        rotation, translation, equivalent_atoms, primitive_lattice, spin_flips,
        max_size, lattice, position, types, tensors, 1, num_atom,
        1 /* with_time_reversal */, 1 /* is_axial */, 1e-5);
    ASSERT_EQ(size, 8);

    // Test spg_get_magnetic_spacegroup_type_from_symmetry
    int *time_reversals;
    time_reversals = (int *)malloc(sizeof(int *) * size);
    for (i = 0; i < size; i++) {
        time_reversals[i] = (1 - spin_flips[i]) / 2;
    }
    SpglibMagneticSpacegroupType msgtype =
        spg_get_magnetic_spacegroup_type_from_symmetry(
            rotation, translation, time_reversals, size, lattice, 1e-5);
    ASSERT_EQ(msgtype.uni_number, 546);

    printf("*** spg_get_symmetry_with_site_tensors (type-III) ***:\n");
    show_magnetic_symmetry_operations(rotation, translation, time_reversals,
                                      size);

    free(rotation);
    rotation = NULL;
    free(translation);
    translation = NULL;
    free(spin_flips);
    spin_flips = NULL;
    free(time_reversals);
    time_reversals = NULL;
}

TEST(MagneticSymmetry, test_spg_get_magnetic_spacegroup_type) {
    SpglibMagneticSpacegroupType msgtype;
    msgtype = spg_get_magnetic_spacegroup_type(1279);
    ASSERT_NE(msgtype.uni_number, 0);
    show_magnetic_spacegroup_type(msgtype);
}

TEST(MagneticSymmetry, test_spg_get_symmetry_with_tensors_rough_symprec) {
    // https://github.com/spglib/spglib/issues/186
    double lattice[][3] = {
        // column-wise!
        {3.36017962, 1.70747655, -0.0117898},
        {0., 3.84457132, -0.29501452},
        {0.95244787, 0.37960516, 6.4450749},
    };
    double positions[][3] = {
        {0.0, 0.0, 0.0},
        {0.26737473, 0.2232512, 0.24199933},
        {0.73262527, 0.7767488, 0.75800067},
    };
    int types[] = {0, 1, 1};
    double tensors[] = {0.005, 0.002, 0.002};
    int num_atoms = 3;
    // 96  = 48 * 2
    // = (max number of order of point group) * (spin degrees of freedom)
    int max_size = num_atoms * 96;
    double symprec = 1e-2;         // with high symprec
    double mag_symprec = symprec;  // with high mag_symprec

    int i, size;
    int equivalent_atoms[3];
    double primitive_lattice[3][3];
    int (*rotations)[3][3];
    double (*translations)[3];
    int *spin_flips;
    int *time_reversals;

    rotations = (int (*)[3][3])malloc(sizeof(int[3][3]) * max_size);
    translations = (double (*)[3])malloc(sizeof(double[3]) * max_size);
    spin_flips = (int *)malloc(sizeof(int *) * max_size);
    time_reversals = (int *)malloc(sizeof(int *) * max_size);

    size = spg_get_symmetry(rotations, translations, max_size, lattice,
                            positions, types, num_atoms, symprec);
    ASSERT_EQ(size, 4);
    show_symmetry_operations(rotations, translations, size);

    size = spgms_get_symmetry_with_site_tensors(
        rotations, translations, equivalent_atoms, primitive_lattice,
        spin_flips, max_size, lattice, positions, types, tensors,
        0 /* tensor_rank */, num_atoms, 1 /* with_time_reversal */,
        0 /* is_axial */, symprec, -1 /* angle_tolerance */, mag_symprec);
    ASSERT_EQ(size, 4);

    for (i = 0; i < size; i++) {
        time_reversals[i] = (1 - spin_flips[i]) / 2;
    }
    show_magnetic_symmetry_operations(rotations, translations, time_reversals,
                                      size);

    free(rotations);
    free(translations);
    free(spin_flips);
    free(time_reversals);
}

TEST(MagneticSymmetry, test_spg_get_symmetry_with_site_tensors_non_collinear) {
    double lattice[3][3] = {{10, 0, 0}, {0, 10, 0}, {0, 0, 10}};
    double position[][3] = {
        {0, 0, 0},
    };
    int types[] = {1};
    double tensors[3] = {1, 0, 0};
    int num_atoms = 1;
    // 96  = 48 * 2
    // = (max number of order of point group) * (spin degrees of freedom)
    int max_size = num_atoms * 96;

    int size;
    int equivalent_atoms[1];
    double primitive_lattice[3][3];
    int (*rotations)[3][3];
    double (*translations)[3];
    int *spin_flips;
    int *time_reversals;

    rotations = (int (*)[3][3])malloc(sizeof(int[3][3]) * max_size);
    translations = (double (*)[3])malloc(sizeof(double[3]) * max_size);
    spin_flips = (int *)malloc(sizeof(int *) * max_size);
    time_reversals = (int *)malloc(sizeof(int *) * max_size);

    size = spg_get_symmetry_with_site_tensors(
        rotations, translations, equivalent_atoms, primitive_lattice,
        spin_flips, max_size, lattice, position, types, tensors, 1, num_atoms,
        1 /* with_time_reversal */, 1 /* is_axial */, 1e-5);

    ASSERT_TRUE(size == 16);
    free(rotations);
    free(translations);
    free(spin_flips);
    free(time_reversals);
}
