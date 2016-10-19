spglib
======
[![Version Badge](https://anaconda.org/jochym/spglib/badges/version.svg)](https://anaconda.org/jochym/spglib)
[![Downloads Badge](https://anaconda.org/jochym/spglib/badges/downloads.svg)](https://anaconda.org/jochym/spglib)
[![PyPI](https://img.shields.io/pypi/dm/spglib.svg?maxAge=2592000)](https://pypi.python.org/pypi/spglib)
[![Build Status](https://travis-ci.org/jochym/spglib.svg?branch=master)](https://travis-ci.org/jochym/spglib)

C library for finding and handling crystal symmetries

How to compile
---------------

    % aclocal
    % autoheader
    % libtoolize # or glibtoolize with macport etc
    % touch INSTALL NEWS README AUTHORS
    % automake -acf
    % autoconf
    % ./configure
