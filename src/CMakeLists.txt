configure_file(version.h.in version.h)

# Configure main target
target_sources(symspg PRIVATE
		arithmetic.c
		cell.c
		debug.c
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
set_target_properties(symspg PROPERTIES
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_VERSION_MAJOR}
		PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/spglib.h)
target_include_directories(symspg PUBLIC
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
		"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

# Link to third-party libraries
if (OpenMP_FOUND)
	target_link_libraries(symspg PRIVATE OpenMP::OpenMP_C)
endif()
# Should not be needed anymore
#if(NOT MSVC)
#	# Link to mathlibrary
#	target_link_libraries(symspg PRIVATE m)
#endif()


# Additional settings
if (SPGLIB_USE_SANITIZER)
	string(TOLOWER ${SPGLIB_USE_SANITIZER} SPGLIB_USE_SANITIZER)
	target_compile_options(symspg PUBLIC -fsanitize=${SPGLIB_USE_SANITIZER})
	target_link_options(symspg PUBLIC -fsanitize=${SPGLIB_USE_SANITIZER})
endif()
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	target_compile_definitions(symspg
			PUBLIC SPGDEBUG SPGWARNING)
endif ()

# Should not be needed anymore
## avoid One Definition Rule problems
## only available since 3.16.0
## VERSION_GREATER_EQUAL only available since 3.7.0, so let's do it manually
#if (NOT CMAKE_MAJOR_VERSION LESS "3")
#	if (CMAKE_MAJOR_VERSION GREATER "3" OR NOT CMAKE_MINOR_VERSION LESS "16")
#		set_source_files_properties(
#				${SOURCES} PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON
#		)
#	endif()
#endif()


# Install
if (NOT SKBUILD)
	# Normal installation target to system. When using scikit-build check python subdirectory
	if (WIN32)
		# TODO: Probably wrong, please fix
		install(TARGETS symspg
				EXPORT spglibTargets
				LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
				ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
				PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
				RUNTIME DESTINATION ${CMAKE_INSTALL_RUNTIMEDIR})
	else()
		install(TARGETS symspg
				EXPORT spglibTargets
				LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
				ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
				PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
	endif()
endif()
