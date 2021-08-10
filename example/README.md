# How to use spglib example files

This is the README file for spglib examples for C, Fortran, and
a linear tetrahedron method example. `tetrahedron_method.c`and
`tetrahedron_method.h` are those distributed under kspclib
(https://github.com/atztogo/kspclib).

## Examples of spglib functions in C

To compile `example.c`:

```
% gcc example.c -I[include directory] -L[library directory] -lsymspg -lm
```

## Linear tetrahedron method

`tetrahedron_method.c` relies only `kgrid.c` and `kgrid.h`. Therefore
in the example in `tetrahedron.c`, although
`spg_get_ir_reciprocal_mesh` is used to obtain irreducible k-points,
this is not essential for running tetrahedron method packed with the
spglib distribution. To compile `tetrahedron.c`:

```
% gcc tetrahedron.c [source directory]/tetrahedron_method.c -I[include directory] -L[library directory] -lsymspg
```

`tetrahedron.c` is written to work with OpenMP. To compile with
`-lgomp -fopenmp`, the tetrahedon method calculation is distributed
into CPU cores.
