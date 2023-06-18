cmake_minimum_required(VERSION 3.15)
# CMake version compatibility
# TODO: Remove when cmake 3.25 is commonly distributed
if (POLICY CMP0140)
    cmake_policy(SET CMP0140 NEW)
endif ()

#[=============================================================================[
#                           Basic project definition                           #
]=============================================================================]

list(APPEND CMAKE_MESSAGE_CONTEXT CXX)

if (NOT COMMAND dynamic_version)
    include(../cmake/DynamicVersion.cmake)
endif ()
# Get dynamic version and save it to PROJECT_VERSION
dynamic_version(
        PROJECT_PREFIX Spglib_CXX_
        PROJECT_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/..
        FALLBACK_VERSION 0.0.0
        OUTPUT_VERSION_FULL Spglib_CXX_VERSION_FULL
        OUTPUT_COMMIT Spglib_CXX_COMMIT
)

project(Spglib_CXX
        VERSION ${PROJECT_VERSION}
        LANGUAGES CXX
)
# Back-porting to PROJECT_IS_TOP_LEVEL to older cmake
# TODO: Remove when requiring cmake >= 3.21
if (NOT DEFINED Spglib_CXX_IS_TOP_LEVEL)
    if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
        set(PROJECT_IS_TOP_LEVEL ON)
    else ()
        set(PROJECT_IS_TOP_LEVEL OFF)
    endif ()
endif ()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#[=============================================================================[
#                                   Options                                   #
]=============================================================================]

option(SPGLIB_INSTALL "Spglib: Install project" ${PROJECT_IS_TOP_LEVEL})
option(SPGLIB_SHARED_LIBS "Spglib: Build as a shared library" ${PROJECT_IS_TOP_LEVEL})

#[=============================================================================[
#                            Project configuration                            #
]=============================================================================]

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
#                              External packages                              #
]=============================================================================]

set(external_libs)
include(FetchContent)

# Get Spglib if it's run as stand-alone project
if (NOT Spglib_IS_TOP_LEVEL)
    FetchContent_Declare(Spglib
            GIT_REPOSITORY https://github.com/spglib/spglib
            GIT_TAG develop
            FIND_PACKAGE_ARGS CONFIG
    )
    list(APPEND external_libs Spglib)
endif ()
FetchContent_MakeAvailable(${external_libs})

#[=============================================================================[
#                               Main definition                               #
]=============================================================================]


# Main c++ wrapper
add_library(Spglib_cxx)
add_library(Spglib::cxx ALIAS Spglib_cxx)
set_target_properties(Spglib_cxx PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        EXPORT_NAME cxx
        OUTPUT_NAME spglib_cpp
)

# Configure c++ wrapper
configure_file(version.cpp.in version.cpp)
target_sources(Spglib_cxx PRIVATE
        spglib.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/version.cpp
)
target_include_directories(Spglib_cxx PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
set_target_properties(Spglib_cxx PROPERTIES
        PUBLIC_HEADER spglib.hpp
)
target_link_libraries(Spglib_cxx PUBLIC Spglib::symspg)

#[=============================================================================[
#                              Install or Export                              #
]=============================================================================]

# Install
if (SPGLIB_INSTALL)
    install(TARGETS Spglib_cxx
            EXPORT SpglibTargets-cxx
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Spglib_Runtime NAMELINK_COMPONENT Spglib_Development
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Spglib_Development
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT Spglib_Development
    )
    export_components(
            PROJECT Spglib
            COMPONENT cxx
            LIB_TYPE ${SPGLIB_LIB_TYPE}
    )
endif ()

# Make project available for FetchContent
if (NOT PROJECT_IS_TOP_LEVEL)
    # Propagate variables
    if (CMAKE_VERSION VERSION_LESS 3.25)
        # TODO: Remove when cmake 3.25 is commonly distributed
        set(Spglib_CXX_VERSION ${Spglib_CXX_VERSION} PARENT_SCOPE)
        set(Spglib_CXX_VERSION_MAJOR ${Spglib_CXX_VERSION_MAJOR} PARENT_SCOPE)
        set(Spglib_CXX_VERSION_MINOR ${Spglib_CXX_VERSION_MINOR} PARENT_SCOPE)
        set(Spglib_CXX_VERSION_PATCH ${Spglib_CXX_VERSION_PATCH} PARENT_SCOPE)
        set(Spglib_CXX_VERSION_TWEAK ${Spglib_CXX_VERSION_TWEAK} PARENT_SCOPE)
        set(Spglib_CXX_VERSION_FULL ${Spglib_CXX_VERSION_FULL} PARENT_SCOPE)
        set(Spglib_CXX_COMMIT ${Spglib_CXX_COMMIT} PARENT_SCOPE)
        set(Spglib_LIB_TYPE ${Spglib_LIB_TYPE} PARENT_SCOPE)
    else ()
        return(PROPAGATE
                Spglib_CXX_VERSION
                Spglib_CXX_VERSION_MAJOR
                Spglib_CXX_VERSION_MINOR
                Spglib_CXX_VERSION_PATCH
                Spglib_CXX_VERSION_TWEAK
                Spglib_CXX_VERSION_FULL
                Spglib_CXX_COMMIT
                Spglib_LIB_TYPE
        )
    endif ()
endif ()
