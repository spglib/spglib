# Main fortran wrapper
add_library(Spglib_fortran spglib_f08.f90)
add_library(Spglib::fortran ALIAS Spglib_fortran)

# Configure Fortran wrapper
set_target_properties(Spglib_fortran PROPERTIES
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_VERSION_MAJOR}
		EXPORT_NAME fortran
		OUTPUT_NAME spglib_f08
		PUBLIC_HEADER spglib_f08.f90)
set_target_properties(Spglib_fortran PROPERTIES
		PUBLIC_HEADER spglib_f08.f90)
target_include_directories(Spglib_fortran PUBLIC
		"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_link_libraries(Spglib_fortran PUBLIC Spglib_symspg)
# Note: Fortran wrapper is not linked to OpenMP library because it should not be defining any such setup

# Install
install(TARGETS Spglib_fortran
		EXPORT SpglibTargets
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Maybe it is handled automatically
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/spglib_f08.mod DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

configure_file(spglib_f08.pc.in spglib_f08.pc @ONLY)
install(FILES  ${CMAKE_CURRENT_BINARY_DIR}/spglib_f08.pc
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
	COMPONENT pkgconfig)
