cmake_minimum_required(VERSION 3.20...3.29)
project(example_spglib LANGUAGES C)

set(extra_args)
if (SPGLIB_WITH_Fortran)
	list(APPEND extra_args
			COMPONENTS fortran
	)
	enable_language(Fortran)
endif ()
find_package(Spglib REQUIRED ${extra_args})

add_executable(main_c src/main.c)
target_link_libraries(main_c PRIVATE Spglib::symspg)

enable_testing()

add_test(NAME smoke_test_c
		COMMAND $<TARGET_FILE:main_c>
)
set_tests_properties(smoke_test_c PROPERTIES
		PASS_REGULAR_EXPRESSION "^Spglib version: ${Spglib_VERSION}\n*$"
)

if (SPGLIB_WITH_Fortran)
	add_executable(main_Fortran src/main.F90)
	target_link_libraries(main_Fortran PRIVATE Spglib::fortran)
	add_test(NAME smoke_test_Fortran
			COMMAND $<TARGET_FILE:main_Fortran>
	)
endif ()
