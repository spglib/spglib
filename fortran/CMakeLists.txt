if (USE_OMP)
	find_package(OpenMP COMPONENTS Fortran REQUIRED)
	set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} ${OpenMP_Fortran_FLAGS}")
endif()

add_library(spglib_f08_static STATIC ${PROJECT_SOURCE_DIR}/fortran/spglib_f08.f90)
set_property(TARGET spglib_f08_static PROPERTY VERSION ${PROJECT_VERSION})
set_property(TARGET spglib_f08_static PROPERTY OUTPUT_NAME spglib_f08)
target_link_libraries(spglib_f08_static PRIVATE symspg_static)

add_library(spglib_f08 SHARED ${PROJECT_SOURCE_DIR}/fortran/spglib_f08.f90)
set_property(TARGET spglib_f08 PROPERTY VERSION ${PROJECT_VERSION})
set_property(TARGET spglib_f08 PROPERTY SOVERSION ${soserial})
target_link_libraries(spglib_f08 PUBLIC symspg)
target_include_directories(spglib_f08 PUBLIC
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
		"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

install(TARGETS spglib_f08
		EXPORT spglibTargets
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(TARGETS spglib_f08_static
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/spglib_f08.mod DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

configure_file(spglib_f08.pc.in ${CMAKE_CURRENT_BINARY_DIR}/spglib_f08.pc @ONLY)
install(
	FILES  ${CMAKE_CURRENT_BINARY_DIR}/spglib_f08.pc
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
	COMPONENT pkgconfig)
