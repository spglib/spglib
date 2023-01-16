find_package(Python3 COMPONENTS REQUIRED Interpreter Development.Module NumPy)
Python3_add_library(python_api MODULE WITH_SOABI _spglib.c)
set_target_properties(python_api PROPERTIES
		OUTPUT_NAME _spglib)
target_link_libraries(python_api PRIVATE
		symspg Python3::NumPy)
if (NOT Python_INSTALL_DIR)
	if (SKBUILD)
		# If built with scikit-build-core, let it handle the installation
		set(Python_INSTALL_DIR ".")
	else()
		# Otherwise try to install in current python executable's setup
		set(Python_INSTALL_DIR ${Python3_SITEARCH}/spglib)
	endif()
endif ()
if (WIN32)
	# Windows needs RUNTIME as well to install the .dll file to
	install(TARGETS symspg
			LIBRARY DESTINATION ${Python_INSTALL_DIR}
			PUBLIC_HEADER DESTINATION ${Python_INSTALL_DIR}
			RUNTIME DESTINATION ${Python_INSTALL_DIR})
else()
	# TODO: Cmake forces to install PUBLIC_HEADER when defined
	# https://gitlab.kitware.com/cmake/cmake/-/issues/24326
	install(TARGETS symspg
			LIBRARY DESTINATION ${Python_INSTALL_DIR}
			PUBLIC_HEADER DESTINATION ${Python_INSTALL_DIR})
endif()
install(TARGETS python_api LIBRARY DESTINATION ${Python_INSTALL_DIR})
install(DIRECTORY spglib/ DESTINATION ${Python_INSTALL_DIR})