list(APPEND ctest_labels c_api)
set(test_sources
		delaunay.cpp
		error.cpp
		symmetry.cpp
		symmetry_search.cpp
		spacegroup_type_search.cpp
		find_primitive_cell.cpp
		refine_cell.cpp
		standardization.cpp
		dataset_access.cpp
		magnetic_symmetry.cpp
		kpoints.cpp
		)
if (test_sources)
	configure_spglib_tests(TARGET Spglib_tests
			SOURCES ${test_sources}
			LABELS ${ctest_labels}
			)
endif ()
