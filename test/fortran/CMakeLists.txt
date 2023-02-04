add_library(test_utils STATIC utils.f90)

# Generated ctest file that switches and displays tests
# Note must be plain C because generated CPP file does not include extern "C"
set(TestsDriver fortran_test.c)

# Add fortran tests here
create_test_sourcelist(FortranTests_Files
		${TestsDriver}
		test_spg_get_symmetry.f90)

add_executable(FortranTests ${FortranTests_Files})
target_link_libraries(FortranTests PRIVATE test_utils spglib_f08)

# Remove the driver executable from the list for add_test
set(TestsToRun ${FortranTests_Files})
list(REMOVE_ITEM TestsToRun ${TestsDriver})

foreach(test ${TestsToRun})
	get_filename_component(test_name ${test} NAME_WE)
	# First argument is the name of the test
	# Can add more cli arguments that will be passed to the fortran test
	add_test(NAME ${test_name}
			COMMAND $<TARGET_FILE:FortranTests> ${test_name} test_arg1 test_arg2)
endforeach()
