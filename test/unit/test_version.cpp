#include <gtest/gtest.h>

#include <string>

extern "C" {
#include "spglib.h"
}

// Stringerize compiler definitions
#define STR(x) #x
#define STRING(x) STR(x)

TEST(Version, version_strings) {
    std::string version(spg_get_version());
    std::string version_full(spg_get_version_full());
    std::string commit(spg_get_commit());

    EXPECT_EQ(version, STRING(Spglib_VERSION));
    EXPECT_EQ(version_full, STRING(Spglib_VERSION_FULL));
    EXPECT_EQ(commit, STRING(Spglib_COMMIT));
}
