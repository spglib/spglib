# Test Spglib with googletest

## Requirements

- CMake >= 3.11
- (Googletest is automatically prepared with [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html))

## How to run tests

For CMake>=3.14
```shell
cd test
cmake -S . -B build -DUSE_SANITIZER="Address" -DCMAKE_BUILD_TYPE="Debug"
cmake --build build
# With multi cores
# cmake --build build -j 32

cd build
ctest
# ctest -V
```

For older CMake<3.14
```shell
cd test
mkdir build && cd build
cmake -DUSE_SANITIZER="Address" -DCMAKE_BUILD_TYPE="Debug" ..
cmake --build build
ctest
```

## How to run single test

Example to run `test_symmetry_search.test_spg_get_symmetry`
```shell
# at test/build
./test_symmetry_search --gtest_filter=test_spg_get_symmetry
```

## How to add new test file

1. Create `test/<your_test_suite_name>.cpp`
2. Append `<your_test_suite_name>` at the bottom loops in [test/CMakeLists.txt](test/CMakeLists.txt)

## How to add test

`test/<your_test_suite_name>.cpp`

```cpp
#include <gtest/gtest.h>

extern "C" {
  #include "spglib.h"
}

TEST(<your_test_suite_name>, <test_name>) {
    // ASSERT_EQ(a, b)
    // ASSERT_NE(a, b)
    // ASSERT_TRUE(a)
}
```

## Resources
- <https://google.github.io/googletest/>
