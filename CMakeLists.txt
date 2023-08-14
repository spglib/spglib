cmake_minimum_required(VERSION 3.15)
# CMake version compatibility
# TODO: Remove when cmake 3.25 is commonly distributed
if (POLICY CMP0140)
	cmake_policy(SET CMP0140 NEW)
endif ()

#[==============================================================================================[
#                                    Basic project defintion                                    #
]==============================================================================================]

project(Spglib
		VERSION 2.1.0
		LANGUAGES C)

# Back-porting to PROJECT_IS_TOP_LEVEL to older cmake
# TODO: Remove when requiring cmake >= 3.21
if (NOT DEFINED Spglib_IS_TOP_LEVEL)
	if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
		set(PROJECT_IS_TOP_LEVEL ON)
	else ()
		set(PROJECT_IS_TOP_LEVEL OFF)
	endif ()
endif ()

# Specify C standard. This can be overwritten using `-DCMAKE_C_STANDARD`
# https://discourse.cmake.org/t/cmake-lang-standard-and-fetchcontent-cache-variable-interaction/8348
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif ()

#[==============================================================================================[
#                                            Options                                            #
]==============================================================================================]

# Boilerplate default values
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(_Spglib_default_debug ON)
else ()
	set(_Spglib_default_debug OFF)
endif ()

#
option(SPGLIB_WITH_Fortran "Spglib: Build Fortran interface" OFF)
option(SPGLIB_WITH_Python "Spglib: Build Python interface" OFF)
option(SPGLIB_USE_OMP "Spglib: Build with OpenMP support" OFF)
option(SPGLIB_WITH_TESTS "Spglib: Build unit tests" ${PROJECT_IS_TOP_LEVEL})
option(SPGLIB_SHARED_LIBS "Spglib: Build as a shared library" ${PROJECT_IS_TOP_LEVEL})
option(SPGLIB_INSTALL "Spglib: Install project" ${PROJECT_IS_TOP_LEVEL})
option(SPGLIB_TEST_COVERAGE "Spglib: Test with coverage" OFF)
mark_as_advanced(SPGLIB_TEST_COVERAGE)
set(SPGLIB_USE_SANITIZER "" CACHE STRING "Spglib: Sanitizer used in compilation")
mark_as_advanced(SPGLIB_USE_SANITIZER)
option(SPGLIB_WARNINGS "Spglib: Enable warning messages" ON)
mark_as_advanced(SPGLIB_WITH_WARNINGS)
option(SPGLIB_DEBUG "Spglib: Build in debug mode" ${_Spglib_default_debug})
mark_as_advanced(SPGLIB_DEBUG)

#[==============================================================================================[
#                                     Project configuration                                     #
]==============================================================================================]

# Compatibilities
# TODO: Remove when requiring cmake >= 3.20
if (NOT COMMAND cmake_path)
	# Note: This is not an actual replacement of cmake_path, just a minimal implementation for
	# this project
	include(cmake/compat/cmake_path.cmake)
endif ()

# Include basic tools
include(cmake/PackageCompsHelper.cmake)
include(FetchContent)
if (SPGLIB_INSTALL)
	include(CMakePackageConfigHelpers)
	if (UNIX)
		include(GNUInstallDirs)
	endif ()
endif ()

# Define basic parameters
set(BUILD_SHARED_LIBS ${SPGLIB_SHARED_LIBS})
if (BUILD_SHARED_LIBS)
	set(Spglib_LIB_TYPE "shared")
else ()
	set(Spglib_LIB_TYPE "static")
endif ()
if (NOT CMAKE_Fortran_MODULE_DIRECTORY)
	set(CMAKE_Fortran_MODULE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/fortran_mods)
endif ()

set(CMAKE_MACOSX_RPATH 1)
# Windows setup
if (WIN32)
	# Make sure there is a .lib file to link to
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
	# Add appropriate debug flags
	set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>")
endif ()
if (SPGLIB_WITH_Fortran)
	enable_language(Fortran)
endif ()
if (SPGLIB_TEST_COVERAGE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
	set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} --coverage -ftest-forall-temp")
endif ()

# Report configuration
message(STATUS "Spglib:: Build type: ${CMAKE_BUILD_TYPE}")

