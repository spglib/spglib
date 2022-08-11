cmake_minimum_required(VERSION 3.11)

project(test_spglib CXX C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_MACOSX_RPATH 1)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  add_definitions(-DSPGDEBUG -DSPGWARNING)
endif()

# Sanitizers
if (USE_SANITIZER STREQUAL "Address")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
elseif (USE_SANITIZER STREQUAL "Thread")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=thread")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
elseif (USE_SANITIZER STREQUAL "Undefined")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
elseif (USE_SANITIZER STREQUAL "Leak")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=leak")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")
elseif (USE_SANITIZER STREQUAL "Memory")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=memory")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=memory")
endif()

# OpenMP
if (USE_OMP)
  find_package(OpenMP REQUIRED)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
endif()

# pthread (required for GoogleTest)
# https://stackoverflow.com/questions/1620918/cmake-and-libpthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Include src/spglib.h and test/utils.h
include_directories("${PROJECT_SOURCE_DIR}/../src" "${PROJECT_SOURCE_DIR}")

# Static library
file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/../src/*.c)
add_library(symspg STATIC ${SOURCES})

# Fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

enable_testing()
foreach(TESTCASE IN ITEMS
        test_error
        test_symmetry_search
        test_spacegroup_type_search
        test_find_primitive_cell
        test_refine_cell
        test_standardization
        test_dataset_access
        test_magnetic_symmetry
        test_kpoints
        )
  add_executable(${TESTCASE} ${TESTCASE}.cpp utils.c)
  target_link_libraries(${TESTCASE} PUBLIC symspg gtest_main PRIVATE Threads::Threads)
  gtest_discover_tests(${TESTCASE})
endforeach()
