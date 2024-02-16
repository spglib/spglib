#include <cmath>
#include <cstdlib>

#include <gtest/gtest.h>

extern "C" {
#include "debug.h"
#include "delaunay.h"
#include "mathfunc.h"
#include "utils.h"
}

TEST(Delaunay, Delaunay_reduce_layer) {
    double min_lattice[3][3];
    // Unimodular matrix
    double lattice[3][3] = {
        {1, 0, 0},
        {0, 37, 10},
        {0, 11, 3},
    };
    int const aperiodic_axis = 0;
    double const symprec = 1e-5;

    del_layer_delaunay_reduce(min_lattice, lattice, aperiodic_axis, symprec);

    // Shortest vectors in the periodic plane are [0, 1, 0] and [0, 0, 1]
    for (int i = 0; i < 3; ++i) {
        double sum = fabs(min_lattice[0][i]) + fabs(min_lattice[1][i]) +
                     fabs(min_lattice[2][i]);
        EXPECT_FLOAT_EQ(sum, 1);
    }
}

TEST(Delaunay, Delaunay_reduce) {
    double min_lattice[3][3];
    double lattice[3][3] = {
        {-2.2204639179669590, -4.4409278359339179, 179.8575773553236843},
        {1.2819854407640749, 0.0, 103.8408207018900669},
        {10.5158083946732219, 0.0, 883.3279051525505565},
    };
    double inv_lat[3][3], tmat[3][3], metric[3][3];
    int int_tmat[3][3];

    auto setenv_result = setenv("SPGLIB_NUM_ATTEMPTS", "100", 1);
    ASSERT_EQ(setenv_result, 0);

    double const symprec = 1e-5;
    int succeeded = del_delaunay_reduce(min_lattice, lattice, symprec);
    // Default get_num_attempts() == 1000 --> succeeded == 1.
    // With get_num_attempts() == 100 --> succeeded == 0.
    EXPECT_EQ(succeeded, 0);

    setenv_result = setenv("SPGLIB_NUM_ATTEMPTS", "1000", 1);
    ASSERT_EQ(setenv_result, 0);

    succeeded = del_delaunay_reduce(min_lattice, lattice, symprec);

    EXPECT_EQ(succeeded, 1);

    mat_get_metric(metric, min_lattice);

    if (HasFailure()) {
        printf("min_lattice\n");
        show_matrix_3d(min_lattice);
        printf("tmat\n");
        show_matrix_3d(tmat);
        printf("metric tensor\n");
        show_matrix_3d(metric);
        return;
    }
    mat_inverse_matrix_d3(inv_lat, lattice, symprec);
    mat_multiply_matrix_d3(tmat, inv_lat, min_lattice);
    mat_cast_matrix_3d_to_3i(int_tmat, tmat);
    // tmat must be almost integers.
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            EXPECT_NEAR(int_tmat[i][j], tmat[i][j], 1e-8);
        }
    }
    EXPECT_FLOAT_EQ(metric[0][0] + metric[1][1] + metric[2][2], 156.599853);
}
