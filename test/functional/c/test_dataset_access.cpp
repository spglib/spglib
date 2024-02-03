#include <gtest/gtest.h>

#include "c_fixtures.hpp"

extern "C" {
#include "spglib.h"
#include "utils.h"
}

// TODO: Make a list of relevant dataset input instantiations
INSTANTIATE_TEST_SUITE_P(
    DatasetAccess, Dataset,
    testing::Values(
        /* Input 1 */
        /* Im-3m (229) */
        DatasetInput{
            {
                {{4, 0, 0}, {0, 4, 0}, {0, 0, 4}} /* lattice */
            },
            {
                /* Vector of atoms */
                {

                    {0, 0, 0}, /* position */
                    1          /* type_number */
                },             /* atom 1 */
                {

                    {0.5, 0.5, 0.5}, /* position */
                    1                /* type_number */
                },                   /* atom 2 */

            },
            1e-5 /* tolerance/symprec */
        }));

TEST(DatasetAccess, test_spg_get_symmetry_from_database) {
    int rotations[192][3][3];
    double translations[192][3];
    int size;

    size = spg_get_symmetry_from_database(rotations, translations, 460);
    ASSERT_EQ(size, 36);
    show_symmetry_operations(rotations, translations, size);
}

TEST_P(Dataset, test_spg_get_hall_number_from_symmetry) {
    printf("hall_number = %d is found by spg_get_dataset.\n",
           dataset->hall_number);
    int hall_number = spg_get_hall_number_from_symmetry(
        dataset->rotations, dataset->translations, dataset->n_operations,
        input.tolerance);
    printf("hall_number = %d is found by spg_get_hall_number_from_symmetry.\n",
           hall_number);
    EXPECT_EQ(hall_number, dataset->hall_number);
    EXPECT_EQ(hall_number, 529);

    auto spgtype = spg_get_spacegroup_type(hall_number);
    EXPECT_EQ(spgtype.number, 229);
    show_spacegroup_type(spgtype);
}

TEST_P(Dataset, test_spg_get_spacegroup_type_from_symmetry) {
    printf("hall_number = %d is found by spg_get_dataset.\n",
           dataset->hall_number);
    auto spgtype = spg_get_spacegroup_type_from_symmetry(
        dataset->rotations, dataset->translations, dataset->n_operations,
        input.lattice, input.tolerance);
    printf("number = %d is found by spg_get_spacegroup_type_from_symmetry.\n",
           spgtype.number);

    show_spacegroup_type(spgtype);
    EXPECT_EQ(spgtype.number, dataset->spacegroup_number);
    EXPECT_EQ(spgtype.number, 229);
}
