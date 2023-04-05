#include <gtest/gtest.h>

extern "C" {
#include "spglib.h"
#include "utils.h"
}

static void show_magnetic_symmetry_operations(const int (*rotations)[3][3],
                                              const double (*translations)[3],
                                              const int *time_reversals,
                                              const int size);
static void show_magnetic_spacegroup_type(
    const SpglibMagneticSpacegroupType msgtype);
static void show_spg_magnetic_dataset(const SpglibMagneticDataset *dataset);

TEST(test_magnetic_symmetry, test_spg_get_magnetic_symmetry_from_database) {
    int rotations[384][3][3];
    double translations[384][3];
    int time_reversals[384];
    int i, j, size;

    /* bns_number: 146.12, uni_number 1242 */
    /* hall_number: 433 -> 146:h */
    /* hall_number: 434 -> 146:r */
    size = spg_get_magnetic_symmetry_from_database(rotations, translations,
                                                   time_reversals, 1242, 434);
    ASSERT_TRUE(size > 0);

    show_magnetic_symmetry_operations(rotations, translations, time_reversals,
                                      size);
}

TEST(test_magnetic_symmetry, test_spg_get_symmetry_with_collinear_spin) {
    double lattice[3][3] = {{4, 0, 0}, {0, 4, 0}, {0, 0, 4}};
    double position[][3] = {{0, 0, 0}, {0.5, 0.5, 0.5}};
    int types[] = {1, 1};
    int equivalent_atoms[2];
    double spins[2];
    int num_atom = 2;
    int i, j, size, retval, max_size;

    int(*rotation)[3][3];
    double(*translation)[3];

    max_size = num_atom * 48;
    rotation = (int(*)[3][3])malloc(sizeof(int[3][3]) * max_size);
    translation = (double(*)[3])malloc(sizeof(double[3]) * max_size);

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

TEST(test_magnetic_symmetry, test_spg_get_symmetry_with_site_tensors) {
    /* MAGNDATA #0.1: LaMnO3 */
    /* BNS: Pn'ma' (62.448), MHall: -P 2ac' 2n' (546) */
    int max_size, size, i, j;
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
    int(*rotation)[3][3];
    double(*translation)[3];
    int *spin_flips;

    max_size = num_atom * 96;
    rotation = (int(*)[3][3])malloc(sizeof(int[3][3]) * max_size);
    translation = (double(*)[3])malloc(sizeof(double[3]) * max_size);
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

TEST(test_magnetic_symmetry, test_spg_get_magnetic_spacegroup_type) {
    SpglibMagneticSpacegroupType msgtype;
    msgtype = spg_get_magnetic_spacegroup_type(1279);
    ASSERT_NE(msgtype.uni_number, 0);
    show_magnetic_spacegroup_type(msgtype);
}

TEST(test_magnetic_symmetry, test_spg_get_magnetic_dataset) {
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
        ASSERT_EQ(dataset->msg_type, 1);
        ASSERT_EQ(dataset->uni_number, 1155);
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
        ASSERT_EQ(dataset->msg_type, 2);
        ASSERT_EQ(dataset->uni_number, 1156);
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
        ASSERT_EQ(dataset->msg_type, 3);
        ASSERT_EQ(dataset->uni_number, 1158);
        show_spg_magnetic_dataset(dataset);

        spg_free_magnetic_dataset(dataset);
    }
}

TEST(test_magnetic_symmetry, test_spg_get_magnetic_dataset_type4) {
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
    ASSERT_EQ(dataset->msg_type, 4);
    ASSERT_EQ(dataset->uni_number, 932);
    show_spg_magnetic_dataset(dataset);

    spg_free_magnetic_dataset(dataset);
}

TEST(test_magnetic_symmetry, test_spg_get_symmetry_with_tensors_rough_symprec) {
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
    int max_size = num_atoms * 96;

    double symprec = 1e-2;         // with high symprec
    double mag_symprec = symprec;  // with high mag_symprec

    int i, size;
    int equivalent_atoms[3];
    double primitive_lattice[3][3];
    int(*rotations)[3][3];
    double(*translations)[3];
    int *spin_flips;
    int *time_reversals;

    rotations = (int(*)[3][3])malloc(sizeof(int[3][3]) * max_size);
    translations = (double(*)[3])malloc(sizeof(double[3]) * max_size);
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

TEST(test_magnetic_symmetry, test_with_broken_symmetry) {
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
    int max_size = num_atoms * 96;

    double symprec = 0.1;  // with very high symprec
    double mag_symprec = symprec;

    int i, size;
    int equivalent_atoms[4];
    double primitive_lattice[3][3];
    int(*rotations)[3][3];
    double(*translations)[3];
    int *spin_flips;
    int *time_reversals;

    rotations = (int(*)[3][3])malloc(sizeof(int[3][3]) * max_size);
    translations = (double(*)[3])malloc(sizeof(double[3]) * max_size);
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
    if (dataset != NULL) spg_free_magnetic_dataset(dataset);
    free(dataset);
}

// TODO: test get_magnetic_dataset with distorted positions

// ****************************************************************************
// Local functions
// ****************************************************************************

static void show_magnetic_symmetry_operations(const int (*rotations)[3][3],
                                              const double (*translations)[3],
                                              const int *time_reversals,
                                              const int size) {
    int i, j;
    for (i = 0; i < size; i++) {
        printf("--- %d ---\n", i + 1);
        for (j = 0; j < 3; j++) {
            printf("%2d %2d %2d\n", rotations[i][j][0], rotations[i][j][1],
                   rotations[i][j][2]);
        }
        printf("%f %f %f\n", translations[i][0], translations[i][1],
               translations[i][2]);
        printf("%2d\n", time_reversals[i]);
    }
}

static void show_magnetic_spacegroup_type(
    const SpglibMagneticSpacegroupType msgtype) {
    printf("UNI Number:    %d\n", msgtype.uni_number);
    printf("Litvin Number: %d\n", msgtype.litvin_number);
    printf("BNS Number:    %s\n", msgtype.bns_number);
    printf("OG Number:     %s\n", msgtype.og_number);
    printf("Number:        %d\n", msgtype.number);
    printf("Type:          %d\n", msgtype.type);
}

static void show_spg_magnetic_dataset(const SpglibMagneticDataset *dataset) {
    int i, p, s;
    printf("UNI number: %d\n", dataset->uni_number);
    printf("Type: %d\n", dataset->msg_type);
    printf("Hall number: %d\n", dataset->hall_number);

    printf("\nSymmetry operations\n");
    for (p = 0; p < dataset->n_operations; p++) {
        printf("--- %d ---\n", p + 1);
        for (s = 0; s < 3; s++) {
            printf("%2d %2d %2d\n", dataset->rotations[p][s][0],
                   dataset->rotations[p][s][1], dataset->rotations[p][s][2]);
        }
        printf("%f %f %f\n", dataset->translations[p][0],
               dataset->translations[p][1], dataset->translations[p][2]);
        printf("%d\n", dataset->time_reversals[p]);
    }

    printf("\nEquivalent atoms:\n");
    for (i = 0; i < dataset->n_atoms; i++) {
        printf(" %d", dataset->equivalent_atoms[i]);
    }
    printf("\n");

    printf("\nTransformation matrix:\n");
    for (s = 0; s < 3; s++) {
        printf("%f %f %f\n", dataset->transformation_matrix[s][0],
               dataset->transformation_matrix[s][1],
               dataset->transformation_matrix[s][2]);
    }
    printf("Origin shift:\n");
    printf("%f %f %f\n", dataset->origin_shift[0], dataset->origin_shift[1],
           dataset->origin_shift[2]);

    printf("\nStandardization\n");
    printf("Rigid rotation\n");
    for (s = 0; s < 3; s++) {
        printf("%f %f %f\n", dataset->std_rotation_matrix[s][0],
               dataset->std_rotation_matrix[s][1],
               dataset->std_rotation_matrix[s][2]);
    }
    printf("Lattice\n");
    for (s = 0; s < 3; s++) {
        printf("%f %f %f\n", dataset->std_lattice[s][0],
               dataset->std_lattice[s][1], dataset->std_lattice[s][2]);
    }
    printf("Positions, types, site tensors \n");
    for (i = 0; i < dataset->n_std_atoms; i++) {
        printf("[%f %f %f], types=%d, ", dataset->std_positions[i][0],
               dataset->std_positions[i][1], dataset->std_positions[i][2],
               dataset->std_types[i]);
        if (dataset->tensor_rank == 0) {
            printf("%f\n", dataset->std_tensors[i]);
        } else if (dataset->tensor_rank == 1) {
            printf("%f %f %f \n", dataset->std_tensors[i * 3],
                   dataset->std_tensors[i * 3 + 1],
                   dataset->std_tensors[i * 3 + 2]);
        }
    }
}
