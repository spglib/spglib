enable_language(CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(CTests
		test_error.cpp
		test_symmetry_search.cpp
		test_spacegroup_type_search.cpp
		test_find_primitive_cell.cpp
		test_refine_cell.cpp
		test_standardization.cpp
		test_dataset_access.cpp
		test_magnetic_symmetry.cpp
		test_kpoints.cpp
		utils.c
		)
gtest_add_tests(TARGET CTests
		TEST_LIST google_tests)
target_link_libraries(CTests
		PRIVATE GTest::gtest GTest::gtest_main Spglib_symspg)

foreach (g_test IN LISTS google_tests)
	set_tests_properties(${g_test} PROPERTIES
			LABELS "c_api;${g_test}")
endforeach ()

if (SPGLIB_WITH_Fortran)
	add_subdirectory(fortran)
endif ()
