cmake_minimum_required(VERSION 3.6)
project(example_spglib LANGUAGES Fortran)

# Import spglib using native cmake
find_package(Spglib COMPONENTS fortran)
# Alternatively use pkg-config
if(NOT Spglib_FOUND)
	find_package(PkgConfig)
	pkg_check_modules(symspg REQUIRED IMPORTED_TARGET
			spglib)
	pkg_check_modules(spglib_f08 REQUIRED IMPORTED_TARGET
			spglib_f08)
endif()


add_executable(example_f example_f08.F90)
if(Spglib_FOUND)
	# Linking using native cmake
	target_link_libraries(example_c PUBLIC
			Spglib::symspg)
	target_link_libraries(example_f PUBLIC
			Spglib::fortran)
else()
	# Linking using pkg-config
	target_link_libraries(example_c PUBLIC
			PkgConfig::symspg)
	target_link_libraries(example_f PUBLIC
			PkgConfig::spglib_f08)
endif()
