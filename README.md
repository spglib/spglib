spglib
======
[![Binstar Badge](https://anaconda.org/jochym/spglib/badges/version.svg)](https://anaconda.org/jochym/spglib)
[![Binstar Badge](https://anaconda.org/jochym/spglib/badges/downloads.svg)](https://anaconda.org/jochym/spglib)
[![Binstar Badge](https://anaconda.org/jochym/spglib/badges/installer/conda.svg)](https://conda.anaconda.org/jochym/spglib)
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
