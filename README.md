[![Version Badge](https://anaconda.org/conda-forge/spglib/badges/version.svg)](https://anaconda.org/conda-forge/spglib)
[![Downloads Badge](https://anaconda.org/conda-forge/spglib/badges/downloads.svg)](https://anaconda.org/conda-forge/spglib)
[![PyPI](https://img.shields.io/pypi/dm/spglib.svg?maxAge=2592000)](https://pypi.python.org/pypi/spglib)

# Spglib

C library for finding and handling crystal symmetries

<!-- TOC -->
* [Spglib](#spglib)
  * [How to compile the main C library](#how-to-compile-the-main-c-library)
  * [How install the python library](#how-install-the-python-library)
  * [How to run test](#how-to-run-test)
  * [Development](#development)
  * [Documentation](#documentation)
  * [Mailing list for questions](#mailing-list-for-questions)
<!-- TOC -->

## How to compile the main C library

The minimum configuration, build and install instructions are as follows:

```console
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
$ cmake --install .
```

For more details on configuration options and how to link your project check
the [cmake documentation](cmake/README.md). A minimal example project using
spglib is available at [example](example/README.md).

## How install the python library

Spglib is available on PyPi, so it can be installed as:

```console
$ pip install spglib
```

This python library will default to the system installed Spglib library detected,
specified via `LD_LIBRARY_PATH` on Linux or `DYLD_LIBRARY_PATH` on MacOS, otherwise
defaulting to a bundled version.

For more details and how to build the python project from source, see the
[python documentation](python/README.rst).

## How to run test

The C library tests are incorporated in the CMake projects and enabled by default.
To execute these tests, run the following command in the build directory

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
