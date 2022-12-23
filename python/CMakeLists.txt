find_package(Python REQUIRED COMPONENTS Interpreter Development NumPy)
add_library(python_api SHARED _spglib.c)
set_target_properties(
		python_api
		PROPERTIES
		PREFIX ""
		OUTPUT_NAME _spglib
		LINKER_LANGUAGE C
)
target_link_libraries(python_api PUBLIC symspg Python::Python Python::NumPy)
