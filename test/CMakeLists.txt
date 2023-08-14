enable_language(CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define/clear local variables to be used
set(ctest_labels "")

# Define the test framework
add_executable(Spglib_tests)
set_target_properties(Spglib_tests PROPERTIES
		OUTPUT_NAME test_suite
		)
target_sources(Spglib_tests PRIVATE
		utils.c
		)
target_link_libraries(Spglib_tests
		PRIVATE Spglib_symspg
		PRIVATE GTest::gtest GTest::gtest_main
		PRIVATE Threads::Threads
		)
target_include_directories(Spglib_tests
		PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Define a helper function to configure the google tests
function(configure_spglib_tests)
	cmake_parse_arguments(ARGS
			""
			"TARGET"
			"LABELS;SOURCES"
			${ARGN})

	target_sources(${ARGS_TARGET} PRIVATE ${ARGS_SOURCES})
	gtest_add_tests(TARGET ${ARGS_TARGET}
			SOURCES ${ARGS_SOURCES}
			TEST_LIST google_tests)
	foreach (g_test IN LISTS google_tests)
		set_tests_properties(${g_test} PROPERTIES
				LABELS "${ARGS_LABELS}")
	endforeach ()
endfunction()

# Add the individual tests types
add_subdirectory(unit)
add_subdirectory(integration)
add_subdirectory(functional)
