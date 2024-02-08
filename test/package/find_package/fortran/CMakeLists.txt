cmake_minimum_required(VERSION 3.20...3.29)
project(example_spglib LANGUAGES Fortran)

find_package(Spglib REQUIRED COMPONENTS fortran)

add_executable(main main.F90)
target_link_libraries(main PRIVATE Spglib::fortran)
