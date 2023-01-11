find_package(Python REQUIRED COMPONENTS Interpreter Development NumPy)
find_package(PythonExtensions REQUIRED)
include(UsePythonExtensions)
add_python_extension(_spglib
		SOURCES _spglib.c
		LINK_LIBRARIES symspg Python::NumPy)
install(TARGETS symspg LIBRARY DESTINATION spglib)
install(TARGETS _spglib LIBRARY DESTINATION spglib)
