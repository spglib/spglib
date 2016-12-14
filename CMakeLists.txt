cmake_minimum_required(VERSION 2.4)
project(spglib C)
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_C_FLAGS_RELEASE "-Wall -O2")
set(CMAKE_C_FLAGS_DEBUG "-g -DSPGDEBUG ")
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Version numbers
file(READ ${PROJECT_SOURCE_DIR}/src/version.h version_file)
string(REGEX MATCH "SPGLIB_MAJOR_VERSION ([0-9]+)" spglib_major_version ${version_file})
set(spglib_major_version ${CMAKE_MATCH_1})
string(REGEX MATCH "SPGLIB_MINOR_VERSION ([0-9]+)" spglib_minor_version ${version_file})
set(spglib_minor_version ${CMAKE_MATCH_1})
string(REGEX MATCH "SPGLIB_MICRO_VERSION ([0-9]+)" spglib_micro_version ${version_file})
set(spglib_micro_version ${CMAKE_MATCH_1})
set(serial "${spglib_major_version}.${spglib_minor_version}.${spglib_micro_version}")
set(soserial "1")

# Source code
include_directories("${PROJECT_SOURCE_DIR}/src")
set(SOURCES ${PROJECT_SOURCE_DIR}/src/arithmetic.c
            ${PROJECT_SOURCE_DIR}/src/cell.c
            ${PROJECT_SOURCE_DIR}/src/debug.c
            ${PROJECT_SOURCE_DIR}/src/delaunay.c
            ${PROJECT_SOURCE_DIR}/src/hall_symbol.c
            ${PROJECT_SOURCE_DIR}/src/kgrid.c
            ${PROJECT_SOURCE_DIR}/src/kpoint.c
            ${PROJECT_SOURCE_DIR}/src/mathfunc.c
            ${PROJECT_SOURCE_DIR}/src/niggli.c
            ${PROJECT_SOURCE_DIR}/src/pointgroup.c
            ${PROJECT_SOURCE_DIR}/src/primitive.c
            ${PROJECT_SOURCE_DIR}/src/refinement.c
            ${PROJECT_SOURCE_DIR}/src/site_symmetry.c
            ${PROJECT_SOURCE_DIR}/src/sitesym_database.c
            ${PROJECT_SOURCE_DIR}/src/spacegroup.c
            ${PROJECT_SOURCE_DIR}/src/spg_database.c
            ${PROJECT_SOURCE_DIR}/src/spglib.c
            ${PROJECT_SOURCE_DIR}/src/spin.c
            ${PROJECT_SOURCE_DIR}/src/symmetry.c)

# Shared library
add_library(symspg SHARED ${SOURCES})
set_property(TARGET symspg PROPERTY VERSION ${serial})
set_property(TARGET symspg PROPERTY SOVERSION ${soserial})
install(TARGETS symspg LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/lib)

# Static link library
add_library(symspg_static STATIC ${SOURCES})
set_property(TARGET symspg_static PROPERTY VERSION ${serial})
set_property(TARGET symspg_static PROPERTY SOVERSION ${soserial})
set_property(TARGET symspg_static PROPERTY OUTPUT_NAME symspg)
install(TARGETS symspg_static ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/lib)

# Header file
install(FILES ${PROJECT_SOURCE_DIR}/src/spglib.h DESTINATION ${PROJECT_SOURCE_DIR}/include)

# make check
enable_testing()
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})
set(CMAKE_CTEST_COMMAND ctest -V)
add_executable(spglibtest EXCLUDE_FROM_ALL ${PROJECT_SOURCE_DIR}/src/test.c ${SOURCES})
add_test(spglibtest spglibtest)
add_dependencies(check spglibtest)
