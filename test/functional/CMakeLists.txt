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
