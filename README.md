[![Version Badge](https://anaconda.org/conda-forge/spglib/badges/version.svg)](https://anaconda.org/conda-forge/spglib)
[![Downloads Badge](https://anaconda.org/conda-forge/spglib/badges/downloads.svg)](https://anaconda.org/conda-forge/spglib)
[![PyPI](https://img.shields.io/pypi/dm/spglib.svg?maxAge=2592000)](https://pypi.python.org/pypi/spglib)

# Spglib

C library for finding and handling crystal symmetries

## How to link spglib C library to your project

We currently support two ways of linking this project: using native `cmake` or via `pkg-config`.
The native cmake package and namespace are `spglib`. The cmake targets and pkg-config files
exported are:

|     CMake target     | pkg-config file | Description     |
|:--------------------:|:---------------:|:----------------|
|   `spglib::symspg`   |    `spglib`     | Main C library  |
| `spglib::spglib_f08` |  `spglib_f08`   | Fortran wrapper |

An example `CMakeLists.txt` to link via native `cmake`:
```cmake
project(foo)

find_package(spglib REQUIRED)

add_library(foo_library)
target_link_libraries(foo_library PRIVATE spglib::symspg)
```

See [example/CMakeLists.txt](example/CMakeLists.txt) for a more complete example.

For other wrappers see the corresponding readme in the respective directories.

## How to compile main C library

```console
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build ${CMAKE_OPTIONS} .
$ cmake --install .
```
Replace the variable `CMAKE_OPTIONS` with appropriate options. For a list of Spglib
specific (marked as **bold**) and other useful native cmake options, see
[CMake Options](#cmake-options).

### CMake Options

Keep in mind that in order to pass cmake options they have to be prefixed with `-D`,
e.g. `-DWITH_Fortran=ON`.

| Option               |               Default               | Description                                                                        |
|:---------------------|:-----------------------------------:|:-----------------------------------------------------------------------------------|
| **WITH_Fortran**     |                 OFF                 | Build Fortran API                                                                  |
| **WITH_Python**      |                 OFF                 | Build Python API                                                                   |
| **WITH_TESTS**       |                 ON                  | Include basic tests                                                                |
| **USE_OMP**          |                 OFF                 | Use OpenMPI                                                                        |
| **USE_SANITIZER**    |                 ""                  | Specify a sanitizer to compile with<br/> See `CMakeLists.txt` for supported values |
| CMAKE_INSTALL_PREFIX | OS specific<br/>(e.g. `/usr/local`) | Location where to install built project                                            |
| BUILD_SHARED_LIBS    |                 ON                  | Whether to build shared or statically linked libraries<br/>(Currently unsupported) |


## How to compile python API

See [python documentation](python/README.rst) for compilation instructions.

Note that the python library uses the system default Spglib library (controllable
by `LD_LIBRARY_PATH` on Linux or `DYLD_LIBRARY_PATH` on MacOS). If it is not found,
it will default to a bundled version compiled with minimum options.

## How to run test

The C library tests are incorporated in the CMake projects and enabled with the
option `WITH_TESTS`. To execute these tests, runt the following command in the
build directory
```console
$ cd test
$ ctest
```

Additionally, there are python tests that cover more use-cases.
See the [README](python/README.rst) in the python folder for more details

## Development

See [Contribution.md](Contribution.md).

## Documentation

Spglib user documetation is written using python sphinx. The source files are
stored in `doc` directory. Please see how to write the documentation at
`doc/README.md`.

## Mailing list for questions

Usual spglib questions should be sent to spglib mailing list
(https://sourceforge.net/p/spglib/mailman/).
