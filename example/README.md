# How to use spglib example files

This is the README file for spglib examples for C and Fortran.

## Examples of using spglib in a cmake project

Here we provide a cmake example for both C and fortran projects linking
with both native cmake and pkg-config installed on the system. Alternatively,
you can download spglib automatically using
[`FetchContent`](https://cmake.org/cmake/help/latest/module/FetchContent.html),
and compile it with specific options, e.g.:

```cmake
cmake_minimum_required(VERSION 3.11)
project(example_spglib_FetchContent
		LANGUAGES C)

FetchContent_Declare(
		Spglib
		GIT_REPOSITORY https://github.com/spglib/spglib
)

set(SPGLIB_USE_OMP ON)

FetchContent_MakeAvailable(Spglib)

add_executable(example_c example.c)
target_link_libraries(example_c PUBLIC
		Spglib::symspg)
```

## Examples of linking to spglib manually

To manually compile `example.c`:

```console
$ gcc example.c -I[include directory] -L[library directory] -lsymspg -lm
```

where the `[include directory]` and `[library directory]` can be obtained
via `pkg-config --cflags --libs spglib`.
