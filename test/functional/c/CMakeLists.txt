set_property(DIRECTORY APPEND
        PROPERTY LABELS c_api
)

Spglib_add_gtest(TARGET Spglib_tests SOURCES
        test_error.cpp
        test_symmetry_search.cpp
        test_spacegroup_type_search.cpp
        test_find_primitive_cell.cpp
        test_refine_cell.cpp
        test_standardization.cpp
        test_dataset_access.cpp
        test_magnetic_symmetry.cpp
        test_magnetic_dataset.cpp
        test_kpoints.cpp
)
