cmake_minimum_required(VERSION 3.25...3.29)
project(example_spglib LANGUAGES Fortran)

find_package(Spglib REQUIRED COMPONENTS fortran)

add_executable(example example.F90)
add_executable(example_full example_full.F90)

target_link_libraries(example PRIVATE Spglib::fortran)
target_link_libraries(example_full PRIVATE Spglib::fortran)
