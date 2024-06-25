cmake_minimum_required(VERSION 3.20...3.29)

#[=============================================================================[
#                           Basic project definition                           #
]=============================================================================]

list(APPEND CMAKE_MESSAGE_CONTEXT Test)
project(Spglib_Test
        LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

#[=============================================================================[
#                                   Options                                   #
]=============================================================================]

include(CMakeDependentOption)
include(FeatureSummary)

cmake_dependent_option(SPGLIB_TEST_COVERAGE "Spglib: Test with coverage" OFF "Spglib_IS_TOP_LEVEL" OFF)
add_feature_info(Coverage SPGLIB_TEST_COVERAGE "Compile with test coverage")
mark_as_advanced(SPGLIB_TEST_COVERAGE)
if (Spglib_IS_TOP_LEVEL)
    set(SPGLIB_USE_SANITIZER "" CACHE STRING "Spglib: Sanitizer used in compilation")
endif ()
mark_as_advanced(SPGLIB_USE_SANITIZER)
option(SPGLIB_WITH_Fortran "Spglib: Build Fortran interface" OFF)
option(SPGLIB_WITH_Python "Spglib: Build Python interface" OFF)

#[=============================================================================[
#                            Project configuration                            #
]=============================================================================]

if (SPGLIB_WITH_Fortran)
    enable_language(Fortran)
endif ()
if (SPGLIB_TEST_COVERAGE)
    target_compile_options(Spglib_symspg PRIVATE --coverage)
    target_link_options(Spglib_symspg PUBLIC --coverage)
    if (SPGLIB_WITH_Fortran)
        target_compile_options(Spglib_fortran PRIVATE --coverage)
        target_link_options(Spglib_fortran PUBLIC --coverage)
    endif ()
    if (SPGLIB_WITH_Python)
        target_compile_options(Spglib_python PRIVATE --coverage)
        target_link_options(Spglib_python PUBLIC --coverage)
    endif ()
endif ()
if (SPGLIB_USE_SANITIZER)
    if (CMAKE_CXX_COMPILER_ID MATCHES "(Intel|MSVC)")
        message(WARNING "Sanitizers not supported on: ${CMAKE_CXX_COMPILER_ID}")
    else ()
        string(TOLOWER ${SPGLIB_USE_SANITIZER} SPGLIB_USE_SANITIZER)
        target_compile_options(Spglib_symspg PRIVATE
                -fsanitize=${SPGLIB_USE_SANITIZER}
        )
        target_link_options(Spglib_symspg PUBLIC
                -fsanitize=${SPGLIB_USE_SANITIZER}
        )
    endif ()
endif ()

#[=============================================================================[
#                              External packages                              #
]=============================================================================]

set(external_libs)
include(FetchContent)

set(fetchcontent_extra_args)
if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.24)
    # TODO: Use by default when minimum is CMake 3.24
    list(APPEND fetchcontent_extra_args
            FIND_PACKAGE_ARGS CONFIG
    )
endif ()

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

set(BUILD_GMOCK OFF)
set(INSTALL_GTEST OFF)
set(gtest_disable_pthreads ON)
FetchContent_Declare(GTest
        GIT_REPOSITORY https://github.com/google/googletest
        GIT_TAG v1.13.0
        # For CMake < 3.24, always download GTest. Packaging for CMake  <3.24 is unsupported
        ${fetchcontent_extra_args}
)
list(APPEND external_libs GTest)

find_package(Threads REQUIRED)
if (SPGLIB_WITH_Python)
    find_package(Python 3.9 REQUIRED)
endif ()

FetchContent_MakeAvailable(${external_libs})
include(GoogleTest)

#[=============================================================================[
#                               Main definition                               #
]=============================================================================]

enable_testing()

if (WIN32 AND CMAKE_VERSION VERSION_LESS 3.27)
    message(FATAL_ERROR "Cannot reliably test for windows with CMake < 3.27")
endif ()

# Define the test framework
add_executable(Spglib_tests)
set_target_properties(Spglib_tests PROPERTIES
        OUTPUT_NAME test_suite
)
if (SPGLIB_WITH_Fortran)
    add_executable(Spglib_Fortran_tests)
    set_target_properties(Spglib_Fortran_tests PROPERTIES
            OUTPUT_NAME test_suite_fortran
    )
endif ()

