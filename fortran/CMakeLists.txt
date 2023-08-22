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
		"$<BUILD_INTERFACE:${CMAKE_Fortran_MODULE_DIRECTORY}>")
target_link_libraries(Spglib_fortran PUBLIC Spglib_symspg)
# Note: Fortran wrapper is not linked to OpenMP library because it should not be defining any such setup

# Install
if (SPGLIB_INSTALL)
	# CMake does not properly support fortran module installation paths.
	# Adapting the standard from fortran-stdlib
	# https://gitlab.kitware.com/cmake/cmake/-/issues/19608
	# https://discourse.cmake.org/t/api-design-c-modules-source-listings-and-interface-properties/5389/14
	cmake_path(APPEND CMAKE_INSTALL_INCLUDEDIR ${PROJECT_NAME} "${CMAKE_Fortran_COMPILER_ID}-${CMAKE_Fortran_COMPILER_VERSION}"
			OUTPUT_VARIABLE _DEFAULT_CMAKE_INSTALL_MODULEDIR)
	set(CMAKE_INSTALL_MODULEDIR ${_DEFAULT_CMAKE_INSTALL_MODULEDIR}
			CACHE STRING
			"Fortran module installation path (Not a cmake native variable)"
	)
	cmake_path(IS_ABSOLUTE CMAKE_INSTALL_MODULEDIR _is_absolute)
	if (_is_absolute)
		set(CMAKE_INSTALL_FULL_MODULEDIR ${CMAKE_INSTALL_MODULEDIR})
	else ()
		cmake_path(APPEND CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_MODULEDIR}
				OUTPUT_VARIABLE CMAKE_INSTALL_FULL_MODULEDIR)
	endif ()
	target_include_directories(Spglib_fortran PUBLIC
			"$<INSTALL_INTERFACE:${CMAKE_INSTALL_MODULEDIR}>")
	install(TARGETS Spglib_fortran
			EXPORT SpglibTargets-fortran
			LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Spglib_Runtime NAMELINK_COMPONENT Spglib_Development
			ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Spglib_Development
			PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT Spglib_Development)
	export_components(COMPONENT fortran LIB_TYPE ${SPGLIB_LIB_TYPE})

	# Maybe it is handled automatically
	install(FILES ${CMAKE_Fortran_MODULE_DIRECTORY}/spglib_f08.mod
			DESTINATION ${CMAKE_INSTALL_MODULEDIR}
			COMPONENT Spglib_Development)

	configure_file(spglib_f08.pc.in spglib_f08.pc @ONLY)
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/spglib_f08.pc
			DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
			COMPONENT Spglib_Development)
endif ()
