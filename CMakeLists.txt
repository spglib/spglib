cmake_minimum_required(VERSION 3.15)
project(spglib
        VERSION 2.1.0
        LANGUAGES C)

option(WITH_Fortran "enable f08 interface" OFF)
option(WITH_Python "build python buindings" OFF)
option(USE_OMP "Build with OpenMP support" OFF)
set(USE_SANITIZER "" CACHE STRING "Sanitizer used in compilation")

# Sanity check if project is imported
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	option(WITH_TESTS "build unit tests" ON)
else ()
	option(WITH_TESTS "build unit tests" OFF)
endif ()

# Include basic tools
include(CMakePackageConfigHelpers)
if (UNIX)
	include(GNUInstallDirs)
endif()

# Define basic parameters
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif (NOT CMAKE_BUILD_TYPE)
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# Windows setup, make sure there is a .lib file to link to
if (SKBUILD AND WIN32)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()
if (WITH_Fortran)
	enable_language(Fortran)
endif()

# Include third-party libraries
if (USE_OMP)
	find_package(OpenMP REQUIRED COMPONENTS C)
endif ()

# Main project
add_library(symspg)
add_subdirectory(src)

if (WITH_Fortran)
	add_subdirectory(fortran)
endif ()
if (WITH_Python)
	add_subdirectory(python)
endif ()
if (WITH_TESTS)
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
			COMPONENT pkgconfig)

	# cmake export files
	write_basic_package_version_file(
			${CMAKE_CURRENT_BINARY_DIR}/spglibConfigVersion.cmake
			VERSION ${PROJECT_VERSION}
			COMPATIBILITY SameMajorVersion)
	configure_package_config_file(
			cmake/spglibConfig.cmake.in
			${CMAKE_CURRENT_BINARY_DIR}/spglibConfig.cmake
			INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/spglib)
	install(
			FILES ${CMAKE_CURRENT_BINARY_DIR}/spglibConfigVersion.cmake
			${CMAKE_CURRENT_BINARY_DIR}/spglibConfig.cmake
			DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/spglib)
	install(EXPORT spglibTargets
			FILE spglibTargets.cmake
			NAMESPACE spglib::
			DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/spglib)
	export(EXPORT spglibTargets
			FILE ${CMAKE_CURRENT_BINARY_DIR}/cmake/spglibTargets.cmake
			NAMESPACE spglib::)
endif ()