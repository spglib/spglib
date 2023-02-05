add_library(test_utils STATIC utils.f90)
set(TestsToRun
    test_fortran_spg_get_symmetry
    test_fortran_spg_refine_cell)

foreach(NAME ${TestsToRun})
    add_executable(${NAME} ${NAME}.f90)
    target_link_libraries(${NAME} PRIVATE test_utils spglib_f08)
    set_target_properties(${NAME} PROPERTIES Fortran_MODULE_DIRECTORY
                                             ${LIB_MOD_DIR})
    add_test(
    NAME ${NAME}
    COMMAND $<TARGET_FILE:${NAME}> ${CMAKE_CURRENT_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endforeach()
