spglib
======
[![Binstar Badge](https://binstar.org/jochym/spglib/badges/version.svg)](https://binstar.org/jochym/spglib)
[![Binstar Badge](https://binstar.org/jochym/spglib/badges/downloads.svg)](https://binstar.org/jochym/spglib)
[![Binstar Badge](https://binstar.org/jochym/spglib/badges/installer/conda.svg)](https://conda.binstar.org/jochym/spglib)
[![Build Status](https://travis-ci.org/jochym/spglib.svg?branch=master)](https://travis-ci.org/jochym/spglib)

C library for finding and handling crystal symmetries

How to compile
---------------

    % aclocal
    % autoheader
    % libtoolize # or glibtoolize on mac
    % touch INSTALL NEWS README AUTHORS
    % automake -acf
    % autoconf
    % ./configure
