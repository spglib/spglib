cmake_minimum_required(VERSION 3.25...3.29)

#[=============================================================================[
#                           Basic project definition                           #
]=============================================================================]

list(APPEND CMAKE_MESSAGE_CONTEXT Python)
project(Spglib_Python
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

#[=============================================================================[
#                                   Options                                   #
]=============================================================================]

option(SPGLIB_INSTALL "Spglib: Install project" ${PROJECT_IS_TOP_LEVEL})
option(SPGLIB_SHARED_LIBS "Spglib: Build as a shared library" ${PROJECT_IS_TOP_LEVEL})

#[=============================================================================[
#                            Project configuration                            #
]=============================================================================]

include(GNUInstallDirs)

set(BUILD_SHARED_LIBS ${SPGLIB_SHARED_LIBS})

#[=============================================================================[
#                                Public targets                                #
]=============================================================================]

# Running `find_package(Python)` early to be able to define target with `Python_add_library`
find_package(Python 3.9 COMPONENTS REQUIRED Development.Module NumPy)
find_package(pybind11 CONFIG REQUIRED)
pybind11_add_module(Spglib_python MODULE WITH_SOABI)
add_library(Spglib::python ALIAS Spglib_python)

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
set_target_properties(Spglib_python PROPERTIES
        OUTPUT_NAME _spglib
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/spglib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/spglib
        INSTALL_RPATH "$<IF:$<BOOL:${APPLE}>,@loader_path,$ORIGIN>/${CMAKE_INSTALL_LIBDIR}"
)
target_sources(Spglib_python PRIVATE
        _spglib.cpp
        py_bindings.cpp
)
target_link_libraries(Spglib_python PRIVATE
        Spglib::symspg Python::NumPy
)
target_include_directories(Spglib_python PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
# _version.py may not have been populated in source yet, use a dummy file for the build environment
# TODO: Use a scikit-build-cli or other CLI to populate the metadata files
if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/spglib/_version.py)
    configure_file(_version.py.in spglib/_version.py)
endif ()

# Copy all python packages to the build directory
file(COPY spglib
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

# On Windows make sure the dll files are in the build directory
# https://stackoverflow.com/a/73550650
if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.26)
    # This form works when no files are specified
    add_custom_command(TARGET Spglib_python POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy -t ${CMAKE_CURRENT_BINARY_DIR}/spglib/ $<TARGET_RUNTIME_DLLS:Spglib_python>
            COMMAND_EXPAND_LISTS
    )
else ()
    if(WIN32)
        add_custom_command(TARGET Spglib_python POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:Spglib_python> ${CMAKE_CURRENT_BINARY_DIR}/spglib/
                COMMAND_EXPAND_LISTS
        )
    endif ()
endif ()

#[=============================================================================[
#                              Install or Export                              #
]=============================================================================]

if (NOT SKBUILD AND SPGLIB_INSTALL)
    message(WARNING "Installing the python bindings outside of scikit-build-core environment is not supported.")
elseif (SPGLIB_INSTALL)
    if (TARGET Spglib_symspg)
        # For windows systems we need to also install a copy of the dll files
        install(TARGETS Spglib_symspg
                RUNTIME DESTINATION .
        )
    endif ()
    install(TARGETS Spglib_python
            LIBRARY DESTINATION .
    )
endif ()
