# Main fortran wrapper
add_library(spglib_f08 spglib_f08.f90)
add_library(spglib::spglib_f08 ALIAS spglib_f08)

# Configure Fortran wrapper
set_target_properties(spglib_f08 PROPERTIES
		VERSION ${PROJECT_VERSION}
		PUBLIC_HEADER spglib_f08.f90)
target_include_directories(spglib_f08 PUBLIC
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
		"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_link_libraries(spglib_f08 PRIVATE symspg)
# Note: Fortran wrapper is not linked to OpenMP library because it should not be defining any such setup

# Install
install(TARGETS spglib_f08
		EXPORT spglibTargets
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Maybe it is handled automatically
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/spglib_f08.mod DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

configure_file(spglib_f08.pc.in spglib_f08.pc @ONLY)
install(FILES  ${CMAKE_CURRENT_BINARY_DIR}/spglib_f08.pc
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
	COMPONENT pkgconfig)
