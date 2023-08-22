cmake_minimum_required(VERSION 3.6)
project(example_spglib LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

find_package(Spglib REQUIRED)

add_executable(example example.c)
add_executable(example_full example_full.c)

target_link_libraries(example PRIVATE Spglib::symspg)
target_link_libraries(example_full PRIVATE Spglib::symspg)
