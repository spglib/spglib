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
            write (output_unit, *) "test_dataset_rutile112"
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
            end select
            deallocate (str)
        end if

        ret = 0
    end function test_fortran_spg_get_magnetic_spacegroup_type

    subroutine test_magnetic_spacegroup_type_Cr() bind(C)
        integer(c_int) :: uni_number = 1599
        integer(c_int) :: litvin_number = 1643
        integer(c_int) :: number = 221
        integer(c_int) :: construction_type = 4
        character(len=8) :: bns_number = "221.97"
        character(len=12) :: og_number = "229.6.1643"
        type(SpglibMagneticSpacegroupType) :: magspg_type

        magspg_type = spg_get_magnetic_spacegroup_type(uni_number)
        call assert(magspg_type%uni_number, uni_number)
        call assert(magspg_type%litvin_number, litvin_number)
        call assert(magspg_type%bns_number, bns_number)
        call assert(magspg_type%og_number, og_number)
        call assert(magspg_type%number, number)
        call assert(magspg_type%type, construction_type)
    end subroutine test_magnetic_spacegroup_type_Cr

end module test_spg_get_magnetic_spacegroup_type
