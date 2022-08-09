Name: ${PROJECT_NAME}
Description: The spglib f08 library
depends: spglib
Version: ${PROJECT_VERSION}
Libs: -L${CMAKE_INSTALL_FULL_LIBDIR} -lspglib_f08
Cflags: -I${CMAKE_INSTALL_PREFIX}/include -I${CMAKE_INSTALL_FULL_LIBDIR}
