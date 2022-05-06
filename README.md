[![Version Badge](https://anaconda.org/conda-forge/spglib/badges/version.svg)](https://anaconda.org/conda-forge/spglib)
[![Downloads Badge](https://anaconda.org/conda-forge/spglib/badges/downloads.svg)](https://anaconda.org/conda-forge/spglib)
[![PyPI](https://img.shields.io/pypi/dm/spglib.svg?maxAge=2592000)](https://pypi.python.org/pypi/spglib)

# Spglib

C library for finding and handling crystal symmetries

## How to compile with cmake

```bash
% mkdir _build
% cd _build
% cmake ..
% cmake --build .
% cmake --install (probably installed under /usr/local)
```

Or to install under the parent directory,

```bash
% mkdir _build
% cd _build
% cmake -DCMAKE_INSTALL_PREFIX=.. ..
% cmake --build .
% cmake --install . --prefix ..
```

A simple test is executed by

```bash
% mkdir _build
% cd _build
% cmake .. -DUSE_SANITIZER="Address" -DCMAKE_BUILD_TYPE="Debug"
% make check
```

## How to run test

Test is provided with the python interface.

```bash
% cd python
% pip install -e .
% pytest
```

## Development

The development of spglib is managed on the `develop` branch of github spglib
repository.

- Github issues is the place to discuss about spglib issues.
- Github pull request is the place to request merging source code.

## Documentation

Spglib user documetation is written using python sphinx. The source files are
stored in `doc` directory. Please see how to write the documentation at
`doc/README.md`.

## Mailing list for questions

Usual spglib questions should be sent to spglib mailing list
(https://sourceforge.net/p/spglib/mailman/).
