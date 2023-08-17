cmake_minimum_required(VERSION 3.6)
project(example_spglib LANGUAGES C)

find_package(Spglib REQUIRED)

add_executable(example example.c)
add_executable(example_full example_full.c)

target_link_libraries(example PRIVATE Spglib::symspg)
target_link_libraries(example_full PRIVATE Spglib::symspg)
