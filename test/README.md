# Spglib test-suite

The main Spglib test-suite is a standalone CMake project that can be executed on a pre-installed Spglib package or as
part of the main Spglib project. On top of this, there are language-dependent test-suites, e.g. `pytest` for the Python
bindings, and a [`tmt` test layer][tmt-section] that orchestrates all tests across each test-suite.

## Requirements

- CMake >= 3.11
- [`Googletest`] (downloaded via `FetchContent` if not available)
- Optional:
  - `tmt`: to execute the tmt tests locally. Currently only Fedora-like systems are [supported][tmt-2557]
  - `[test]` python optional dependency: to execute python tests

## Folder structure

We organize the test-suite as follows:

- `unit`: Unit tests that test small individual components of the code-base
- `integration`: Integration tests that test the interplay between the units and the external dependencies. (Currently
  the api binding tests are not covered here, but in `functional`)
- `functional`: Functional tests that test the physical correctness and how the user would actually interact with the
  library
- `example`: Standalone example project tests. These are symlinked to `../example` folder
- `package`: Packaging tests that check the supported methods that the project can be imported

## Ctest layer

All tests are either implemented within the `ctest` test-suite layer or they are indirectly called by it. There can be
discrepancy in what and how the tests are executed, e.g. the project source being tested in `package` tests, and these
differences are being covered by the [`tmt` test layer][tmt-section].

Tests on the `ctest` layer are implemented in a few methods:

- as `Googletest` individual tests: `unit`, `functional`, `integration`
- as standalone CMake projects invoked by [`ctest --build-and-test`]: `example`, `package`
- as standalone test-suite invoked directly by ctest: `pytest`

### How to build the ctest project

There are two ways of building and executing the ctest test-suite:

- Build as part of the main Spglib project with `SPGLIB_WITH_TESTS=ON`. This tests the main Spglib project that is being
  built
- Build as standalone project, i.e. running the typical cmake workflow in this test folder
  ```console
  $ cmake -B ./build
  $ cmake --build ./build
  $ ctest --test-dir ./build
  ```
  This tests the Spglib project that is already installed on the system via `find_package`, or that `FetchContent`
  points to

### How to run specific tests

You can find a full list of available tests by running:

```console
$ ctest --test-dir ./build -N
```

The `-N` can also be used to check the effect of the test filtering by regex/labels.

The tests are also organized by labels which you can find a list of using:

```console
$ ctest --test-dir ./build --print-labels
```

To select specific tests, you can either use regex `-R` (simplest being just the name of the test), or the labels `-L`:

```console
$ ctest --test-dir ./build -R "SymmetrySearch.test_spg_get_symmetry"
$ ctest --test-dir ./build -L unit_tests
```

### How to add tests

First you have to consider what is being tested and if it is a `unit`/`functional`/`integration` etc. test. After that,
depending on the type of test you either need to:

- Add or expand a `<gtest_testsuite_name>.cpp` file with appropriate link in the folder's `CMakeLists.txt` file. The
  content of the `<gtest_testsuite_name>.cpp` looks like:
  ```cpp
  #include <gtest/gtest.h>

  extern "C" {
    #include "spglib.h"
  }

  TEST(<gtest_testsuite_name>, <test_name>) {
      // ASSERT_EQ(a, b)
      // ASSERT_NE(a, b)
      // ASSERT_TRUE(a)
  }
  ```
- Add an example CMake project. It should be buildable by itself with optional/minimal integrated tests. Use the
  [`tmt` test layer][tmt-section] to do more complex test checks.
- Add or expand a language-specific test. Make sure the test-suite is included as an `add_test` inside `ctest`

### Other notes

#### VSCode setting

Although Googletest is automatically prepared, you may need to install it locally to highlight structs and functions.

Below is an example of `c_cpp_properties.json` for Mac.

```json
{
    "configurations": [
        {
            ...
            "includePath": [
                "${workspaceFolder}/**",
                "${workspaceFolder}/cmake-build-release-gcc/_deps/googletest-src/googletest/include",
                "${workspaceFolder}/cmake-build-release-llvm/_deps/googletest-src/googletest/include"
            ],
            ...
        }
    ]
}
```

