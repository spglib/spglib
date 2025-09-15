#include <cstdlib>

#include <gtest/gtest.h>

extern "C" {
#include "debug.h"
#include "mathfunc.h"
#include "niggli.h"
#include "utils.h"
}

TEST(Niggli, Niggli_reduce) {
    double min_lattice[3][3];
    double lattice[3][3] = {
        {-2.2204639179669590, -4.4409278359339179, 179.8575773553236843},
        {1.2819854407640749, 0.0, 103.8408207018900669},
        {10.5158083946732219, 0.0, 883.3279051525505565},
    };
    double inv_lat[3][3], tmat[3][3], metric[3][3];
    int int_tmat[3][3];

    double const symprec = 1e-5;
    mat_copy_matrix_d3(min_lattice, lattice);

    auto setenv_result = setenv("SPGLIB_NUM_ATTEMPTS", "100", 1);
    ASSERT_EQ(setenv_result, 0);

    int succeeded = niggli_reduce((double *)min_lattice, symprec, -1);
    // Default get_num_attempts() == 1000 --> succeeded == 1.
    // With get_num_attempts() == 100 --> succeeded == 0.
    EXPECT_EQ(succeeded, 0);

    setenv_result = setenv("SPGLIB_NUM_ATTEMPTS", "1000", 1);
    ASSERT_EQ(setenv_result, 0);

    succeeded = niggli_reduce((double *)min_lattice, symprec, -1);
    EXPECT_EQ(succeeded, 1);

    mat_inverse_matrix_d3(inv_lat, lattice, symprec);
    mat_multiply_matrix_d3(tmat, inv_lat, min_lattice);
    mat_cast_matrix_3d_to_3i(int_tmat, tmat);
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

    // tmat must be almost integers.
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            EXPECT_NEAR(int_tmat[i][j], tmat[i][j], 1e-8);
        }
    }
    EXPECT_FLOAT_EQ(metric[0][0] + metric[1][1] + metric[2][2], 156.599853);
}
