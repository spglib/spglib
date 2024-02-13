program spglib_example
    use, intrinsic :: iso_c_binding
    use spglib_f08, only: spg_get_version, version
    implicit none

    print *, "Spglib version: ", spg_get_version()
    print *, "Spglib_Fortran version: ", version
end program spglib_example
