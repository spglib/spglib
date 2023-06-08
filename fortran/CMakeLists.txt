# Main fortran wrapper
add_library(Spglib_fortran spglib_f08.F90)
add_library(Spglib::fortran ALIAS Spglib_fortran)

# Configure Fortran wrapper
set_target_properties(Spglib_fortran PROPERTIES
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_VERSION_MAJOR}
		EXPORT_NAME fortran
		OUTPUT_NAME spglib_f08
		PUBLIC_HEADER spglib_f08.F90)
set_target_properties(Spglib_fortran PROPERTIES
		PUBLIC_HEADER spglib_f08.F90)
target_include_directories(Spglib_fortran PUBLIC
		"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_link_libraries(Spglib_fortran PUBLIC Spglib_symspg)
# Note: Fortran wrapper is not linked to OpenMP library because it should not be defining any such setup

# Install
if (SPGLIB_INSTALL)
	install(TARGETS Spglib_fortran
			EXPORT SpglibTargets-fortran
			LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Spglib_Runtime NAMELINK_COMPONENT Spglib_Development
			ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Spglib_Development
			PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT Spglib_Development)
	export_components(COMPONENT fortran LIB_TYPE ${SPGLIB_LIB_TYPE})

	# Maybe it is handled automatically
	install(FILES ${CMAKE_Fortran_MODULE_DIRECTORY}/spglib_f08.mod
			DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
			COMPONENT Spglib_Development)

	configure_file(spglib_f08.pc.in spglib_f08.pc @ONLY)
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/spglib_f08.pc
			DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
			COMPONENT Spglib_Development)
endif ()
