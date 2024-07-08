set_property(DIRECTORY APPEND
        PROPERTY LABELS example
)

set(test_list
        c_api
)
if (SPGLIB_WITH_Fortran)
    list(APPEND test_list
            fortran_api
    )
endif ()

foreach (test IN LISTS test_list)
    Spglib_add_test(${test}
            TEST_NAME example-${test}
            TEST_COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${test}/example && ${CMAKE_CURRENT_BINARY_DIR}/${test}/example_full
    )
endforeach ()

if (SPGLIB_WITH_Python)
    add_test(NAME example-python
            COMMAND ${Python_EXECUTABLE} -Werror ${CMAKE_CURRENT_SOURCE_DIR}/python_api/example.py
    )
    add_test(NAME example-python-full
            COMMAND ${Python_EXECUTABLE} -Werror ${CMAKE_CURRENT_SOURCE_DIR}/python_api/example_full.py
    )
    set_property(TEST example-python example-python-full
            APPEND PROPERTY
            ENVIRONMENT PYTHONPATH=${Spglib_Python_BINARY_DIR}
    )
endif ()
