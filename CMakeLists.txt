cmake_minimum_required(VERSION 3.15)
project(Spglib
		VERSION 2.1.0
		LANGUAGES C)

option(SPGLIB_WITH_Fortran "enable f08 interface" OFF)
option(SPGLIB_WITH_Python "build python buindings" OFF)
option(SPGLIB_USE_OMP "Build with OpenMP support" OFF)
set(SPGLIB_USE_SANITIZER "" CACHE STRING "Sanitizer used in compilation")

# Sanity check if project is imported
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	option(SPGLIB_WITH_TESTS "build unit tests" ON)
	option(SPGLIB_SHARED_LIBS "Build Spglib as a shared library" ON)
else ()
	option(SPGLIB_WITH_TESTS "build unit tests" OFF)
	option(SPGLIB_SHARED_LIBS "Build Spglib as a shared library" OFF)
endif ()

# Include basic tools
include(CMakePackageConfigHelpers)
if (UNIX)
	include(GNUInstallDirs)
endif ()

# Define basic parameters
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif ()
set(BUILD_SHARED_LIBS ${SPGLIB_SHARED_LIBS})
if (NOT CMAKE_Fortran_MODULE_DIRECTORY)
	set(CMAKE_Fortran_MODULE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/fortran_mods)
endif ()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# Windows setup, make sure there is a .lib file to link to
if (SKBUILD AND WIN32)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif ()
if (SPGLIB_WITH_Fortran)
	enable_language(Fortran)
endif ()

# Include third-party libraries
if (SPGLIB_USE_OMP)
	find_package(OpenMP REQUIRED COMPONENTS C)
endif ()

# Main project
add_library(Spglib_symspg)
set_target_properties(Spglib_symspg PROPERTIES
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_VERSION_MAJOR}
		EXPORT_NAME symspg
		OUTPUT_NAME symspg)
add_library(Spglib::symspg ALIAS Spglib_symspg)
add_subdirectory(src)

if (SPGLIB_WITH_Fortran)
	add_subdirectory(fortran)
endif ()
if (SPGLIB_WITH_Python)
	add_subdirectory(python)
endif ()
if (SPGLIB_WITH_TESTS)
	add_subdirectory(test)
endif ()

# Installation
# TODO: Temporarily disable normal cmake install targets when using scikit-build-core
#  Check how bundled cmake searches CMAKE_MODULE_DIR and add accordingly
#  (use SKBUILD_PLATLIB_DIR for install root)
if (NOT SKBUILD)
	# pkg-config files
	configure_file(cmake/spglib.pc.in spglib.pc @ONLY)
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/spglib.pc
			DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
			COMPONENT Spglib_Development)

	# cmake export files
	write_basic_package_version_file(
			${CMAKE_CURRENT_BINARY_DIR}/SpglibConfigVersion.cmake
			VERSION ${PROJECT_VERSION}
			COMPATIBILITY SameMajorVersion)
	configure_package_config_file(
			cmake/SpglibConfig.cmake.in
			${CMAKE_CURRENT_BINARY_DIR}/SpglibConfig.cmake
			INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Spglib)
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SpglibConfigVersion.cmake
			${CMAKE_CURRENT_BINARY_DIR}/SpglibConfig.cmake
			DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Spglib
			COMPONENT Spglib_Development)
	install(EXPORT SpglibTargets
			FILE SpglibTargets.cmake
			NAMESPACE Spglib::
			DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Spglib
			COMPONENT Spglib_Development)
	export(EXPORT SpglibTargets
			FILE ${CMAKE_CURRENT_BINARY_DIR}/cmake/SpglibTargets.cmake
			NAMESPACE Spglib::)
endif ()
