[![Version Badge](https://anaconda.org/conda-forge/spglib/badges/version.svg)](https://anaconda.org/conda-forge/spglib)
[![Downloads Badge](https://anaconda.org/conda-forge/spglib/badges/downloads.svg)](https://anaconda.org/conda-forge/spglib)
[![PyPI](https://img.shields.io/pypi/dm/spglib.svg?maxAge=2592000)](https://pypi.python.org/pypi/spglib)
[![Build Status](https://travis-ci.org/atztogo/spglib.svg?branch=master)](https://travis-ci.org/atztogo/spglib)

# Spglib

C library for finding and handling crystal symmetries

## How to compile with cmake

```
% mkdir _build
% cd _build
% cmake ..
% make
% make install (probably installed under /usr/local)
```

Or to install under /some/where

```
% mkdir _build
% cd _build
% cmake -DCMAKE_INSTALL_PREFIX="" ..
% make
% make DESTDIR=/some/where install
```

A simple test is executed by

```
% make check
```

## Mailing list for questions

Usual spglib questions should be sent to spglib mailing list (https://sourceforge.net/s/spglib/mailman/).

## Development

The development of spglib is managed on the `develop` branch of github spglib repository.

* Github issues is the place to discuss about spglib issues.
* Github pull request is the place to request merging source code.

## Documentation

Spglib user documetation is written using python sphinx. The source files are stored in `doc` directory. Please see how to write the documentation at `doc/README.md`.
