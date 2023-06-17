list(APPEND ctest_labels unit_tests)
set(test_sources
		dataset_acess.cpp
		error.cpp
		)
if (test_sources)
	configure_spglib_tests(TARGET Spglib_tests
			SOURCES ${test_sources}
			LABELS ${ctest_labels}
			)
endif ()
