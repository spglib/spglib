# TODO: Make package REQUIRED when a different solution is found to
# https://github.com/pypa/cibuildwheel/issues/639#issuecomment-818020937
find_package(Python COMPONENTS Interpreter Development NumPy)
Python_add_library(python_api MODULE WITH_SOABI _spglib.c)
set_target_properties(python_api PROPERTIES
		OUTPUT_NAME _spglib)
target_link_libraries(python_api PRIVATE
		symspg Python::NumPy)
if (NOT Python_INSTALL_DIR)
	if (SKBUILD)
		# If built with scikit-build-core, let it handle the installation
		set(Python_INSTALL_DIR ".")
	else()
		# Otherwise try to install in current python executable's setup
		set(Python_INSTALL_DIR ${Python_SITEARCH}/spglib)
	endif()
endif ()
install(TARGETS symspg LIBRARY DESTINATION ${Python_INSTALL_DIR})
install(TARGETS python_api LIBRARY DESTINATION ${Python_INSTALL_DIR})
install(DIRECTORY spglib/ DESTINATION ${Python_INSTALL_DIR})