#include <gtest/gtest.h>

extern "C" {
#include "spglib.h"
#include "utils.h"
}

// TODO: Move this to a test fixture and test more values
TEST(DatasetAccess, test_spg_get_spacegroup_type) {
    SpglibSpacegroupType spgtype = spg_get_spacegroup_type(446);
    ASSERT_EQ(spg_get_error_code(), SPGLIB_SUCCESS);
    EXPECT_EQ(spgtype.number, 156);
}
