# How to use spglib fortran wrapper

## Fortran wrapper

This fortran wrapper requires c-spglib
library. https://github.com/spglib/spglib/blob/develop/README.md gives
the simple instruction to compile with cmake. Here it is assumed that
the `PREFIX` of the library is located at `../build`.

In this directory that contains the fortran wrapper, to complie
`example_f08.f90` with gfortran, it is simply done by

```
% make -f Makefile_f08
```

What is done in this makefile is as follows:

```
% gfortran -c spglib_f08.f90
% gfortran example_f08.f90 spglib_f08.o ../build/lib/libsymspg.a -o example
```
