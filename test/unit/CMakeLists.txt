list(APPEND ctest_labels unit_tests)
set(test_sources
		test_dataset_access.cpp
		test_error.cpp
		)
if (test_sources)
	configure_spglib_tests(TARGET Spglib_tests
			SOURCES ${test_sources}
			LABELS ${ctest_labels}
			)
endif ()
