list(APPEND ctest_labels unit_tests)
set(test_sources
		test_dataset_access.cpp
		test_error.cpp
		test_version.cpp
		)
target_compile_definitions(Spglib_tests PRIVATE
		Spglib_VERSION=${Spglib_VERSION}
		Spglib_VERSION_FULL=${Spglib_VERSION_FULL}
		Spglib_COMMIT=${Spglib_COMMIT}
)
if (test_sources)
	configure_spglib_tests(TARGET Spglib_tests
			SOURCES ${test_sources}
			LABELS ${ctest_labels}
			)
endif ()
