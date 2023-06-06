cmake_minimum_required(VERSION 3.15)
project(Spglib
		VERSION 2.1.0
		LANGUAGES C)

option(SPGLIB_WITH_Fortran "Spglib: Build Fortran interface" OFF)
option(SPGLIB_WITH_Python "Spglib: Build Python interface" OFF)
option(SPGLIB_USE_OMP "Spglib: Build with OpenMP support" OFF)
set(SPGLIB_USE_SANITIZER "" CACHE STRING "Spglib: Sanitizer used in compilation")
mark_as_advanced(SPGLIB_USE_SANITIZER)
option(SPGLIB_TEST_COVERAGE "Spglib: Test with coverage" OFF)
mark_as_advanced(SPGLIB_TEST_COVERAGE)

# Sanity check if project is imported
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	option(SPGLIB_WITH_TESTS "Spglib: Build unit tests" ON)
	option(SPGLIB_SHARED_LIBS "Spglib: Build as a shared library" ON)
else ()
	option(SPGLIB_WITH_TESTS "Spglib: Build unit tests" OFF)
	option(SPGLIB_SHARED_LIBS "Spglib: Build as a shared library" OFF)
endif ()

# Include basic tools
include(CMakePackageConfigHelpers)
if (UNIX)
	include(GNUInstallDirs)
endif ()
include(FetchContent)
include(cmake/PackageCompsHelper.cmake)

# Define basic parameters
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif ()
set(BUILD_SHARED_LIBS ${SPGLIB_SHARED_LIBS})
if (BUILD_SHARED_LIBS)
	set(SPGLIB_LIB_TYPE "shared")
else ()
	set(SPGLIB_LIB_TYPE "static")
endif ()
if (NOT CMAKE_Fortran_MODULE_DIRECTORY)
	set(CMAKE_Fortran_MODULE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/fortran_mods)
endif ()
message(STATUS "Spglib:: Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_MACOSX_RPATH 1)
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
if (SPGLIB_TEST_COVERAGE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
	set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} --coverage -ftest-forall-temp")
#	set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} --coverage")
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
	enable_testing()
	if (SPGLIB_TEST_COVERAGE)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
		set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} --coverage")
	endif ()
	add_subdirectory(test)
endif ()

# Installation
# TODO: Temporarily disable normal cmake install targets when using scikit-build-core
#  Check how bundled cmake searches CMAKE_MODULE_DIR and add accordingly
#  (use SKBUILD_PLATLIB_DIR for install root)
# Note: Completely disabling install when project is included as an ExternalProject
#       Currently unclear how to expose it and protect it from overwriting other installations
#       Please contact if such functionality is needed
if (NOT SKBUILD AND CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
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
			cmake/PackageCompsHelper.cmake
			DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Spglib
			COMPONENT Spglib_Development)
	export_components(LIB_TYPE ${SPGLIB_LIB_TYPE})
endif ()
