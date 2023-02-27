cmake_minimum_required(VERSION 3.6)
project(example_spglib LANGUAGES C)

# Alternatively use pkg-config
find_package(Spglib)
if(NOT Spglib_FOUND)
	find_package(PkgConfig)
	pkg_check_modules(symspg REQUIRED IMPORTED_TARGET
			spglib)
endif()

add_executable(example_c example.c)
if(Spglib_FOUND)
	# Linking using native cmake
	target_link_libraries(example_c PUBLIC
			Spglib::symspg)
else()
	# Linking using pkg-config
	target_link_libraries(example_c PUBLIC
			PkgConfig::symspg)
endif()
