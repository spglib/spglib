find_package(Python REQUIRED COMPONENTS Interpreter Development NumPy)
add_library(python_api SHARED _spglib.c)
set(lib_name "_spglib")
if (CPython_Suffix)
	set(lib_name "${lib_name}.${CPython_Suffix}")
endif ()
set_target_properties(
		python_api
		PROPERTIES
		PREFIX ""
		OUTPUT_NAME ${lib_name}
		LINKER_LANGUAGE C
)

if (WIN32)
	set_target_properties(
			python_api
			PROPERTIES
			SUFFIX ".pyd"
	)
endif ()

target_link_libraries(python_api PUBLIC symspg Python::Python Python::NumPy)
