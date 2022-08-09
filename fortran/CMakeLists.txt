if (USE_OMP)
	find_package(OpenMP COMPONENTS Fortran REQUIRED)
	set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} ${OpenMP_Fortran_FLAGS}")
endif()

add_library(spglib_f08_static STATIC ${PROJECT_SOURCE_DIR}/fortran/spglib_f08.f90)
set_property(TARGET spglib_f08_static PROPERTY VERSION ${serial})
set_property(TARGET spglib_f08_static PROPERTY SOVERSION ${soserial})
set_property(TARGET spglib_f08_static PROPERTY OUTPUT_NAME spglib_f08)

add_library(spglib_f08 SHARED ${PROJECT_SOURCE_DIR}/fortran/spglib_f08.f90)
set_property(TARGET spglib_f08 PROPERTY VERSION ${serial})
set_property(TARGET spglib_f08 PROPERTY SOVERSION ${soserial})
add_dependencies(spglib_f08 spglib_f08_static)

add_executable(spglib_example_fortran.X ${PROJECT_SOURCE_DIR}/fortran/example_f08.f90)
target_link_libraries(spglib_example_fortran.X PRIVATE spglib_f08_static symspg_static ${OpenMP_Fortran_LIBRARIES})
install(TARGETS spglib_f08_static spglib_f08 spglib_example_fortran.X
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} LIBRARY DESTINATION DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(FILES ${PROJECT_SOURCE_DIR}/fortran/spglib_f08.f90 DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/spglib_f08.mod DESTINATION ${CMAKE_INSTALL_LIBDIR})

configure_file(spglib_f08.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/spglib_f08.pc)
install(
	FILES  ${CMAKE_CURRENT_BINARY_DIR}/spglib_f08.pc
	DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig
	COMPONENT pkgconfig)
