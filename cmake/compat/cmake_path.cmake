# Note: This is not an actual replacement of cmake_path, just a minimal implementation for
# spglib project
function(cmake_path operator path_var)
	if (NOT ${operator} STREQUAL APPEND)
		message(FATAL_ERROR Unsupported operator)
	endif ()

	set(ARGS_Options)
	set(ARGS_OneValue "OUTPUT_VARIABLE")
	set(ARGS_MultiValue)
	cmake_parse_arguments(ARGS "${ARGS_Options}" "${ARGS_OneValue}" "${ARGS_MultiValue}" ${ARGN})

	if(NOT DEFINED ARGS_OUTPUT_VARIABLE)
		set(ARGS_OUTPUT_VARIABLE ${path_var})
	endif ()
	set(${ARGS_OUTPUT_VARIABLE} ${${path_var}})
	foreach (path IN LISTS ARGS_UNPARSED_ARGUMENTS)
		set(${ARGS_OUTPUT_VARIABLE} "${${ARGS_OUTPUT_VARIABLE}}/${path}")
	endforeach ()
endfunction()
