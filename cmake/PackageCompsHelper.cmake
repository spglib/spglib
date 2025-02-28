## Helpers to get package components
# Special handling for `static` and `shared` components
# Implementation inspired by Alex Reinking blog post
# https://alexreinking.com/blog/building-a-dual-shared-and-static-library-with-cmake.html

include_guard()

# Define a helper macro for the next macro
macro(try_get_package_comp _comp)
    # Macro variables::
    #   _comp (string): Package component name
    # Named arguments::
    #   PACKAGE (string) [${PROJECT_NAME}]: Project name. Also used as prefix
    #   LIB_PREFIX (string): Static/Shared library prefix
    #   FALLBACK_PREFIX (string): Allowed library fallback prefix
    # Options
    #   PRINT: Print results when loading each component
    #   CHECK_REQUIRED: Check if component is required and not present

    cmake_parse_arguments(ARGS
            "PRINT;CHECK_REQUIRED"
            "PACKAGE;LIB_PREFIX;FALLBACK_PREFIX"
            ""
            ${ARGN})
    message(VERBOSE "Running try_get_package_comp: ${_comp}")

    if (NOT DEFINED ARGS_PACKAGE)
        message(FATAL_ERROR "It should be define upstream")
        set(ARGS_PACKAGE "${CMAKE_FIND_PACKAGE_NAME}")
    endif ()

    message(DEBUG
            "PACKAGE = ${ARGS_PACKAGE}\n"
            "_comp = ${_comp}\n"
            "LIB_PREFIX = ${ARGS_LIB_PREFIX}\n"
            "FALLBACK_PREFIX = ${ARGS_FALLBACK_PREFIX}\n"
            "PRINT = ${ARGS_PRINT}\n"
            "CHECK_REQUIRED = ${ARGS_CHECK_REQUIRED}\n"
            "${ARGS_PACKAGE}_FIND_REQUIRED_${_comp} = ${${ARGS_PACKAGE}_FIND_REQUIRED_${_comp}}\n"
            "${ARGS_PACKAGE}_${_comp}_SharedStatic = ${${ARGS_PACKAGE}_${_comp}_SharedStatic}")

    set(${ARGS_PACKAGE}_${_comp}_FOUND "NOTFOUND")
    if (DEFINED ARGS_LIB_PREFIX)
        # We may have shared/static components
        message(DEBUG "Including ${_comp} with possible shared/static:")
        if (${ARGS_PACKAGE}_${_comp}_SharedStatic)
            # If we know it is a shared/static target parse it appropriately
            message(DEBUG "${_comp} has to have shared/static library:")
            # Error if we want specific shared/static but it was not found
            if (ARGS_CHECK_REQUIRED AND ${ARGS_PACKAGE}_FIND_REQUIRED_${_comp} AND NOT DEFINED ARGS_FALLBACK_PREFIX AND NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/${ARGS_PACKAGE}Targets_${_comp}_${ARGS_LIB_PREFIX}.cmake)
                message(WARNING "Required ${ARGS_LIB_PREFIX} library component ${_comp} in ${ARGS_PACKAGE} NOT FOUND")
                set(${ARGS_PACKAGE}_FOUND FALSE)
                return()
            endif ()

            # Try to load shared/static library
            if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/${ARGS_PACKAGE}Targets_${_comp}_${ARGS_LIB_PREFIX}.cmake)
                # Load the correct library
                include(${CMAKE_CURRENT_LIST_DIR}/${ARGS_PACKAGE}Targets_${_comp}_${ARGS_LIB_PREFIX}.cmake
                        RESULT_VARIABLE ${ARGS_PACKAGE}_${_comp}_FOUND)
                message(VERBOSE "Including ${ARGS_PACKAGE}Targets_${_comp}_${ARGS_LIB_PREFIX}.cmake: ${${ARGS_PACKAGE}_${_comp}_FOUND}")
                # Reformat Comp_FOUND variable to TRUE/FALSE
                if (${ARGS_PACKAGE}_${_comp}_FOUND)
                    set(${ARGS_PACKAGE}_${_comp}_FOUND TRUE)
                else ()
                    set(${ARGS_PACKAGE}_${_comp}_FOUND FALSE)
                endif ()
                set(${ARGS_PACKAGE}_${_comp}_LIB_TYPE ${ARGS_LIB_PREFIX})
            elseif (ARGS_FALLBACK_PREFIX)
                # Try to load the fallback library if have one
                include(${CMAKE_CURRENT_LIST_DIR}/${ARGS_PACKAGE}Targets_${_comp}_${ARGS_FALLBACK_PREFIX}.cmake OPTIONAL
                        RESULT_VARIABLE ${ARGS_PACKAGE}_${_comp}_FOUND)
                message(VERBOSE "Including ${ARGS_PACKAGE}Targets_${_comp}_${ARGS_FALLBACK_PREFIX}.cmake: ${${ARGS_PACKAGE}_${_comp}_FOUND}")
                # Reformat Comp_FOUND variable to TRUE/FALSE
                if (${ARGS_PACKAGE}_${_comp}_FOUND)
                    set(${ARGS_PACKAGE}_${_comp}_FOUND TRUE)
                else ()
                    set(${ARGS_PACKAGE}_${_comp}_FOUND FALSE)
                endif ()
                if (${ARGS_PACKAGE}_${_comp}_FOUND)
                    set(${ARGS_PACKAGE}_${_comp}_LIB_TYPE ${ARGS_FALLBACK_PREFIX})
                endif ()
            endif ()
        else ()
            # If we don't know what type of target it is try to load shared/static
            message(DEBUG "${_comp} is of unknown type. Trying to find shared/static:")
            if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/${ARGS_PACKAGE}Targets_${_comp}_${ARGS_LIB_PREFIX}.cmake)
                # Load the correct library
                include(${CMAKE_CURRENT_LIST_DIR}/${ARGS_PACKAGE}Targets_${_comp}_${ARGS_LIB_PREFIX}.cmake
                        RESULT_VARIABLE ${ARGS_PACKAGE}_${_comp}_FOUND)
                message(VERBOSE "Including ${ARGS_PACKAGE}Targets_${_comp}_${ARGS_LIB_PREFIX}.cmake: ${${ARGS_PACKAGE}_${_comp}_FOUND}")
                # Reformat Comp_FOUND variable to TRUE/FALSE
                if (${ARGS_PACKAGE}_${_comp}_FOUND)
                    set(${ARGS_PACKAGE}_${_comp}_FOUND TRUE)
                else ()
                    set(${ARGS_PACKAGE}_${_comp}_FOUND FALSE)
                endif ()
                set(${ARGS_PACKAGE}_${_comp}_LIB_TYPE ${ARGS_LIB_PREFIX})
            elseif (ARGS_FALLBACK_PREFIX)
                # Try to load the fallback library if have one
                include(${CMAKE_CURRENT_LIST_DIR}/${ARGS_PACKAGE}Targets_${_comp}_${ARGS_FALLBACK_PREFIX}.cmake OPTIONAL
                        RESULT_VARIABLE ${ARGS_PACKAGE}_${_comp}_FOUND)
                message(VERBOSE "Including ${ARGS_PACKAGE}Targets_${_comp}_${ARGS_FALLBACK_PREFIX}.cmake: ${${ARGS_PACKAGE}_${_comp}_FOUND}")
                # Reformat Comp_FOUND variable to TRUE/FALSE
                if (${ARGS_PACKAGE}_${_comp}_FOUND)
                    set(${ARGS_PACKAGE}_${_comp}_FOUND TRUE)
                else ()
                    set(${ARGS_PACKAGE}_${_comp}_FOUND FALSE)
                endif ()
                if (${ARGS_PACKAGE}_${_comp}_FOUND)
                    set(${ARGS_PACKAGE}_${_comp}_LIB_TYPE ${ARGS_FALLBACK_PREFIX})
                endif ()
            endif ()
        endif ()

        # Handle the non-shared/static library targets
        if (DEFINED ${ARGS_PACKAGE}_${_comp}_SharedStatic AND NOT ${ARGS_PACKAGE}_${_comp}_SharedStatic)
            # If we know the package is not shared/static library handle it as the main component file
            # Error if required but missing
            if (ARGS_CHECK_REQUIRED AND ${ARGS_PACKAGE}_FIND_REQUIRED_${_comp} AND NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/${ARGS_PACKAGE}Targets_${_comp}.cmake)
                message(WARNING "Required component ${_comp} in ${ARGS_PACKAGE} NOT FOUND")
                set(${ARGS_PACKAGE}_FOUND FALSE)
                return()
            endif ()
            # Include and set it as found or not accordingly
            include(${CMAKE_CURRENT_LIST_DIR}/${ARGS_PACKAGE}Targets_${_comp}.cmake OPTIONAL
                    RESULT_VARIABLE ${ARGS_PACKAGE}_${_comp}_FOUND)
            message(VERBOSE "Including ${ARGS_PACKAGE}Targets_${_comp}.cmake: ${${ARGS_PACKAGE}_${_comp}_FOUND}")
            # Reformat Comp_FOUND variable to TRUE/FALSE
            if (${ARGS_PACKAGE}_${_comp}_FOUND)
                set(${ARGS_PACKAGE}_${_comp}_FOUND TRUE)
            else ()
                set(${ARGS_PACKAGE}_${_comp}_FOUND FALSE)
            endif ()
        else ()
            # Otherwise still try to parse the non shared/static library
            message(VERBOSE "Trying to include ${ARGS_PACKAGE}Targets_${_comp}.cmake (${ARGS_PACKAGE}_${_comp}_FOUND=${${ARGS_PACKAGE}_${_comp}_FOUND})")
            if (DEFINED ${ARGS_PACKAGE}_${_comp}_SharedStatic)
                # We know it is a static/shared library, don't mess with ${${ARGS_PACKAGE}_${_comp}_FOUND}
                include(${CMAKE_CURRENT_LIST_DIR}/${ARGS_PACKAGE}Targets_${_comp}.cmake OPTIONAL)
            else ()
                # We don't know what type the component is. Try to set it to found ${${ARGS_PACKAGE}_${_comp}_FOUND}
                include(${CMAKE_CURRENT_LIST_DIR}/${ARGS_PACKAGE}Targets_${_comp}.cmake OPTIONAL
                        RESULT_VARIABLE _temp_flag)
                # Check if either it was static/shared or arbitrary component
                if (${ARGS_PACKAGE}_${_comp}_FOUND OR _temp_flag)
                    set(${ARGS_PACKAGE}_${_comp}_FOUND TRUE)
                else ()
                    set(${ARGS_PACKAGE}_${_comp}_FOUND FALSE)
                endif ()
                message(DEBUG "Include result:\n"
                        "_temp_flag=${_temp_flag}\n"
                        "${ARGS_PACKAGE}_${_comp}_FOUND = ${${ARGS_PACKAGE}_${_comp}_FOUND}")
                # Check if it managed to load
                if (ARGS_CHECK_REQUIRED AND ${ARGS_PACKAGE}_FIND_REQUIRED_${_comp} AND NOT ${ARGS_PACKAGE}_${_comp}_FOUND)
                    message(WARNING "Required component ${_comp} in ${ARGS_PACKAGE} NOT FOUND")
                    set(${ARGS_PACKAGE}_FOUND FALSE)
                    return()
                endif ()
            endif ()
        endif ()
    else ()
        # We do not have shared/static components
        message(VERBOSE "Including ${_comp} without shared/static:")
        # Check if required component is installed
        if (ARGS_CHECK_REQUIRED AND ${ARGS_PACKAGE}_FIND_REQUIRED_${_comp} AND NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/${ARGS_PACKAGE}Targets_${_comp}.cmake)
            message(WARNING "Required component ${_comp} in ${ARGS_PACKAGE} NOT FOUND")
            set(${ARGS_PACKAGE}_FOUND FALSE)
            return()
        endif ()
        include(${CMAKE_CURRENT_LIST_DIR}/${ARGS_PACKAGE}Targets_${_comp}.cmake OPTIONAL
                RESULT_VARIABLE ${ARGS_PACKAGE}_${_comp}_FOUND)
        # Reformat Comp_FOUND variable to TRUE/FALSE
        if (${ARGS_PACKAGE}_${_comp}_FOUND)
            set(${ARGS_PACKAGE}_${_comp}_FOUND TRUE)
        else ()
            set(${ARGS_PACKAGE}_${_comp}_FOUND FALSE)
        endif ()
    endif ()

    # Print result
    if (ARGS_PRINT)
        set(_message "Component ${_comp} of ${ARGS_PACKAGE}:")
        if (${ARGS_PACKAGE}_${_comp}_FOUND)
            set(_message "${_message} FOUND")
        else ()
            set(_message "${_message} NOT FOUND")
        endif ()
        if (DEFINED ${ARGS_PACKAGE}_${_comp}_LIB_TYPE)
            set(_message "${_message} (LIB_TYPE: ${${ARGS_PACKAGE}_${_comp}_LIB_TYPE})")
        endif ()
        message(STATUS "${_message}")
    endif ()
    message(DEBUG
            "${ARGS_PACKAGE}_${_comp}_FOUND = ${${ARGS_PACKAGE}_${_comp}_FOUND}\n"
            "${ARGS_PACKAGE}_${_comp}_LIB_TYPE = ${${ARGS_PACKAGE}_${_comp}_LIB_TYPE}")
