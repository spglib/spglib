list(APPEND ctest_labels c_api)
set(test_sources
		test_delaunay.cpp
		test_error.cpp
		test_symmetry.cpp
		test_symmetry_search.cpp
		test_spacegroup_type_search.cpp
		test_find_primitive_cell.cpp
		test_refine_cell.cpp
		test_standardization.cpp
		test_dataset_access.cpp
		test_magnetic_symmetry.cpp
		test_kpoints.cpp
		)
if (test_sources)
	configure_spglib_tests(TARGET Spglib_tests
			SOURCES ${test_sources}
			LABELS ${ctest_labels}
			)
endif ()
