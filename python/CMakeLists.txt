Python3_add_library(Spglib_python MODULE WITH_SOABI _spglib.c)
set_target_properties(Spglib_python PROPERTIES
		OUTPUT_NAME _spglib)
target_link_libraries(Spglib_python PRIVATE
		Spglib_symspg Python3::NumPy)
if (NOT Python_INSTALL_DIR)
	if (SKBUILD)
		# If built with scikit-build-core, let it handle the installation
		set(Python_INSTALL_DIR ".")
	else ()
		# Otherwise try to install in current python executable's setup
		set(Python_INSTALL_DIR ${Python3_SITEARCH}/spglib)
	endif ()
endif ()
if (SPGLIB_INSTALL)
	if (WIN32)
		# Windows needs RUNTIME as well to install the .dll file to
		install(TARGETS Spglib_symspg
				LIBRARY DESTINATION ${Python_INSTALL_DIR} COMPONENT Spglib_Runtime
				PUBLIC_HEADER DESTINATION ${Python_INSTALL_DIR} COMPONENT Spglib_Runtime
				RUNTIME DESTINATION ${Python_INSTALL_DIR} COMPONENT Spglib_Runtime)
	else ()
		# TODO: Cmake forces to install PUBLIC_HEADER when defined
		# https://gitlab.kitware.com/cmake/cmake/-/issues/24326
		install(TARGETS Spglib_symspg
				LIBRARY DESTINATION ${Python_INSTALL_DIR} COMPONENT Spglib_Runtime
				PUBLIC_HEADER DESTINATION ${Python_INSTALL_DIR} COMPONENT Spglib_Runtime)
	endif ()
	install(TARGETS Spglib_python
			LIBRARY DESTINATION ${Python_INSTALL_DIR} COMPONENT Spglib_Runtime)

	if (NOT SKBUILD)
		install(DIRECTORY spglib/ DESTINATION ${Python_INSTALL_DIR} COMPONENT Spglib_Runtime)
	endif ()
endif ()
