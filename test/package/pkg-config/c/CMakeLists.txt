cmake_minimum_required(VERSION 3.20...3.29)
project(example_spglib LANGUAGES C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(spglib REQUIRED IMPORTED_TARGET spglib)

add_executable(main main.c)
target_link_libraries(main PRIVATE PkgConfig::spglib)
