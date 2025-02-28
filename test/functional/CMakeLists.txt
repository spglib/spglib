set_property(DIRECTORY APPEND
        PROPERTY LABELS functional_tests
)

add_subdirectory(c)
if (SPGLIB_WITH_Fortran)
    add_subdirectory(fortran)
endif ()
