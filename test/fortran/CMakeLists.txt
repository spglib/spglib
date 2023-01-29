# Bundle some helpful C->Fortran translation functions
# Source: FortranWiki https://fortranwiki.org/fortran/show/c_interface_module
add_library(c_interface_module STATIC c_interface_module.F90)

# Generated ctest file that switches and displays tests
# Note must be plain C because generated CPP file does not include extern "C"
set(TestsDriver fortran_test.c)

# Add fortran tests here
create_test_sourcelist(FortranTests_Files
		${TestsDriver}
		test_dummy.F90)

add_executable(FortranTests ${FortranTests_Files})
target_link_libraries(FortranTests PRIVATE c_interface_module spglib_f08)

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
