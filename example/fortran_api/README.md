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
where the `[include directory]` and `[library directory]` can be obtained by
```
PKG_CONFIG_PATH=/path/to/spglib_f08.pc pkg-config --cflags --libs spglib_f08
```
