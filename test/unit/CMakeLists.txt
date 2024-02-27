set_property(DIRECTORY APPEND
        PROPERTY LABELS unit_tests
)

Spglib_add_gtest(TARGET Spglib_tests SOURCES
        test_dataset_access.cpp
        test_error.cpp
        test_version.cpp
)
target_compile_definitions(Spglib_tests PRIVATE
        Spglib_VERSION=${Spglib_VERSION}
        Spglib_VERSION_FULL=${Spglib_VERSION_FULL}
        Spglib_COMMIT=${Spglib_COMMIT}
)

if (TARGET Spglib_symspg)
    # Add non-public tests only if test-suite is run by main project
    target_include_directories(Spglib_tests PRIVATE
            ${Spglib_SOURCE_DIR}/src
    )
    target_compile_definitions(Spglib_symspg PRIVATE SPG_TESTING)
    Spglib_add_gtest(TARGET Spglib_tests SOURCES
            test_delaunay.cpp
            test_niggli.cpp
            test_symmetry.cpp
    )
endif ()
