cmake_minimum_required(VERSION 3.25...3.29)
project(example_spglib LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(Spglib REQUIRED)

add_executable(example example.c)
add_executable(example_full example_full.c)

target_link_libraries(example PRIVATE Spglib::symspg)
target_link_libraries(example_full PRIVATE Spglib::symspg)

# Windows is weird like that :/
# https://stackoverflow.com/a/73550650
if (CMAKE_IMPORT_LIBRARY_SUFFIX)
    add_custom_command(TARGET example POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:example> $<TARGET_FILE_DIR:example>
            COMMAND_EXPAND_LISTS
    )
    add_custom_command(TARGET example_full POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:example_full> $<TARGET_FILE_DIR:example_full>
            COMMAND_EXPAND_LISTS
    )
endif ()
