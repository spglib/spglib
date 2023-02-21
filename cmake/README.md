# CMake Integration

<!-- TOC -->

- [CMake Integration](#cmake-integration)
  - [How to configure the spglib library](#how-to-configure-the-spglib-library)
  - [CMake Options](#cmake-options)
  - [How to link spglib C library to your project](#how-to-link-spglib-c-library-to-your-project)
    - [Linking against locally installed spglib](#linking-against-locally-installed-spglib)
    - [Downloading and using specific versions](#downloading-and-using-specific-versions)

<!-- TOC -->

## How to configure the spglib library

The configuration is done by passing cmake options and environment variables at the
configuration stage, i.e.:

```console
$ cmake . -B build  ${CMAKE_OPTIONS}
```

Replace the variable `CMAKE_OPTIONS` with appropriate built-in CMake options, e.g.
`-DCMAKE_C_COMPILER=gcc`, and Spglib specific options. For a list of useful options
check [CMake Options](#cmake-options).

## CMake Options

All spglib specific options are prefixed with `SPGLIB_` in order to avoid clashes
and allow the project to be included via `FetchContent`.
Keep in mind that in order to pass cmake options they have to be prefixed with `-D`,
e.g. `-DSPGLIB_WITH_Fortran=ON`.

| Option                   |               Default               | Description                                                                        |
| :----------------------- | :---------------------------------: | :--------------------------------------------------------------------------------- |
| **SPGLIB_SHARED_LIBS**   |                 ON                  | Build spglib as a shared library. Turn off to build static.                        |
| **SPGLIB_WITH_Fortran**  |                 OFF                 | Build Fortran API                                                                  |
| **SPGLIB_WITH_Python**   |                 OFF                 | Build Python API                                                                   |
| **SPGLIB_WITH_TESTS**    |                 ON                  | Include basic tests                                                                |
| **SPGLIB_USE_OMP**       |                 OFF                 | Use OpenMPI                                                                        |
| **SPGLIB_USE_SANITIZER** |                 ""                  | Specify a sanitizer to compile with<br/> e.g. `address`                            |
| CMAKE_INSTALL_PREFIX     | OS specific<br/>(e.g. `/usr/local`) | Location where to install built project                                            |
| BUILD_SHARED_LIBS        |                 ON                  | Whether to build shared or statically linked libraries<br/>(Currently unsupported) |

## How to link spglib C library to your project

### Linking against locally installed spglib

We currently support two ways of linking this project: using native `cmake` or via `pkg-config`.
The native cmake package and namespace are `Spglib`. The cmake targets and pkg-config files
exported are:

|   CMake target    | pkg-config file | Description                                                      |
| :---------------: | :-------------: | :--------------------------------------------------------------- |
| `Spglib::symspg`  |    `spglib`     | Main C library                                                   |
|   `Spglib::omp`   |       ---       | Equivalent to `Spglib::symspg` <br/> Guaranteed `OpenMP` support |
| `Spglib::fortran` |  `spglib_f08`   | Fortran wrapper                                                  |

An example `CMakeLists.txt` to link via native `cmake`:

```cmake
project(foo)

find_package(Spglib REQUIRED)

add_library(foo_library)
target_link_libraries(foo_library PRIVATE Spglib::symspg)
```

Additionally, we provide convenient components and variables to control and check the
spglib library:

| Component |  Imported target  | CMake variable    | Description                             |
| :-------: | :---------------: | ----------------- | --------------------------------------- |
| `static`  |        ---        | `Spglib_LIB_TYPE` | Link against statically built libraries |
| `shared`  |        ---        | `Spglib_LIB_TYPE` | Link against shared libraries           |
| `fortran` | `Spglib::fortran` | `Spglib_Fortran`  | Check and include Fortran target        |
|   `omp`   |   `Spglib::omp`   | `Spglib_OMP`      | Check and include OpenMP target         |

See [example/CMakeLists.txt](../example/CMakeLists.txt) for a more complete example.
Note that this library is built as a shared library, unless it is imported via
`FetchContent` or equivalent approaches. This can be controlled via `SPGLIB_SHARED_LIBS`.

### Downloading and using specific versions

Alternatively, you can download spglib automatically using
[`FetchContent`](https://cmake.org/cmake/help/latest/module/FetchContent.html),
and compile it with specific options in [CMake Options](#cmake-options), e.g.:

```cmake
project(foo)

FetchContent_Declare(
	Spglib
	GIT_REPOSITORY https://github.com/spglib/spglib
	GIT_TAG main
)

set(SPGLIB_USE_OMP ON)

FetchContent_MakeAvailable(Spglib)

add_library(foo_library)
target_link_libraries(foo_library PRIVATE Spglib::symspg)
```