#[==============================================================================================[
#                                       External packages                                       #
]==============================================================================================]

# Include third-party libraries
set(spglib_dependencies "")
if (SPGLIB_USE_OMP)
	find_package(OpenMP REQUIRED COMPONENTS C)
endif ()
if (SPGLIB_WITH_TESTS)
	set(BUILD_GMOCK OFF)
	set(INSTALL_GTEST OFF)
	set(gtest_disable_pthreads ON)
	FetchContent_Declare(GTest
			GIT_REPOSITORY https://github.com/google/googletest
			GIT_TAG v1.13.0
			FIND_PACKAGE_ARGS
	)
	list(APPEND spglib_dependencies GTest)
	find_package(Threads REQUIRED)
endif ()
if (SPGLIB_WITH_Python)
	find_package(Python3 COMPONENTS REQUIRED Interpreter Development.Module NumPy)
endif ()
FetchContent_MakeAvailable(${spglib_dependencies})

#[==============================================================================================[
#                                        Main definition                                        #
]==============================================================================================]

# Main project
add_library(Spglib_symspg)
set_target_properties(Spglib_symspg PROPERTIES
		VERSION ${PROJECT_VERSION}
		SOVERSION ${PROJECT_VERSION_MAJOR}
		EXPORT_NAME symspg
		OUTPUT_NAME symspg)
add_library(Spglib::symspg ALIAS Spglib_symspg)
# Main definitions inside src
add_subdirectory(src)

# Bindings
if (SPGLIB_WITH_Fortran)
	add_subdirectory(fortran)
endif ()
if (SPGLIB_WITH_Python)
	add_subdirectory(python)
endif ()

# Testing
if (SPGLIB_WITH_TESTS)
	enable_testing()
	add_subdirectory(test)
endif ()

#[==============================================================================================[
#                                       Install or Export                                       #
]==============================================================================================]
# Installation
# TODO: Temporarily disable installing cmake files when using scikit-build-core
#  Check how bundled cmake searches CMAKE_MODULE_DIR and add accordingly
#  (use SKBUILD_PLATLIB_DIR for install root)
if (NOT SKBUILD AND SPGLIB_INSTALL)
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
	export_components(LIB_TYPE ${Spglib_LIB_TYPE})
endif ()

# Make project available for FetchContent
if(NOT PROJECT_IS_TOP_LEVEL)
	# Set variables for FetchContent
	# All variables have to be consistent with SpglibConfig.cmake
	set(Spglib_Fortran ${SPGLIB_WITH_Fortran})
	set(Spglib_Python ${SPGLIB_WITH_Python})
	set(Spglib_OMP ${SPGLIB_USE_OMP})
	# Propagate variables
	if (CMAKE_VERSION VERSION_LESS 3.25)
		# TODO: Remove when cmake 3.25 is commonly distributed
		set(Spglib_VERSION ${Spglib_VERSION} PARENT_SCOPE)
		set(Spglib_VERSION_MAJOR ${Spglib_VERSION_MAJOR} PARENT_SCOPE)
		set(Spglib_VERSION_MINOR ${Spglib_VERSION_MINOR} PARENT_SCOPE)
		set(Spglib_VERSION_PATCH ${Spglib_VERSION_PATCH} PARENT_SCOPE)
		set(Spglib_VERSION_TWEAK ${Spglib_VERSION_TWEAK} PARENT_SCOPE)
		set(Spglib_Fortran ${Spglib_Fortran} PARENT_SCOPE)
		set(Spglib_Python ${Spglib_Python} PARENT_SCOPE)
		set(Spglib_OMP ${Spglib_OMP} PARENT_SCOPE)
		set(Spglib_LIB_TYPE ${Spglib_LIB_TYPE} PARENT_SCOPE)
	else ()
		return(PROPAGATE
				Spglib_VERSION
				Spglib_VERSION_MAJOR
				Spglib_VERSION_MINOR
				Spglib_VERSION_PATCH
				Spglib_VERSION_TWEAK
				Spglib_Fortran
				Spglib_Python
				Spglib_OMP
				Spglib_LIB_TYPE
				)
	endif ()
endif ()
