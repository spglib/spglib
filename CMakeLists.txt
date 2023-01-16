cmake_minimum_required(VERSION 3.14)
project(spglib
        VERSION 2.1.0
        LANGUAGES C)

option(WITH_Fortran "enable f08 interface" OFF)
option(WITH_Python "build python buindings" OFF)
option(USE_OMP "Build with OpenMP support" OFF)
set(USE_SANITIZER "" CACHE STRING "Sanitizer used in compilation")

# Sanity check if project is imported
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    option(WITH_TESTS "build unit tests" ON)
else()
    option(WITH_TESTS "build unit tests" OFF)
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(CMAKE_MACOSX_RPATH 1)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  add_definitions(-DSPGDEBUG -DSPGWARNING)
endif()

# Windows setup, make sure there is a .lib file to link to
if (SKBUILD AND WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if (USE_SANITIZER STREQUAL "Address")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
elseif (USE_SANITIZER STREQUAL "Thread")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=thread")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
elseif (USE_SANITIZER STREQUAL "Undefined")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
elseif (USE_SANITIZER STREQUAL "Leak")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=leak")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")
elseif (USE_SANITIZER STREQUAL "Memory")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=memory")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=memory")
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (USE_OMP)
  find_package(OpenMP REQUIRED)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
endif()

# Version numbers
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/src/version.h)
set(soserial "1")

# Source code
include_directories(${PROJECT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR}/src)
set(SOURCES ${PROJECT_SOURCE_DIR}/src/arithmetic.c
            ${PROJECT_SOURCE_DIR}/src/cell.c
            ${PROJECT_SOURCE_DIR}/src/debug.c
            ${PROJECT_SOURCE_DIR}/src/delaunay.c
            ${PROJECT_SOURCE_DIR}/src/determination.c
            ${PROJECT_SOURCE_DIR}/src/hall_symbol.c
            ${PROJECT_SOURCE_DIR}/src/kgrid.c
            ${PROJECT_SOURCE_DIR}/src/kpoint.c
            ${PROJECT_SOURCE_DIR}/src/magnetic_spacegroup.c
            ${PROJECT_SOURCE_DIR}/src/mathfunc.c
            ${PROJECT_SOURCE_DIR}/src/msg_database.c
            ${PROJECT_SOURCE_DIR}/src/niggli.c
            ${PROJECT_SOURCE_DIR}/src/overlap.c
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

# avoid One Definition Rule problems
# only available since 3.16.0
# VERSION_GREATER_EQUAL only available since 3.7.0, so let's do it manually
if (NOT CMAKE_MAJOR_VERSION LESS "3")
  if (CMAKE_MAJOR_VERSION GREATER "3" OR NOT CMAKE_MINOR_VERSION LESS "16")
    set_source_files_properties(
        ${SOURCES} PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON
    )
  endif()
endif()

# Shared library
add_library(symspg SHARED ${SOURCES})
target_include_directories(symspg PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

if(NOT MSVC)
  target_link_libraries(symspg PUBLIC m)
endif()
set_property(TARGET symspg PROPERTY VERSION ${PROJECT_VERSION})
set_property(TARGET symspg PROPERTY SOVERSION ${soserial})

# Static link library
add_library(symspg_static STATIC ${SOURCES})
set_property(TARGET symspg_static PROPERTY VERSION ${PROJECT_VERSION})
set_property(TARGET symspg_static PROPERTY OUTPUT_NAME symspg)

if (WITH_Fortran)
  enable_language(Fortran)
  add_subdirectory(fortran)
endif()
if (WITH_Python)
    add_subdirectory(python)
endif()
if (WITH_TESTS)
    add_subdirectory(test)
endif()

# Installation
# TODO: Temporarily disable normal cmake install targets when using scikit-build-core
#  Check how bundled cmake searches CMAKE_MODULE_DIR and add accordingly
#  (use SKBUILD_PLATLIB_DIR for install root)
if (NOT SKBUILD)
    # Main target
    install(TARGETS symspg
            EXPORT spglibTargets
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(TARGETS symspg_static
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(FILES ${PROJECT_SOURCE_DIR}/src/spglib.h
            ${PROJECT_SOURCE_DIR}/fortran/spglib_f08.f90
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})


    # pkg-config files
    configure_file(spglib.pc.in ${CMAKE_CURRENT_BINARY_DIR}/spglib.pc @ONLY)
    install(FILES  ${CMAKE_CURRENT_BINARY_DIR}/spglib.pc
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
            COMPONENT pkgconfig)

    # cmake export files
    write_basic_package_version_file(
            ${CMAKE_CURRENT_BINARY_DIR}/spglibConfigVersion.cmake
            VERSION ${PROJECT_VERSION}
            COMPATIBILITY SameMajorVersion)
    configure_package_config_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/cmake/spglibConfig.cmake.in
            ${CMAKE_CURRENT_BINARY_DIR}/spglibConfig.cmake
            INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/spglib)
    install(
            FILES ${CMAKE_CURRENT_BINARY_DIR}/spglibConfigVersion.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/spglibConfig.cmake
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/spglib)
    install(EXPORT spglibTargets
            FILE spglibTargets.cmake
            NAMESPACE spglib::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/spglib)
    export(EXPORT spglibTargets
            FILE ${CMAKE_CURRENT_BINARY_DIR}/cmake/spglibTargets.cmake
            NAMESPACE spglib::)
endif ()