endmacro()

# Define a portable generic find_package macro
macro(find_package_with_comps)
    # Named arguments::
    #   PACKAGE (string) [${PROJECT_NAME}]: Project name. Also used as prefix
    #   COMPONENTS (list<string>): List of supported components (without deprecated components). If empty will just load global target
    #   DEPRECATED_COMPONENTS (list<string>): List of deprecated components.
    # Options
    #   PRINT: Print results when loading each component
    #   LOAD_ALL_DEFAULT: Load all supported components if no components are passed
    #   HAVE_GLOBAL: Whether global targets file is defined (see below)
    #   HAVE_GLOBAL_SHARED_STATIC: Whether global static/shared targets file is defined (see below)
    # Assumptions ::
    #   Defined variables in ${PACKAGE}Config.cmake file::
    #     ${PACKAGE}_<comp>_Replacement (string): Replacement components for component <comp>. If not defined will ignore.
    #     ${PACKAGE}_<comp>_SharedStatic (bool): Whether the component should have static/shared targets. If not defined will try to find target.
    #   Name format of target files:
    #     ${PACKAGE}Targets.cmake: Global targets
    #     ${PACKAGE}Targets-{static/shared}.cmake: Global static/shared library targets
    #     ${PACKAGE}Targets-<comp>.cmake: Component targets
    #     ${PACKAGE}Targets-{static/shared}-<comp>.cmake: Static/Shared component library targets
    #
    # For a reference to the find_package variables check:
    # https://cmake.org/cmake/help/latest/command/find_package.html#package-file-interface-variables

    cmake_parse_arguments(ARGS
            "PRINT;LOAD_ALL_DEFAULT;HAVE_GLOBAL;HAVE_GLOBAL_SHARED_STATIC"
            "PACKAGE"
            "COMPONENTS;DEPRECATED_COMPONENTS"
            ${ARGN})
    message(VERBOSE "Running find_package_with_comps:")

    ## Basic checks
    set(${ARGS_PACKAGE}_FOUND TRUE)
    if (NOT DEFINED ARGS_PACKAGE)
        set(ARGS_PACKAGE "${CMAKE_FIND_PACKAGE_NAME}")
    endif ()
    if (NOT DEFINED ARGS_COMPONENTS AND DEFINED ${ARGS_PACKAGE}_Supported_Comps)
        set(ARGS_COMPONENTS ${${ARGS_PACKAGE}_Supported_Comps})
    endif ()
    if (NOT DEFINED ARGS_DEPRECATED_COMPONENTS AND DEFINED ${ARGS_PACKAGE}_Deprecated_Comps)
        set(ARGS_DEPRECATED_COMPONENTS ${${ARGS_PACKAGE}_Deprecated_Comps})
    elseif (NOT DEFINED ARGS_DEPRECATED_COMPONENTS)
        set(ARGS_DEPRECATED_COMPONENTS "")
    endif ()
    message(DEBUG
            "PACKAGE = ${ARGS_PACKAGE}\n"
            "COMPONENTS = ${ARGS_COMPONENTS}\n"
            "DEPRECATED_COMPONENTS = ${ARGS_DEPRECATED_COMPONENTS}\n"
            "PRINT = ${ARGS_PRINT}\n"
            "LOAD_ALL_DEFAULT = ${ARGS_LOAD_ALL_DEFAULT}\n"
            "HAVE_GLOBAL = ${ARGS_HAVE_GLOBAL}\n"
            "HAVE_GLOBAL_SHARED_STATIC = ${ARGS_HAVE_GLOBAL_SHARED_STATIC}\n"
            "${ARGS_PACKAGE}_FIND_COMPONENTS = ${${ARGS_PACKAGE}_FIND_COMPONENTS}")
    if (CMAKE_MESSAGE_LOG_LEVEL STREQUAL "DEBUG")
        file(GLOB _cmake_target_files RELATIVE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/*)
        message(DEBUG
                "CMAKE_CURRENT_LIST_DIR = ${CMAKE_CURRENT_LIST_DIR}\n"
                "_cmake_target_files::")
        foreach (_file IN LISTS _cmake_target_files)
            message(DEBUG "  ${_file}")
        endforeach ()
    endif ()
    if (NOT DEFINED ARGS_COMPONENTS)
        # End early if no component logic is defined
        if (NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/${ARGS_PACKAGE}Targets.cmake)
            message(WARNING "Report to package distributor: No ${ARGS_PACKAGE}Targets.cmake file bundled.")
            set(${ARGS_PACKAGE}_FOUND FALSE)
        else ()
            include(${CMAKE_CURRENT_LIST_DIR}/${ARGS_PACKAGE}Targets.cmake)
            if (ARGS_PRINT)
                message(STATUS "Found package: ${ARGS_PACKAGE}")
            endif ()
        endif ()
        return()
    else ()
        if ("shared" IN_LIST ARGS_COMPONENTS AND "static" IN_LIST ARGS_COMPONENTS)
            set(_with_shared_static TRUE)
        elseif (NOT "shared" IN_LIST ARGS_COMPONENTS AND NOT "static" IN_LIST ARGS_COMPONENTS)
            set(_with_shared_static FALSE)
        else ()
            message(WARNING "Report to package maintainer: Defined only shared or static components but not both.")
            set(${ARGS_PACKAGE}_FOUND FALSE)
            return()
        endif ()
    endif ()

    # Set subfunction options
    set(sub_func_ARGS "PACKAGE;${ARGS_PACKAGE}")
    if (ARGS_PRINT)
        list(APPEND sub_func_ARGS PRINT)
    endif ()

    # Check for unknown components
    # TODO: Not checking for different name used in ${CMAKE_FIND_PACKAGE_NAME}
    foreach (comp IN LISTS ${ARGS_PACKAGE}_FIND_COMPONENTS)
        if (NOT comp IN_LIST ARGS_COMPONENTS)
            if (comp IN_LIST ARGS_DEPRECATED_COMPONENTS)
                if (DEFINED ${ARGS_PACKAGE}_${comp}_Replacement)
                    message(DEPRECATION "Using deprecated component in ${ARGS_PACKAGE}: Replace ${comp} with ${${ARGS_PACKAGE}_${comp}_Replacement}")
                    list(APPEND ${ARGS_PACKAGE}_FIND_COMPONENTS "${${ARGS_PACKAGE}_${comp}_Replacement}")
                else ()
                    message(DEPRECATION "Using deprecated component in ${ARGS_PACKAGE}: This has now no effect")
                endif ()
                list(REMOVE_ITEM ${ARGS_PACKAGE}_FIND_COMPONENTS "${comp}")
            else ()
                message(WARNING "Unsupported component passed to find_package(${ARGS_PACKAGE}): ${comp}")
                set(${ARGS_PACKAGE}_FOUND FALSE)
                return()
            endif ()
        endif ()
    endforeach ()

    # Handle shared and static components
    if (_with_shared_static)
        # Error if both shared and static components are requested
        if ("shared" IN_LIST ${ARGS_PACKAGE}_FIND_COMPONENTS AND "static" IN_LIST ${ARGS_PACKAGE}_FIND_COMPONENTS)
            message(WARNING "Passed both shared and static components in ${ARGS_PACKAGE}, but that is ambiguous")
            set(${ARGS_PACKAGE}_FOUND FALSE)
            return()
        endif ()

        # Get the shared/static targets to be loaded
        if ("shared" IN_LIST ${ARGS_PACKAGE}_FIND_COMPONENTS)
            list(REMOVE_ITEM ${ARGS_PACKAGE}_FIND_COMPONENTS "shared")
            set(_libPrefix "shared")
        elseif ("static" IN_LIST ${ARGS_PACKAGE}_FIND_COMPONENTS)
            list(REMOVE_ITEM ${ARGS_PACKAGE}_FIND_COMPONENTS "static")
            set(_libPrefix "static")
        elseif (DEFINED ${ARGS_PACKAGE}_SHARED_LIBS)
            if (${ARGS_PACKAGE}_SHARED_LIBS)
                set(_libPrefix "shared")
            else ()
                set(_libPrefix "static")
            endif ()
        else ()
            if (DEFINED BUILD_SHARED_LIBS AND NOT BUILD_SHARED_LIBS)
                set(_libPrefix "static")
                set(_fallbackPrefix "shared")
            else ()
                set(_libPrefix "shared")
                set(_fallbackPrefix "static")
            endif ()
        endif ()
        list(APPEND sub_func_ARGS LIB_PREFIX ${_libPrefix})
        if (DEFINED _fallbackPrefix)
            list(APPEND sub_func_ARGS FALLBACK_PREFIX ${_fallbackPrefix})
        endif ()
    endif ()
    message(DEBUG
            "_with_shared_static = ${_with_shared_static}\n"
            "_libPrefix = ${_libPrefix}\n"
            "_fallbackPrefix = ${_fallbackPrefix}\n"
            "sub_func_ARGS = ${sub_func_ARGS}")

    # Parse global targets
    # Note: These have to be parsed before components in case components depend on it.
    #       Global targets should not have component dependencies
    if (ARGS_HAVE_GLOBAL)
        message(VERBOSE "Trying to include ${ARGS_PACKAGE}Targets.cmake")
        if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/${ARGS_PACKAGE}Targets.cmake)
            include(${CMAKE_CURRENT_LIST_DIR}/${ARGS_PACKAGE}Targets.cmake)
        else ()
            message(WARNING "Report to package distributor: No ${ARGS_PACKAGE}Targets.cmake file bundled.")
            set(${ARGS_PACKAGE}_FOUND FALSE)
            return()
        endif ()
    endif ()

    # Parse global static/shared targets
    if (_with_shared_static AND ARGS_HAVE_GLOBAL_SHARED_STATIC)
        message(VERBOSE "Trying to include ${ARGS_PACKAGE}Targets_${_libPrefix}.cmake (or ${_fallbackPrefix})")
        if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/${ARGS_PACKAGE}Targets_${_libPrefix}.cmake)
            include(${CMAKE_CURRENT_LIST_DIR}/${ARGS_PACKAGE}Targets_${_libPrefix}.cmake)
            set(${ARGS_PACKAGE}_LIB_TYPE ${_libPrefix})
        elseif (DEFINED _fallbackPrefix AND EXISTS ${CMAKE_CURRENT_LIST_DIR}/${ARGS_PACKAGE}Targets_${_fallbackPrefix}.cmake)
            include(${CMAKE_CURRENT_LIST_DIR}/${ARGS_PACKAGE}Targets_${_fallbackPrefix}.cmake)
            set(${ARGS_PACKAGE}_LIB_TYPE ${_fallbackPrefix})
        else ()
            message(WARNING "${ARGS_PACKAGE} package is missing file: ${ARGS_PACKAGE}Targets_${_libPrefix}.cmake")
            set(${ARGS_PACKAGE}_FOUND FALSE)
            return()
        endif ()
    endif ()

    # Parse components
    message(DEBUG "Before components parsed: ${ARGS_PACKAGE}_FOUND = ${${ARGS_PACKAGE}_FOUND}")
    if (${ARGS_PACKAGE}_FIND_COMPONENTS)
        # If specific components are passed handle only these components
        message(VERBOSE "Trying to search for specific components: ${${ARGS_PACKAGE}_FIND_COMPONENTS}")
        foreach (comp IN LISTS ${ARGS_PACKAGE}_FIND_COMPONENTS)
            try_get_package_comp(${comp} CHECK_REQUIRED ${sub_func_ARGS})
            if (NOT ${ARGS_PACKAGE}_FOUND)
                message(DEBUG "${comp} set ${ARGS_PACKAGE}_FOUND to false type (${ARGS_PACKAGE}_FOUND = ${${ARGS_PACKAGE}_FOUND})")
                return()
            endif ()
        endforeach ()
    elseif (ARGS_LOAD_ALL_DEFAULT)
        # If no components are passed and ${_load_all_default} is true, get all supported components
        list(REMOVE_ITEM ARGS_COMPONENTS "static" "shared")
        message(VERBOSE "Trying to search for all components: ${ARGS_COMPONENTS}")
        foreach (comp IN LISTS ARGS_COMPONENTS)
            try_get_package_comp(${comp} ${sub_func_ARGS})
            if (NOT ${ARGS_PACKAGE}_FOUND)
                message(DEBUG "${comp} set ${ARGS_PACKAGE}_FOUND to false type (${ARGS_PACKAGE}_FOUND = ${${ARGS_PACKAGE}_FOUND})")
                return()
            endif ()
        endforeach ()
    endif ()
    message(DEBUG "After components parsed: ${ARGS_PACKAGE}_FOUND = ${${ARGS_PACKAGE}_FOUND}")

    # Final print status
    if (ARGS_PRINT)
        message(STATUS "Found package: ${ARGS_PACKAGE}")
    endif ()
endmacro()

function(export_components)
    # Named arguments::
    #   PROJECT (string) [${PROJECT_NAME}]: Project name. Also used as prefix
    #   COMPONENT (string): Target component
    #   LIB_TYPE (string) : Whether the target is shared/static or general
    set(possible_LIB_TYPE "static" "shared")
    #	cmake_parse_arguments(ARGS "" "PROJECT;COMPONENT;LIB_TYPE" "" ${ARGN})
    cmake_parse_arguments(PARSE_ARGV 0 ARGS
            ""
            "PROJECT;COMPONENT;LIB_TYPE"
            "")
    if (NOT DEFINED ARGS_PROJECT)
        set(ARGS_PROJECT "${PROJECT_NAME}")
    endif ()
    if (DEFINED ARGS_LIB_TYPE AND NOT ARGS_LIB_TYPE IN_LIST possible_LIB_TYPE)
        message(FATAL_ERROR "Unknown LIB_TYPE passed: ${ARGS_LIB_TYPE}")
    endif ()
    if (DEFINED ARGS_COMPONENT AND ARGS_COMPONENT)
        set(CmakeTarget "${ARGS_PROJECT}Targets-${ARGS_COMPONENT}")
        if (DEFINED ARGS_LIB_TYPE)
            set(CmakeTargetFile "${ARGS_PROJECT}Targets_${ARGS_COMPONENT}_${ARGS_LIB_TYPE}.cmake")
        else ()
            set(CmakeTargetFile "${ARGS_PROJECT}Targets_${ARGS_COMPONENT}.cmake")
        endif ()
    else ()
        set(CmakeTarget "${ARGS_PROJECT}Targets")
        if (DEFINED ARGS_LIB_TYPE)
            set(CmakeTargetFile "${ARGS_PROJECT}Targets_${ARGS_LIB_TYPE}.cmake")
        else ()
            set(CmakeTargetFile "${ARGS_PROJECT}Targets.cmake")
        endif ()
    endif ()
    install(EXPORT ${CmakeTarget}
            FILE ${CmakeTargetFile}
            NAMESPACE ${ARGS_PROJECT}::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${ARGS_PROJECT}
    )
    export(EXPORT ${CmakeTarget}
            FILE ${${ARGS_PROJECT}_BINARY_DIR}/${CmakeTargetFile}
            NAMESPACE ${ARGS_PROJECT}::)
endfunction()
