spglib
======
[![Version Badge](https://anaconda.org/atztogo/spglib/badges/version.svg)](https://anaconda.org/atztogo/spglib)
[![Downloads Badge](https://anaconda.org/atztogo/spglib/badges/downloads.svg)](https://anaconda.org/atztogo/spglib)
[![PyPI](https://img.shields.io/pypi/dm/spglib.svg?maxAge=2592000)](https://pypi.python.org/pypi/spglib)
[![Build Status](https://travis-ci.org/atztogo/spglib.svg?branch=master)](https://travis-ci.org/atztogo/spglib)

C library for finding and handling crystal symmetries

How to compile with autotools
-------------------------------

    % aclocal
    % autoheader
    % libtoolize # or glibtoolize with macport etc
    % touch INSTALL NEWS README AUTHORS
    % automake -acf
    % autoconf
    % ./configure

How to compile with cmake
--------------------------

    % mkdir _build
    % cd _build
    % cmake ..
    % make
    % make install (probably installed under /usr/local)

Or to install under /some/where

    % mkdir _build
    % cd _build
    % cmake -DCMAKE_INSTALL_PREFIX="" ..
    % make
    % make DESTDIR=/some/where install
