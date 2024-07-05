cmake_minimum_required(VERSION 3.20...3.29)

#[=============================================================================[
#                           Basic project definition                           #
]=============================================================================]

list(APPEND CMAKE_MESSAGE_CONTEXT Python)
project(Spglib_Python
        LANGUAGES C
)
# Back-porting to PROJECT_IS_TOP_LEVEL to older cmake
# TODO: Remove when requiring cmake >= 3.21
if (NOT DEFINED Spglib_Python_IS_TOP_LEVEL)
    if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
        set(PROJECT_IS_TOP_LEVEL ON)
    else ()
        set(PROJECT_IS_TOP_LEVEL OFF)
    endif ()
endif ()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

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

set(BUILD_SHARED_LIBS ${SPGLIB_SHARED_LIBS})

#[=============================================================================[
#                              External packages                              #
]=============================================================================]

set(external_libs)
include(FetchContent)

# Get Spglib if it's run as stand-alone project
if (NOT Spglib_SOURCE_DIR)
    if (CMAKE_VERSION VERSION_LESS 3.24)
        # TODO: Use by default when minimum is CMake 3.24
        find_package(Spglib CONFIG QUIET)
        if (NOT Spglib_FOUND)
            FetchContent_Declare(Spglib
                    GIT_REPOSITORY https://github.com/spglib/spglib
                    GIT_TAG develop
            )
            list(APPEND external_libs Spglib)
        endif ()
    else ()
        FetchContent_Declare(Spglib
                GIT_REPOSITORY https://github.com/spglib/spglib
                GIT_TAG develop
                FIND_PACKAGE_ARGS CONFIG
        )
        list(APPEND external_libs Spglib)
    endif ()
endif ()
find_package(Python 3.9 COMPONENTS REQUIRED Interpreter Development.Module NumPy)
FetchContent_MakeAvailable(${external_libs})

#[=============================================================================[
#                               Main definition                               #
]=============================================================================]

Python_add_library(Spglib_python MODULE WITH_SOABI _spglib.c)
set_target_properties(Spglib_python PROPERTIES
        OUTPUT_NAME _spglib
)
target_link_libraries(Spglib_python PRIVATE
        Spglib::symspg Python::NumPy
)
# _version.py may not have been populated in source yet, use a dummy file for the build environment
# TODO: Use a scikit-build-cli or other CLI to populate the metadata files
if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/spglib/_version.py)
    configure_file(_version.py.in spglib/_version.py)
endif ()

# Copy all python packages to the build directory
file(COPY spglib
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
# Link the built library to the package in the binary directory
add_custom_command(TARGET Spglib_python POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        $<TARGET_FILE:Spglib_python>
        ${CMAKE_CURRENT_BINARY_DIR}/spglib/$<TARGET_FILE_NAME:Spglib_python>
)
# On Windows make sure the dll files are in the build directory
# https://stackoverflow.com/a/73550650
if (CMAKE_IMPORT_LIBRARY_SUFFIX)
    add_custom_command(TARGET Spglib_python POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:Spglib_python> ${CMAKE_CURRENT_BINARY_DIR}/spglib/
            COMMAND_EXPAND_LISTS
    )
endif ()

#[=============================================================================[
#                              Install or Export                              #
]=============================================================================]

if (NOT Python_INSTALL_DIR)
    if (SKBUILD)
        # If built with scikit-build-core, let it handle the installation
        set(Python_INSTALL_DIR ".")
    else ()
        # Otherwise try to install in current python executable's setup
        set(Python_INSTALL_DIR ${Python_SITEARCH})
    endif ()
endif ()
if (SPGLIB_INSTALL)
    if (TARGET Spglib_symspg)
        # If Spglib is not already installed on the system, install it in ${Python_INSTALL_DIR}
        # TODO: Cmake forces to install PUBLIC_HEADER when defined
        # https://gitlab.kitware.com/cmake/cmake/-/issues/24326
        install(TARGETS Spglib_symspg
                LIBRARY DESTINATION ${Python_INSTALL_DIR}/spglib/lib COMPONENT Spglib_Runtime
                NAMELINK_COMPONENT Spglib_Development
                ARCHIVE DESTINATION ${Python_INSTALL_DIR}/spglib/lib COMPONENT Spglib_Development
                PUBLIC_HEADER DESTINATION ${Python_INSTALL_DIR}/spglib/include COMPONENT Spglib_Development
                RUNTIME DESTINATION ${Python_INSTALL_DIR}/spglib COMPONENT Spglib_Runtime
        )
    endif ()
    install(TARGETS Spglib_python
            LIBRARY DESTINATION ${Python_INSTALL_DIR}/spglib COMPONENT Spglib_Runtime
    )

    # Install spglib module to local python path
    if (NOT SKBUILD)
        install(DIRECTORY spglib/ DESTINATION ${Python_INSTALL_DIR}/spglib COMPONENT Spglib_Runtime)
    endif ()
endif ()
