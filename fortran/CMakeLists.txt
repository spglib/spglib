cmake_minimum_required(VERSION 3.25...3.29)

#[=============================================================================[
#                           Basic project definition                           #
]=============================================================================]

list(APPEND CMAKE_MESSAGE_CONTEXT Fortran)

if (NOT COMMAND dynamic_version)
    include(../cmake/DynamicVersion.cmake)
endif ()
# Get dynamic version and save it to PROJECT_VERSION
dynamic_version(
        PROJECT_PREFIX Spglib_Fortran_
        PROJECT_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/..
        FALLBACK_VERSION 0.0.0
        OUTPUT_VERSION_FULL Spglib_Fortran_VERSION_FULL
        OUTPUT_COMMIT Spglib_Fortran_COMMIT
)

project(Spglib_Fortran
        VERSION ${PROJECT_VERSION}
        LANGUAGES Fortran
)

#[=============================================================================[
#                                   Options                                   #
]=============================================================================]

option(SPGLIB_INSTALL "Spglib: Install project" ${PROJECT_IS_TOP_LEVEL})
option(SPGLIB_SHARED_LIBS "Spglib: Build as a shared library" ${PROJECT_IS_TOP_LEVEL})
if (SPGLIB_INSTALL)
    # CMake does not properly support fortran module installation paths.
    # Adapting the standard from fortran-stdlib
    # https://gitlab.kitware.com/cmake/cmake/-/issues/19608
    # https://discourse.cmake.org/t/api-design-c-modules-source-listings-and-interface-properties/5389/14
    cmake_path(APPEND CMAKE_INSTALL_INCLUDEDIR ${PROJECT_NAME} "${CMAKE_Fortran_COMPILER_ID}-${CMAKE_Fortran_COMPILER_VERSION}"
            OUTPUT_VARIABLE _DEFAULT_CMAKE_INSTALL_MODULEDIR)
    set(CMAKE_INSTALL_MODULEDIR ${_DEFAULT_CMAKE_INSTALL_MODULEDIR}
            CACHE STRING
            "Fortran module installation path (Not a cmake native variable)"
    )
endif ()

#[=============================================================================[
#                            Project configuration                            #
]=============================================================================]

include(GNUInstallDirs)
include(../cmake/PackageCompsHelper.cmake)

# Define basic parameters
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()
set(BUILD_SHARED_LIBS ${SPGLIB_SHARED_LIBS})
if (BUILD_SHARED_LIBS)
    set(Spglib_LIB_TYPE "shared")
else ()
    set(Spglib_LIB_TYPE "static")
endif ()

if (NOT CMAKE_Fortran_MODULE_DIRECTORY)
    set(CMAKE_Fortran_MODULE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/fortran_mods)
endif ()

#[=============================================================================[
#                                Public targets                                #
]=============================================================================]

add_library(Spglib_fortran)
add_library(Spglib::fortran ALIAS Spglib_fortran)

#[=============================================================================[
#                              External packages                              #
]=============================================================================]

# Get Spglib if it's run as stand-alone project
if (NOT TARGET Spglib::symspg)
    find_package(Spglib CONFIG)
    if (NOT Spglib_FOUND)
        message(STATUS "Using bundled spglib sources")
        add_subdirectory(${PROJECT_SOURCE_DIR}/.. _deps/spglib-build)
    endif ()
endif ()

#[=============================================================================[
#                               Main definition                               #
]=============================================================================]

# Define main target
set_target_properties(Spglib_fortran PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        EXPORT_NAME fortran_mod
        OUTPUT_NAME spglib_f08
)

# This target is only used in the SpglibConfig.cmake
add_library(Spglib_fortran_include INTERFACE)
set_target_properties(Spglib_fortran_include PROPERTIES
        EXPORT_NAME fortran_include
)

# Configure Fortran wrapper
configure_file(spglib_version.f90.in spglib_version.f90)

target_sources(Spglib_fortran_include INTERFACE
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/spglib_f08.F90>"
)

# Concatenate the contents of the fortran file so it can be compiled from source
execute_process(COMMAND ${CMAKE_COMMAND} -E cat
        ${CMAKE_CURRENT_BINARY_DIR}/spglib_version.f90
        ${CMAKE_CURRENT_SOURCE_DIR}/spglib_f08.F90
        OUTPUT_VARIABLE spglib_f08_concat
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/spglib_f08.F90 ${spglib_f08_concat})

# Use the separate files for the project itself so that they are properly re-built
target_sources(Spglib_fortran PRIVATE
        spglib_f08.F90
        ${CMAKE_CURRENT_BINARY_DIR}/spglib_version.f90
)

set_target_properties(Spglib_fortran_include PROPERTIES
        PUBLIC_HEADER ${CMAKE_CURRENT_BINARY_DIR}/spglib_f08.F90
)
target_include_directories(Spglib_fortran PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_Fortran_MODULE_DIRECTORY}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_MODULEDIR}>"
)
target_link_libraries(Spglib_fortran PUBLIC Spglib::symspg)
target_link_libraries(Spglib_fortran_include INTERFACE Spglib::symspg)

#[=============================================================================[
#                              Install or Export                              #
]=============================================================================]

# Copy PackageCompsHelper to make it available for build import
if (Spglib_BINARY_DIR)
    configure_file(try_compile.f90 ${Spglib_BINARY_DIR}/try_compile.f90 COPYONLY)
endif ()

# Install
if (SPGLIB_INSTALL)
    cmake_path(IS_ABSOLUTE CMAKE_INSTALL_MODULEDIR _is_absolute)
    if (_is_absolute)
        set(CMAKE_INSTALL_FULL_MODULEDIR ${CMAKE_INSTALL_MODULEDIR})
    else ()
        cmake_path(APPEND CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_MODULEDIR}
                OUTPUT_VARIABLE CMAKE_INSTALL_FULL_MODULEDIR)
    endif ()
    install(TARGETS Spglib_fortran Spglib_fortran_include
            EXPORT SpglibTargets-fortran
    )
    export_components(
            PROJECT Spglib
            COMPONENT fortran
            LIB_TYPE ${Spglib_LIB_TYPE}
    )
    install(FILES try_compile.f90
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Spglib
    )
    install(FILES ${CMAKE_Fortran_MODULE_DIRECTORY}/spglib_f08.mod
            DESTINATION ${CMAKE_INSTALL_MODULEDIR}
    )
    configure_file(spglib_f08.pc.in spglib_f08.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/spglib_f08.pc
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    )
endif ()

# Make project available for FetchContent
if (NOT PROJECT_IS_TOP_LEVEL)
    # Propagate variables
    return(PROPAGATE
            Spglib_Fortran_VERSION
            Spglib_Fortran_VERSION_MAJOR
            Spglib_Fortran_VERSION_MINOR
            Spglib_Fortran_VERSION_PATCH
            Spglib_Fortran_VERSION_TWEAK
            Spglib_Fortran_VERSION_FULL
            Spglib_Fortran_COMMIT
            Spglib_LIB_TYPE
    )
endif ()
