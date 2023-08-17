cmake_minimum_required(VERSION 3.11)
project(example_spglib LANGUAGES C)

find_package(Spglib REQUIRED)

add_executable(main main.c)
target_link_libraries(main PRIVATE Spglib::symspg)