# Basic configuration for Spglib_tests
target_sources(Spglib_tests PRIVATE
        utils.c
)
target_link_libraries(Spglib_tests PRIVATE
        Spglib::symspg
        GTest::gtest GTest::gtest_main
        Threads::Threads
)
target_include_directories(Spglib_tests
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

# Define a helper function to configure the google tests
function(Spglib_add_gtest)
    #[===[.md
    # Spglib_add_gtest

    Internal helper for adding GTest tests with appropriate labels

    ## Synopsis
    ```cmake
    Spglib_add_gtest(TARGET <target> SOURCES <files>
            [LABELS <label1> <label2>])
    ```

    ## Options

    `TARGET`
      GTest target

    `SOURCES`
      Additional sources to add

    `LABELS`
      Additional labels to be added

    ]===]

    list(APPEND CMAKE_MESSAGE_CONTEXT "Spglib_add_gtest")

    set(ARGS_Options)
    set(ARGS_OneValue
            TARGET
    )
    set(ARGS_MultiValue
            LABELS
            SOURCES
    )
    cmake_parse_arguments(PARSE_ARGV 0 ARGS "${ARGS_Options}" "${ARGS_OneValue}" "${ARGS_MultiValue}")

    target_sources(${ARGS_TARGET} PRIVATE ${ARGS_SOURCES})
    gtest_add_tests(TARGET ${ARGS_TARGET}
            SOURCES ${ARGS_SOURCES}
            TEST_LIST google_tests
    )

    # Set test labels
    get_directory_property(test_labels LABELS)
    list(APPEND test_labels ${ARGS_LABELS})
    set_tests_properties(${google_tests} PROPERTIES
            LABELS "${test_labels}"
    )
    if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.27)
        # https://stackoverflow.com/a/77990416
        set_property(TEST ${google_tests} APPEND PROPERTY
                ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<JOIN:$<TARGET_RUNTIME_DLL_DIRS:Spglib_tests>,\;>"
        )
    endif ()
endfunction()

function(Spglib_add_test test)
    #[===[.md
    # Spglib_add_test

    Internal helper for adding functional tests specific for Spglib

    ## Synopsis
    ```cmake
    Spglib_add_test(<name>
            [TEST_NAME <test_name>]
            [LABELS <label1> <label2>]
            [TEST_COMMAND <arg1> <arg2>]
            )
    ```

    ## Options

    `<name>`
      Path to the CMake project to be executed relative to `${CMAKE_CURRENT_SOURCE_DIR}`

    `TEST_NAME` [Default: `<name>`]
      Name for the test to be used as the ctest name

    `LABELS`
      Additional labels to be added

    `TEST_COMMAND`
      Test command to use instead of ctest

    ]===]

    list(APPEND CMAKE_MESSAGE_CONTEXT "Spglib_add_test")

    set(ARGS_Options)
    set(ARGS_OneValue
            TEST_NAME
    )
    set(ARGS_MultiValue
            LABELS
            TEST_COMMAND
    )
    cmake_parse_arguments(PARSE_ARGV 1 ARGS "${ARGS_Options}" "${ARGS_OneValue}" "${ARGS_MultiValue}")
    # Check required/optional arguments
    if (ARGC LESS 1)
        message(FATAL_ERROR "Missing test name")
    endif ()
    if (NOT DEFINED ARGS_TEST_NAME)
        set(ARGS_TEST_NAME ${test})
    endif ()
    if (NOT DEFINED ARGS_TEST_COMMAND)
        set(ARGS_TEST_COMMAND
                ${CMAKE_CTEST_COMMAND} --test-dir ${CMAKE_CURRENT_BINARY_DIR}/${test} --output-on-failure
        )
    endif ()

    set(extra_args)
    if (Spglib_IS_TOP_LEVEL)
        list(APPEND extra_args
                -DFETCHCONTENT_TRY_FIND_PACKAGE_MODE=ALWAYS
                # Generated Config file point to binary targets until it is installed
                -DSpglib_ROOT=${Spglib_BINARY_DIR}
                -DFETCHCONTENT_SOURCE_DIR_SPGLIB=${Spglib_SOURCE_DIR}
        )
    endif ()
    list(APPEND extra_args
            -DSPGLIB_WITH_Fortran=${SPGLIB_WITH_Fortran}
    )
    set(compiler_args)
    foreach (lang IN ITEMS C CXX Fortran)
        if (DEFINED CMAKE_${lang}_COMPILER)
            list(APPEND compiler_args -DCMAKE_${lang}_COMPILER=${CMAKE_${lang}_COMPILER})
        endif ()
    endforeach ()

    add_test(NAME ${ARGS_TEST_NAME}
            COMMAND ${CMAKE_CTEST_COMMAND} --build-and-test ${CMAKE_CURRENT_SOURCE_DIR}/${test}
            ${CMAKE_CURRENT_BINARY_DIR}/${test}
            # Use the same build environment as the current runner
            --build-generator "${CMAKE_GENERATOR}"
            --build-options
            ${compiler_args} ${extra_args}
            --test-command ${ARGS_TEST_COMMAND}
    )
    set_tests_properties(${ARGS_TEST_NAME} PROPERTIES
            LABELS "${ARGS_LABELS}"
    )
endfunction()

# Add the individual tests types
add_subdirectory(unit)
add_subdirectory(integration)
add_subdirectory(functional)
add_subdirectory(package)
add_subdirectory(example)
