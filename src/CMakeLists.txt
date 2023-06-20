configure_file(version.h.in version.h)

# Add compiler warnings
if (MSVC)
	target_compile_options(Spglib_symspg PRIVATE /W4)
else()
	# TODO: C23: Disabled -Wpedantic because of warning spam
	#  Add it back when C23 standard is widespread and revert this [Temp][C23] commit
	target_compile_options(Spglib_symspg PRIVATE -Wall -Wextra)
endif()

# Configure main target
target_sources(Spglib_symspg PRIVATE
		arithmetic.c
		cell.c
		delaunay.c
		determination.c
		hall_symbol.c
		kgrid.c
		kpoint.c
		magnetic_spacegroup.c
		mathfunc.c
		msg_database.c
		niggli.c
		overlap.c
		pointgroup.c
		primitive.c
		refinement.c
		site_symmetry.c
		sitesym_database.c
		spacegroup.c
		spg_database.c
		spglib.c
		spin.c
		symmetry.c
		)
if (SPGLIB_DEBUG)
	target_sources(Spglib_symspg PRIVATE
			debug.c
			)
endif ()
set_target_properties(Spglib_symspg PROPERTIES
		PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/spglib.h)
# TODO: Should move spglib.h to a separate folder so other headers are not included in ExternalProject setups
target_include_directories(Spglib_symspg PUBLIC
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
		"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

# Link to third-party libraries
if (SPGLIB_USE_OMP)
	target_link_libraries(Spglib_symspg PRIVATE OpenMP::OpenMP_C)
endif ()
if (NOT MSVC)
	# Link to mathlibrary
	target_link_libraries(Spglib_symspg PUBLIC m)
endif ()


# Additional settings
if (SPGLIB_USE_SANITIZER)
	if (CMAKE_Fortran_COMPILER_ID MATCHES "Intel.*")
		# TODO: Address sanitizer cannot run on intel
		message(WARNING "Sanitizer are not bundled well with intel compiler")
	else ()
		string(TOLOWER ${SPGLIB_USE_SANITIZER} SPGLIB_USE_SANITIZER)
		target_compile_options(Spglib_symspg PRIVATE -fsanitize=${SPGLIB_USE_SANITIZER})
		target_link_options(Spglib_symspg PUBLIC -fsanitize=${SPGLIB_USE_SANITIZER})
	endif ()
endif ()
if (SPGLIB_DEBUG)
	target_compile_definitions(Spglib_symspg PRIVATE SPGDEBUG)
endif ()
if (SPGLIB_WARNINGS)
	target_compile_definitions(Spglib_symspg PRIVATE SPGWARNING)
endif ()

# Avoid One Definition Rule problems. Please fix these
if (CMAKE_UNITY_BUILD)
	set_source_files_properties(
			symmetry.c site_symmetry.c spacegroup.c spglib.c refinement.c pointgroup.c determination.c delaunay.c
			PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON
	)
endif ()


# Install
if (NOT SKBUILD AND SPGLIB_INSTALL)
	# Normal installation target to system. When using scikit-build check python subdirectory
	if (WIN32)
		# TODO: Probably wrong, please fix
		install(TARGETS Spglib_symspg
				EXPORT SpglibTargets
				LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Spglib_Runtime NAMELINK_COMPONENT Spglib_Development
				ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Spglib_Development
				PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT Spglib_Development
				RUNTIME DESTINATION ${CMAKE_INSTALL_RUNTIMEDIR} COMPONENT Spglib_Runtime)
	else ()
		install(TARGETS Spglib_symspg
				EXPORT SpglibTargets
				LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Spglib_Runtime NAMELINK_COMPONENT Spglib_Development
				ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Spglib_Development
				PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT Spglib_Development)
	endif ()
endif ()
