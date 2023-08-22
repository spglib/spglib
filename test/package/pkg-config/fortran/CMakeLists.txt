cmake_minimum_required(VERSION 3.11)
project(example_spglib LANGUAGES Fortran)

find_package(PkgConfig REQUIRED)
pkg_check_modules(spglib REQUIRED IMPORTED_TARGET spglib_f08)

add_executable(main main.F90)
target_link_libraries(main PRIVATE PkgConfig::spglib)
