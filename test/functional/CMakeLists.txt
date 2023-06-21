list(APPEND ctest_labels functional_tests)
set(test_sources "")
if (test_sources)
	configure_spglib_tests(TARGET Spglib_tests
			SOURCES ${test_sources}
			LABELS ${ctest_labels}
			)
endif ()
add_subdirectory(c)
if (SPGLIB_WITH_Fortran)
	add_subdirectory(fortran)
endif ()
if (SPGLIB_WITH_Python)
	# TODO: Migrate the main tests to C library
	add_test (NAME python-tests
			COMMAND ${Python3_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}
			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			)
	set_tests_properties(python-tests PROPERTIES
			LABELS "functional_tests;pytest")
endif ()
