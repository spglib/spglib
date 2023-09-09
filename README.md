[![Version Badge](https://anaconda.org/conda-forge/spglib/badges/version.svg)](https://anaconda.org/conda-forge/spglib)
[![Downloads Badge](https://anaconda.org/conda-forge/spglib/badges/downloads.svg)](https://anaconda.org/conda-forge/spglib)
[![PyPI](https://img.shields.io/pypi/dm/spglib.svg?maxAge=2592000)](https://pypi.python.org/pypi/spglib)

# Spglib

C library for finding and handling crystal symmetries

<!-- TOC -->

- [Spglib](#spglib)
  - [What does Spglib do?](#what-does-spglib-do)
  - [Requirements](#requirements)
  - [How to use Spglib](#how-to-use-spglib)
    - [How install the python library](#how-install-the-python-library)
    - [How to manually compile](#how-to-manually-compile)
  - [How to run test](#how-to-run-test)
  - [How to cite Spglib](#how-to-cite-spglib)
  - [Contributing](#contributing)
  - [Documentation](#documentation)

<!-- TOC -->

## What does Spglib do?

A crystal structure is given to a spglib function to obtain symmetry
information. `spg_get_dataset` is the function that returns most fruitful
space group information. This returns the result in the C-structure
`SpglibDataset`. We don't know the length of symmetry operations to be returned,
the memory spaces are allocated dynamically, which means we have to free them.
by `spg_free_dataset`. To tolerate tiny distortion of crystal structure,
it has to be specified when calling symmetry search function. The following
pages show the details.

- [Crystal structure format](https://spglib.github.io/spglib/variable.html)
- [Functions](https://spglib.github.io/spglib/api.html)
- [Spglib dataset](https://spglib.github.io/spglib/dataset.html)

```C
#include <assert.h>
#include "spglib.h"

int main(void) {
    SpglibDataset *dataset;
    // Wurtzite structure (P6_3mc)
    double lattice[3][3] = {
        {3.111, -1.5555, 0}, {0, 2.6942050311733885, 0}, {0, 0, 4.988}};
    double position[4][3] = {
        {1.0 / 3, 2.0 / 3, 0.0},
        {2.0 / 3, 1.0 / 3, 0.5},
        {1.0 / 3, 2.0 / 3, 0.6181},
        {2.0 / 3, 1.0 / 3, 0.1181},
    };
    int types[4] = {1, 1, 2, 2};
    int num_atom = 4;
    double symprec = 1e-5;

    // SplibDataset has to be freed after use.
    dataset = spg_get_dataset(lattice, position, types, num_atom, symprec);

    // Obtain data in SpglibDataset.
    // The space group number 186 corresponds to the wurtzite-type (P6_3mc).
    // See https://www.cryst.ehu.es/cgi-bin/cryst/programs/nph-table
    assert(dataset->spacegroup_number == 186);

    // Deallocate SpglibDataset, otherwise induce memory leak.
    spg_free_dataset(dataset);
}
```

## Requirements

- C standard: As of version 2.1.0, Spglib explicitly enforces a minimum standard of
  [C11](https://en.cppreference.com/w/c/11)

## How to use Spglib

We currently only officially support conda and pip packaging of spglib, with plans
to expand to FedoraProject in the near future. We are looking for additional
contributors to package on other linux distributions. For the systems that are not
yet officially supported please look at the instructions for [manual compilation](#how-to-manually-compile)

The main interface for spglib is the C api, but we also officially support Fortran
and Python interfaces. For a minimal example of how to use this library please
check the provided [examples](example/README.md)

### How install the python library

Spglib is available on PyPi, so it can be installed as:

```console
$ pip install spglib
```

Alternatively you can install the conda package that bundles the C and Fortran libraries

```console
$ conda install -c conda-forge spglib
```

This python library will default to the system installed Spglib library detected,
specified via `LD_LIBRARY_PATH` on Linux or `DYLD_LIBRARY_PATH` on MacOS, otherwise
defaulting to a bundled version.

For more details and how to build the python project from source, see the
[python documentation](python/README.rst).

### How to manually compile

The minimum configuration, build and install instructions are as follows:

```console
$ cmake . -B ./build
$ cmake --build ./build
$ cmake --install ./build
```

For more details on configuration options and how to link your project check
the [cmake documentation](cmake/README.md). A minimal example project using
spglib is available at [example](example/README.md).

## How to run test

The C library tests are incorporated in the CMake projects and enabled by default.
To execute these tests, run the following command with the appropriate path to the
build directory

```console
$ ctest --test-dir ./build
```

Additionally, there are python tests that cover more use-cases.
See the [README](python/README.rst) in the python folder for more details

## How to cite Spglib

If you use spglib in your research, please consider citing the following work:

- [Atsushi Togo and Isao Tanaka, Spglib: a software library for crystal symmetry search, arXiv:1808.01590 (2018)](https://arxiv.org/abs/1808.01590)

```
@misc{spglibv1,
  Author = {Atsushi Togo and Isao Tanaka},
  Title = {$\texttt{Spglib}$: a software library for crystal symmetry search},
  Eprint = {arXiv:1808.01590},
  howpublished = {\url{https://github.com/spglib/spglib}},
  year = {2018}
}
```

If you use functions to search for magnetic symmetry operations in spglib, please also consider citing the following work:

- [Kohei Shinohara, Atsushi Togo, and Isao Tanaka, Algorithms for magnetic symmetry operation search and identification of magnetic space group from magnetic crystal structure, Acta Cryst. A 79, 390-398 (2023)](https://onlinelibrary.wiley.com/iucr/doi/10.1107/S2053273323005016) (open access)

```
@article{spglibv2,
  author = "Shinohara, Kohei and Togo, Atsushi and Tanaka, Isao",
  title = "{Algorithms for magnetic symmetry operation search and identification of magnetic space group from magnetic crystal structure}",
  journal = "Acta Cryst. A",
  year = "2023",
  volume = "79",
  number = "5",
  pages = "390--398",
  month = "Sep",
  doi = {10.1107/S2053273323005016},
  url = {https://doi.org/10.1107/S2053273323005016},
}
```

## Contributing

We welcome any contribution from the core development of the library to documentation
improvements. See [Contributing.md](Contributing.md) for more details.

## Documentation

See [https://spglib.readthedocs.io/](https://spglib.readthedocs.io/) for further
documentation. See the [doc documentation](doc/README.md) for more information on
how to contribute to the documentation.
