set_property(DIRECTORY APPEND
		PROPERTY LABELS unit_tests
)

Spglib_add_gtest(TARGET Spglib_tests SOURCES
		test_dataset_access.cpp
		test_error.cpp
		test_version.cpp
		)
target_compile_definitions(Spglib_tests PRIVATE
		Spglib_VERSION=${Spglib_VERSION}
		Spglib_VERSION_FULL=${Spglib_VERSION_FULL}
		Spglib_COMMIT=${Spglib_COMMIT}
)
