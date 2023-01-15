# Test Spglib with googletest

## Requirements

- CMake >= 3.11
- (Googletest is automatically prepared with [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html))

## How to run single test

In the `test` folder of the build directory (e.g. `build/test`), run the specific test executable.
For example, to run `test_symmetry_search.test_spg_get_symmetry`
```shell
# at build/test
./test_symmetry_search --gtest_filter=test_spg_get_symmetry
```

## How to add a new test file

1. Create `<your_test_suite_name>.cpp`
2. Append `<your_test_suite_name>` at the bottom loops in [the current CMakeLists.txt](CMakeLists.txt)

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

## Utility functions for preparing structures

When you find a doubtful `cell` with Python API, the following python snippets will be useful to prepare a test case for Python testing and C testing.

```python
def print_python_cell(cell):
    if len(cell) == 3:
        lattice, positions, numbers = cell
        magmoms = None
    elif len(cell) == 4:
        lattice, positions, numbers, magmoms = cell

    contents = []

    # lattice
    contents.append("lattice = np.array([")
    for i in range(3):
        contents.append(f"    [{lattice[i][0]:.8f}, {lattice[i][1]:.8f}, {lattice[i][2]:.8f}],")
    contents.append("])")

    # positions
    contents.append("positions = np.array([")
    for i in range(len(positions)):
        contents.append(
            f"    [{positions[i][0]:.8f}, {positions[i][1]:.8f}, {positions[i][2]:.8f}],"
        )
    contents.append("])")

    # numbers
    contents.append("numbers = np.array([" + ", ".join(map(str, numbers)) + "])")

    # magmoms
    if magmoms:
        contents.append("magmoms = np.array([")
        for i in range(len(positions)):
            contents.append(f"    [{magmoms[i][0]:.8f}, {magmoms[i][1]:.8f}, {magmoms[i][2]:.8f}],")
        contents.append("])")

    print("\n".join(contents))
```

```python
def print_cpp_cell(cell):
    if len(cell) == 3:
        lattice, positions, numbers = cell
        magmoms = None
    elif len(cell) == 4:
        lattice, positions, numbers, magmoms = cell

    contents = []

    # lattice
    lattice_col = lattice.T
    contents.append("double lattice[3][3] = {")
    for i in range(3):
        contents.append(
            f"    {{ {lattice_col[i][0]:.8f}, {lattice_col[i][1]:.8f}, {lattice_col[i][2]:.8f} }},"
        )
    contents.append("};")

    # positions
    contents.append("double positions[][3] = {")
    for i in range(len(positions)):
        contents.append(
            f"    {{ {positions[i][0]:.8f}, {positions[i][1]:.8f}, {positions[i][2]:.8f} }},"
        )
    contents.append("};")

    # numbers
    contents.append("int types[] = {" + ", ".join(map(str, numbers)) + "};")

    # magmoms
    if magmoms:
        contents.append("double spins[] = {")
        for i in range(len(positions)):
            contents.append(f"    {magmoms[i][0]:.8f}, {magmoms[i][1]:.8f}, {magmoms[i][2]:.8f},")
        contents.append("};")

    contents.append(f"int num_atom = {len(positions)};")

    print("\n".join(contents))
```

## Resources
- <https://google.github.io/googletest/>
