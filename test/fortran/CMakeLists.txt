add_library(test_utils STATIC utils.f90 c_interface_module.F90)

# Generated ctest file that switches and displays tests
# Note must be plain C because generated CPP file does not include extern "C"
set(TestsDriver fortran_test.c)
set(SubTests_test_fortran_spg_get_symmetry "test_rutile112;test_zincblende")

# Add fortran tests here
create_test_sourcelist(FortranTests_Files
		${TestsDriver}
		test_fortran_spg_get_symmetry.F90
		test_fortran_spg_refine_cell.F90)
# Define any subtests (would be nice to auto-detect them like Catch2)


add_executable(FortranTests ${FortranTests_Files})
target_link_libraries(FortranTests PRIVATE test_utils spglib_f08)

# Remove the driver executable from the list for add_test
set(TestsToRun ${FortranTests_Files})
list(REMOVE_ITEM TestsToRun ${TestsDriver})

# Loop over all main tests files
foreach (test ${TestsToRun})
	get_filename_component(test_name ${test} NAME_WE)
	# Check if we have subtests defined
	if (DEFINED SubTests_${test_name})
		# Loop over all subtests
		foreach (subtest ${SubTests_${test_name}})
			# Call test via ${FortranTests_Executable} ${test_name} ${subtest}
			# E.g. running manually: ./FortranTests_Executable test_fortran_spg_get_symmetry test_rutile112
			add_test(NAME "${test_name}/${subtest}"
					COMMAND $<TARGET_FILE:FortranTests> ${test_name} ${subtest})
			set_tests_properties("${test_name}/${subtest}" PROPERTIES
					LABELS "fortran;${test_name}")
		endforeach ()
	else ()
		# Only run the main test without arguments. Maybe can pass other variable data though :-?
		add_test(NAME ${test_name}
				COMMAND $<TARGET_FILE:FortranTests> ${test_name})
		set_tests_properties(${test_name} PROPERTIES
				LABELS "fortran")
	endif ()
endforeach ()
