! Stringerize compiler definitions
! https://stackoverflow.com/a/46342008
#ifdef __GFORTRAN__
# define STRINGIFY_START(X) "&
# define STRINGIFY_END(X) &X"
#else /* default stringification */
# define STRINGIFY_(X) #X
# define STRINGIFY_START(X) &
# define STRINGIFY_END(X) STRINGIFY_(X)
#endif

module test_version
    use spglib_f08, only: version, version_full, commit, &
                          spg_get_version, spg_get_version_full, spg_get_commit
    use test_utils
    use C_interface_module
    use, intrinsic :: iso_fortran_env
    implicit none
contains
    function test_fortran_version(argc, argv) bind(C) result(ret)
        use, intrinsic :: iso_c_binding
        integer(c_int), value, intent(in) :: argc
        type(c_ptr), intent(in) :: argv(argc)
        integer(c_int) :: ret

        integer :: len
        character(len=:), allocatable :: str

        if (argc < 2) then
            write (output_unit, *) "version_string"
            call test_version_string()
        else
            len = c_strlen(argv(2))
            allocate (character(len=len) :: str)
            call C_F_string(argv(2), str)
            select case (str)
            case ("version_string")
                call test_version_string()
            case default
                write (error_unit, *) "No known sub-test passed"
                ret = 1
                return
            end select
            deallocate (str)
        end if

        ret = 0
    end function test_fortran_version

    subroutine test_version_string()
        character(len=:), allocatable :: spg_version, spg_version_full, spg_commit, value

        spg_version = STRINGIFY_START(Spglib_VERSION)
        STRINGIFY_END(Spglib_VERSION)
        spg_version_full = STRINGIFY_START(Spglib_VERSION_FULL)
        STRINGIFY_END(Spglib_VERSION_FULL)
        spg_commit = STRINGIFY_START(Spglib_COMMIT)
        STRINGIFY_END(Spglib_COMMIT)

        call assert(version, spg_version)
        call assert(version_full, spg_version_full)
        call assert(commit, spg_commit)
        value = spg_get_version()
        call assert(value, spg_version)
        value = spg_get_version_full()
        call assert(value, spg_version_full)
        value = spg_get_commit()
        call assert(value, spg_commit)
    end subroutine test_version_string
end module test_version
