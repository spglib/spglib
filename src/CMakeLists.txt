configure_file(version.h.in version.h)
set_property(DIRECTORY APPEND PROPERTY
        CMAKE_CONFIGURE_DEPENDS ${PROJECT_BINARY_DIR}/.git_commit
)
add_dependencies(Spglib_symspg Spglib_GitHash)
target_compile_definitions(Spglib_symspg PRIVATE SPG_BUILD)

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
configure_file(spglib.h spglib.h COPYONLY)
target_include_directories(Spglib_symspg
        PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
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
            symmetry.c site_symmetry.c spacegroup.c spglib.c refinement.c pointgroup.c determination.c delaunay.c
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
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT Spglib_Development
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Spglib_Runtime
    )
endif ()
