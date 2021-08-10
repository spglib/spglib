# How to use spglib fortran wrapper

## Fortran wrapper

This fortran wrapper requires c-spglib
library. https://github.com/spglib/spglib/blob/develop/README.md gives
simple instructions to compile with cmake. Here it is assumed that
the `PREFIX` of the library is located at `../build`.

In this directory that contains the fortran wrapper, to complie
`example_f08.f90` with gfortran, it is simply done by

```
% make
```

