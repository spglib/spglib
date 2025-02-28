configure_file(version.h.in version.h)
set_property(DIRECTORY APPEND PROPERTY
        CMAKE_CONFIGURE_DEPENDS ${PROJECT_BINARY_DIR}/.git_commit
)
add_dependencies(Spglib_symspg Spglib_GitHash)
target_compile_definitions(Spglib_symspg PRIVATE SPG_BUILD)
# For windows consumers we need to disable __declspec(dllimport) if it's built as a static library
if (WIN32 AND NOT SPGLIB_SHARED_LIBS)
    target_compile_definitions(Spglib_symspg PUBLIC SPG_STATIC_LIBRARY)
endif ()

# Add compiler warnings
if (SPGLIB_COMPILATION_WARNING)
    if (MSVC)
        target_compile_options(Spglib_symspg PRIVATE /W4)
    else ()
        # TODO: C23: Disabled -Wpedantic because of warning spam
        #  Add it back when C23 standard is widespread and revert this [Temp][C23] commit
        target_compile_options(Spglib_symspg PRIVATE -Wall -Wextra)
    endif ()
endif ()

# Configure main target
target_sources(Spglib_symspg PRIVATE
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
target_include_directories(Spglib_symspg PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
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
if (SPGLIB_INFO)
    target_compile_definitions(Spglib_symspg PRIVATE SPGINFO)
endif ()

# Avoid One Definition Rule problems. Please fix these
if (CMAKE_UNITY_BUILD)
    set_source_files_properties(
            symmetry.c site_symmetry.c spacegroup.c spglib.c refinement.c pointgroup.c determination.c delaunay.c
            PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON
    )
endif ()


# Install
if (SPGLIB_INSTALL)
    # Normal installation target to system. When using scikit-build this is installed again in python path
    install(TARGETS Spglib_symspg
        EXPORT SpglibTargets
        FILE_SET HEADERS
    )
endif ()
