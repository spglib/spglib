# Fortran example

This is an example of how to use Spglib through the Fortran api

## Examples of using spglib in a cmake project

See the local [`CMakeLists.txt`](CMakeLists.txt) file for how to use
spglib in a cmake project, using either native cmake options or pkg-config.

## Examples of linking to spglib manually

To manually compile `example.f90`:

```console
gfortran example.f90 -I[include directory] -L[library directory] -lsymspg
```

Here it is assumed that spglib library was built by

```console
cmake -B build -DCMAKE_INSTALL_PREFIX=. -DSPGLIB_WITH_Fortran=on -DSPGLIB_SHARED_LIBS=off -DSPGLIB_WITH_TESTS=off .
cmake --build build
cmake --install build
```
