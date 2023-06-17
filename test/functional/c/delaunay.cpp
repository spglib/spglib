#include <gtest/gtest.h>

extern "C" {
#include <math.h>

#include "debug.h"
#include "delaunay.h"
}

TEST(Delaunay, Delaunay_reduce_layer) {
    double min_lattice[3][3];
    // Unimodular matrix
    double lattice[3][3] = {
        {1, 0, 0},
        {0, 37, 10},
        {0, 11, 3},
    };
    const int aperiodic_axis = 0;
    const double symprec = 1e-5;

    del_layer_delaunay_reduce(min_lattice, lattice, aperiodic_axis, symprec);

    // Shortest vectors in the periodic plane are [0, 1, 0] and [0, 0, 1]
    debug_print_matrix_d3(min_lattice);
    for (int i = 0; i < 3; ++i) {
        double sum = fabs(min_lattice[0][i]) + fabs(min_lattice[1][i]) +
                     fabs(min_lattice[2][i]);
        ASSERT_FLOAT_EQ(sum, 1);
    }
}
