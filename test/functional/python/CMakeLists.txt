set_property(DIRECTORY APPEND
        PROPERTY LABELS pytest
)
add_test(NAME pytests
        COMMAND ${Python_EXECUTABLE} -m pytest
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/..
)
set_property(TEST pytests APPEND PROPERTY
        ENVIRONMENT PYTHONPATH=${Spglib_Python_BINARY_DIR}
)
