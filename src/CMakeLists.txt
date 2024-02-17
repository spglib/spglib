configure_file(version.h.in version.h)
set_property(DIRECTORY APPEND PROPERTY
		CMAKE_CONFIGURE_DEPENDS ${PROJECT_BINARY_DIR}/.git_commit
)
add_dependencies(Spglib_symspg Spglib_GitHash)

# Add compiler warnings
if (SPGLIB_COMPILATION_WARNING)
	if (MSVC)
		target_compile_options(Spglib_symspg PRIVATE /W4)
	else()
		# TODO: C23: Disabled -Wpedantic because of warning spam
		#  Add it back when C23 standard is widespread and revert this [Temp][C23] commit
		target_compile_options(Spglib_symspg PRIVATE -Wall -Wextra)
	endif()
endif ()

# Configure main target
target_sources(Spglib_symspg PRIVATE
		arithmetic.c
		arithmetic.h
		cell.c
		cell.h
		debug.h
		delaunay.c
		delaunay.h
		determination.c
		determination.h
		hall_symbol.c
		hall_symbol.h
		kgrid.c
		kgrid.h
		kpoint.c
		kpoint.h
		magnetic_spacegroup.c
		magnetic_spacegroup.h
		mathfunc.c
		mathfunc.h
		msg_database.c
		msg_database.h
		niggli.c
		niggli.h
		overlap.c
		overlap.h
		pointgroup.c
		pointgroup.h
		primitive.c
		primitive.h
		refinement.c
		refinement.h
		site_symmetry.c
		site_symmetry.h
		sitesym_database.c
		sitesym_database.h
		spacegroup.c
		spacegroup.h
		spg_database.c
		spg_database.h
		spglib.c
		spin.c
		spin.h
		symmetry.c
		symmetry.h
)
if (SPGLIB_DEBUG)
	target_sources(Spglib_symspg PRIVATE debug.c)
endif ()

# Write spglib_export.h to the current binary directory
include(GenerateExportHeader)
generate_export_header(Spglib_symspg
	EXPORT_FILE_NAME spglib_export.h
	EXPORT_MACRO_NAME API_SPGLIB
)
configure_file(spglib.h spglib.h COPYONLY)

target_sources(Spglib_symspg
	PUBLIC
		FILE_SET HEADERS
		BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
		FILES
			${CMAKE_CURRENT_BINARY_DIR}/spglib_export.h
			${CMAKE_CURRENT_BINARY_DIR}/spglib.h
)
target_include_directories(Spglib_symspg
		PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
		# This is what FILE_SET TYPE HEADERS should do automatically
		# PUBLIC
		# "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
		# "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

# Link to third-party libraries
if (SPGLIB_USE_OMP)
	target_link_libraries(Spglib_symspg PRIVATE OpenMP::OpenMP_C)
endif ()
if (NOT MSVC)
	# Link to mathlibrary
	target_link_libraries(Spglib_symspg PUBLIC m)
endif ()


# Additional settings
if (SPGLIB_DEBUG)
	target_compile_definitions(Spglib_symspg PRIVATE SPGDEBUG)
endif ()
if (SPGLIB_WARNINGS)
	target_compile_definitions(Spglib_symspg PRIVATE SPGWARNING)
endif ()

# Avoid One Definition Rule problems. Please fix these
if (CMAKE_UNITY_BUILD)
	set_source_files_properties(
			spin.c symmetry.c site_symmetry.c spacegroup.c spglib.c
			refinement.c pointgroup.c determination.c delaunay.c
			PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON
	)
endif ()


# Install
if (NOT SKBUILD AND SPGLIB_INSTALL)
	# Normal installation target to system. When using scikit-build this is installed again in python path
	install(TARGETS Spglib_symspg
			EXPORT SpglibTargets
			LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Spglib_Runtime
			NAMELINK_COMPONENT Spglib_Development
			ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Spglib_Development
			FILE_SET HEADERS INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
			RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Spglib_Runtime
	)
endif ()
