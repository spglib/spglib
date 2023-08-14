cmake_minimum_required(VERSION 3.6)
project(example_spglib LANGUAGES C)

# Alternatively use pkg-config
find_package(Spglib)
if(NOT Spglib_FOUND)
	find_package(PkgConfig)
	pkg_check_modules(symspg REQUIRED IMPORTED_TARGET
			spglib)
endif()

add_executable(example example.c)
add_executable(example_full example_full.c)
if(Spglib_FOUND)
	# Linking using native cmake
	target_link_libraries(example PRIVATE Spglib::symspg)
	target_link_libraries(example_full PRIVATE Spglib::symspg)
else()
	# Linking using pkg-config
	target_link_libraries(example PRIVATE PkgConfig::symspg)
	target_link_libraries(example_full PRIVATE PkgConfig::symspg)
endif()
