target_sources(Spglib_symspg PUBLIC
    FILE_SET HEADERS
    FILES
        spglib.h
)

target_include_directories(Spglib_symspg PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