The `launch.json` below runs a single test with a debugger:

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug Launch(lldb): #381",
            "type": "lldb",
            "request": "launch",
            "program": "${workspaceFolder}/build/test/test_suite",
            "args": ["--gtest_filter=MagneticDataset.test_with_slightly_distorted_positions"],
            "cwd": "${workspaceFolder}/build/test",
        },
    ]
}
```

#### Utility functions for preparing structures

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

    contents.append(f"int num_atoms = {len(positions)};")

    print("\n".join(contents))
```

#### Exposing API for unit tests

Normally all functions outside of the exported `spglib.h` are not exposed, but for the purposes of unit testing, we need
access to these. Currently, we don't know a clear way of resolving this issue, but we know two ways of addressing this:

- Building Spglib as a static library for linking to the test-suite
- Use a macro `SPG_API_TEST` that expands to `SPG_API` when building unit tests

We have opted for the latter approach for now, so keep in mind when writing unit tests to include `SPG_API_TEST`
directive.

## Tmt test-suite

The [`tmt`] test layer is primarily for orchestrating and running all the tests in arbitrary environments, e.g. in the
CI, post packaging, in the Fedora-CI, etc. Although this test-suite is currently limited to fedora-like environments,
there are plans to [expand this][tmt-2557] to arbitrary systems soon.

The `tmt` tests-suite is composed of `plans` (a collection of `tests` with a common preparation step) and individual
`tests`, all of which are defined in the `.fmf` files.

### Introspecting tmt tests/plans

After installing `tmt` from PyPI or other sources, you can do basic inspection of the tests/plans, even if the OS does
not support actual execution of the tests.

The `.fmf` files are simply yaml files with [folder structure inheritance][fmf-inheritance], and to view the actual
generated yaml-like content use:

```console
$ fmf show --path ./
```

:::{note}
The `--path` in `fmf` is equivalent to the `--root` in `tmt` and it points to the fmf tree root (the folder that
contains `.fmf/version` file) that you wish to introspect.
:::

To view the actual plans/tests use:

```console
$ tmt --root ./ tests show
$ tmt --root ../.distro plans show
```

### Executing tests

The tmt tests are primarily executed within the [testing-farm] environment through [packit] or other downstream like
Fedora-CI. But you can also interact with it [directly][tmt-cli], e.g.

```console
$ tmt --root ../.distro run --all plans --name full
```

:::{note}
The tests are currently assuming Spglib is already installed on the system. Standalone plans that compile Spglib as a
preparation step is yet to be designed.
:::

You can alter these executions by adding individual steps, e.g. if you want to change the provision step so that you run
in a container you can add a `provision` step in the middle like:

```console
$ tmt --root ../.distro run --all provision --how container --image=fedora:rawhide plans --name full
```

### Designing tmt tests/plans

At the bare minimum, you can simply design a test that executes the test-suite, see `pytest.fmf` for example. For more
complex testing we use [`beakerlib`], which basically provides a structured way of executing bash commands and check the
results and stdout. These tests are found as `<test>.sh`, the simplest reference being that of `ctest.sh`.

## Resources

- [`Googletest`]
- [`tmt`]
- [`beakerlib` manual]

[fmf-inheritance]: https://fmf.readthedocs.io/en/stable/features.html#inheritance
[packit]: https://packit.dev
[testing-farm]: https://docs.testing-farm.io/Testing%20Farm/0.1/index.html
[tmt-2557]: https://github.com/teemtee/tmt/pull/2557
[tmt-cli]: https://tmt.readthedocs.io/en/stable/stories/cli.html#command-line
[tmt-section]: #tmt-test-suite
[`beakerlib`]: https://github.com/beakerlib/beakerlib
[`beakerlib` manual]: https://beakerlib.readthedocs.io/en/latest/manual.html
[`ctest --build-and-test`]: https://cmake.org/cmake/help/latest/manual/ctest.1.html#build-and-test-mode
[`googletest`]: https://google.github.io/googletest
[`tmt`]: https://tmt.readthedocs.io/en/stable/index.html
