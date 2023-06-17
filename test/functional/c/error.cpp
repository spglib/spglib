#include <gtest/gtest.h>

#include <list>
#include <random>

extern "C" {
#include "spglib.h"
}

TEST(Error, test_spg_get_error_message) {
    // printf("Primitive cell was not found.\n");
    double lattice[3][3] = {{4, 0, 0}, {0, 4, 0}, {0, 0, 4}};
    double position[][3] = {{0, 0, 0}, {0.5, 0.5, 0.5}, {0.5, 0.5, 0.5}};
    int types[] = {1, 1, 1};
    int num_atom = 3;
    int num_primitive_atom;
    double symprec = 1e-5;
    SpglibError error;

    /* lattice, position, and types are overwritten. */
    num_primitive_atom =
        spg_find_primitive(lattice, position, types, num_atom, symprec);
    ASSERT_EQ(num_primitive_atom, 0);

    error = spg_get_error_code();
    printf("%s\n", spg_get_error_message(error));
    ASSERT_NE(error, SPGLIB_SUCCESS);
}
