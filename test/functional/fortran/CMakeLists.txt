set_property(DIRECTORY APPEND
        PROPERTY LABELS fortran_api
)

# Cannot use native googletest. Using a rudimentary ctest native testing suite
target_sources(Spglib_Fortran_tests PRIVATE
        utils.F90
        c_interface_module.F90
)

# Generated ctest file that switches and displays tests
# Note must be plain C because generated CPP file does not include extern "C"
# TODO: Gather subtests via regex analyzing sources
set(SubTests_test_fortran_spg_get_symmetry
        test_rutile112
        test_zincblende
)
set(SubTests_test_fortran_spg_get_dataset
        test_dataset_rutile112
        test_dataset_zincblende
)
set(SubTests_test_fortran_spg_get_spacegroup_type
        test_spacegroup_type_rutile
)
set(SubTests_test_fortran_spg_get_magnetic_dataset
        test_rutile_type_III
        test_Cr_type_IV
        test_magnetic_dataset_non_collinear
)
set(SubTests_test_fortran_spg_get_magnetic_spacegroup_type
        test_magnetic_spacegroup_type_Cr
)
set(SubTests_test_fortran_spg_get_symmetry_with_site_tensors
        test_site_tensors_rutile_type_III
        test_site_tensors_Cr_type_IV
        test_site_tensors_non_collinear
)

# Add fortran tests here
create_test_sourcelist(FortranTests_Files
        fortran_test.c
        test_fortran_spg_get_spacegroup_type.F90
        test_fortran_spg_get_dataset.F90
        test_fortran_spg_get_magnetic_spacegroup_type.F90
        test_fortran_spg_get_magnetic_dataset.F90
        test_fortran_spg_get_symmetry_with_site_tensors.F90
        test_fortran_spg_get_symmetry.F90
        test_fortran_spg_refine_cell.F90
        test_fortran_version.F90
)

# CMake < 3.29 includes the driver source in the source list
list(REMOVE_ITEM FortranTests_Files fortran_test.c)
target_sources(Spglib_Fortran_tests PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/fortran_test.c
        ${FortranTests_Files}
)
target_link_libraries(Spglib_Fortran_tests PRIVATE Spglib::fortran)
target_link_options(Spglib_Fortran_tests PRIVATE
        "$<$<Fortran_COMPILER_ID:Intel,IntelLLVM>:$<$<COMPILE_LANGUAGE:Fortran>:-nofor-main>>")
target_compile_definitions(Spglib_Fortran_tests PRIVATE
        Spglib_VERSION=${Spglib_VERSION}
        Spglib_VERSION_FULL=${Spglib_VERSION_FULL}
        Spglib_COMMIT=${Spglib_COMMIT}
)

# Loop over all main tests files
foreach (test IN LISTS FortranTests_Files)
    if (test MATCHES ".*fortran_test.*")
        # Skip the test driver
        continue()
    endif ()
    cmake_path(REMOVE_EXTENSION test OUTPUT_VARIABLE test_name)
    # Check if we have subtests defined
    if (DEFINED SubTests_${test_name})
        # Loop over all subtests
        foreach (subtest ${SubTests_${test_name}})
            # Call test via ${Spglib_Fortran_tests} ${test_name} ${subtest}
            # E.g. running manually: ./test_suite_fortran test_fortran_spg_get_symmetry test_rutile112
            add_test(NAME "${test_name}.${subtest}"
                    COMMAND $<TARGET_FILE:Spglib_Fortran_tests> ${test_name} ${subtest}
            )
        endforeach ()
    else ()
        # Only run the main test without arguments. Maybe can pass other variable data though :-?
        add_test(NAME ${test_name}
                COMMAND $<TARGET_FILE:Spglib_Fortran_tests> ${test_name}
        )
    endif ()
endforeach ()
