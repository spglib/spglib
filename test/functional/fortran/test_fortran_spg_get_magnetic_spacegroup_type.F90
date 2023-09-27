module test_spg_get_magnetic_spacegroup_type
    use spglib_f08, only: spg_get_magnetic_spacegroup_type, SpglibMagneticSpacegroupType
    use test_utils
    use C_interface_module
    use, intrinsic :: iso_fortran_env
    implicit none
contains
    function test_fortran_spg_get_magnetic_spacegroup_type(argc, argv) bind(C) result(ret)
        use, intrinsic :: iso_c_binding
        integer(c_int), value, intent(in) :: argc
        type(c_ptr), intent(in) :: argv(argc)
        integer(c_int) :: ret

        integer :: len
        character(len=:), allocatable :: str

        if (argc < 2) then
            write (output_unit, *) "test_magnetic_spacegroup_type_Cr"
            call test_magnetic_spacegroup_type_Cr()
        else
            len = c_strlen(argv(2))
            allocate (character(len=len) :: str)
            call C_F_string(argv(2), str)
            select case (str)
            case ("test_magnetic_spacegroup_type_Cr")
                call test_magnetic_spacegroup_type_Cr()
            case default
                write (error_unit, *) "No known sub-test passed"
                ret = 1
                return
            end select
            deallocate (str)
        end if

        ret = 0
    end function test_fortran_spg_get_magnetic_spacegroup_type

    subroutine test_magnetic_spacegroup_type_Cr() bind(C)
        type(SpglibMagneticSpacegroupType) :: magspg_type

        magspg_type = spg_get_magnetic_spacegroup_type(1599)
        call assert(magspg_type%uni_number, 1599)
        call assert(magspg_type%litvin_number, 1643)
        call assert(magspg_type%bns_number, "221.97")
        call assert(magspg_type%og_number, "229.6.1643")
        call assert(magspg_type%number, 221)
        call assert(magspg_type%type, 4)
    end subroutine test_magnetic_spacegroup_type_Cr

end module test_spg_get_magnetic_spacegroup_type
