cmake_minimum_required(VERSION 3.25...3.29)
project(example_spglib LANGUAGES C)

include(FetchContent)

if (DEFINED ENV{PACKIT_SOURCE_URL})
    set(git_repo $ENV{PACKIT_SOURCE_URL})
else ()
    set(git_repo https://github.com/spglib/spglib)
endif ()
if (DEFINED ENV{PACKIT_SOURCE_SHA})
    set(git_tag $ENV{PACKIT_SOURCE_SHA})
elseif (DEFINED ENV{PACKIT_COMMIT_SHA})
    set(git_tag $ENV{PACKIT_COMMIT_SHA})
else ()
    set(git_tag develop)
endif ()

FetchContent_Declare(Spglib
        GIT_REPOSITORY ${git_repo}
        GIT_TAG ${git_tag}
)
FetchContent_MakeAvailable(Spglib)

add_executable(main_c src/main.c)
target_link_libraries(main_c PRIVATE Spglib::symspg)

enable_testing()

add_test(NAME smoke_test_c
        COMMAND $<TARGET_FILE:main_c>
)
set_tests_properties(smoke_test_c PROPERTIES
        PASS_REGULAR_EXPRESSION "^Spglib version: ${Spglib_VERSION}\n*$"
)
if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.27)
    # https://stackoverflow.com/a/77990416
    set_property(TEST smoke_test_c APPEND PROPERTY
            ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<JOIN:$<TARGET_RUNTIME_DLL_DIRS:main_c>,\;>"
    )
endif ()

if (SPGLIB_WITH_Fortran)
    enable_language(Fortran)
    add_executable(main_Fortran src/main.F90)
    target_link_libraries(main_Fortran PRIVATE Spglib::fortran)
    add_test(NAME smoke_test_Fortran
            COMMAND $<TARGET_FILE:main_Fortran>
    )
    set_tests_properties(smoke_test_Fortran PROPERTIES
            PASS_REGULAR_EXPRESSION
            "^[ \n]*Spglib version: ${Spglib_VERSION}[ \n]*Spglib_Fortran version: ${Spglib_VERSION}[ \n]*$"
    )
    if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.27)
        # https://stackoverflow.com/a/77990416
        set_property(TEST smoke_test_Fortran APPEND PROPERTY
                ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<JOIN:$<TARGET_RUNTIME_DLL_DIRS:main_Fortran>,\;>"
        )
    endif ()
endif ()
