module test_spg_get_spacegroup_type
    use spglib_f08, only: spg_get_spacegroup_type, SpglibSpacegroupType
    use test_utils
    use C_interface_module
    use, intrinsic :: iso_fortran_env
    implicit none
contains
    function test_fortran_spg_get_spacegroup_type(argc, argv) bind(C) result(ret)
        use, intrinsic :: iso_c_binding
        integer(c_int), value, intent(in) :: argc
        type(c_ptr), intent(in) :: argv(argc)
        integer(c_int) :: ret

        integer :: len
        character(len=:), allocatable :: str

        if (argc < 2) then
            write (output_unit, *) "test_spacegroup_type_rutile"
            call test_spacegroup_type_rutile()
        else
            len = c_strlen(argv(2))
            allocate (character(len=len) :: str)
            call C_F_string(argv(2), str)
            select case (str)
            case ("test_spacegroup_type_rutile")
                call test_spacegroup_type_rutile()
            case default
                write (error_unit, *) "No known sub-test passed"
                ret = 1
                return
            end select
            deallocate (str)
        end if

        ret = 0
    end function test_fortran_spg_get_spacegroup_type

    subroutine test_spacegroup_type_rutile() bind(C)
        integer(c_int) :: hall_number = 419
        integer(c_int) :: arithmetic_crystal_class_number = 36

        type(SpglibSpacegroupType) :: spg_type
        character(len=11) :: international_short = "P4_2/mnm"
        character(len=20) :: international_full = "P 4_2/m 2_1/n 2/m"
        character(len=32) :: international = "P 4_2/m n m"
        character(len=7) ::  schoenflies = "D4h^14"
        character(len=17) :: hall_symbol = "-P 4n 2n"
        character(len=6) :: choice = ""
        character(len=6) :: pointgroup_international = "4/mmm"
        character(len=6) :: pointgroup_schoenflies = "D4h"
        character(len=7) :: arithmetic_crystal_class_symbol = "4/mmmP"

        hall_number = 419
        spg_type = spg_get_spacegroup_type(hall_number)
        call assert(spg_type%number, 136)
        call assert(spg_type%international_short, international_short)
        call assert(spg_type%international_full, international_full)
        call assert(spg_type%international, international)
        call assert(spg_type%schoenflies, schoenflies)
        call assert(spg_type%hall_number, hall_number)
        call assert(spg_type%hall_symbol, hall_symbol)
        call assert(spg_type%choice, choice)
        call assert(spg_type%pointgroup_international, pointgroup_international)
        call assert(spg_type%pointgroup_schoenflies, pointgroup_schoenflies)
        call assert(spg_type%arithmetic_crystal_class_number, arithmetic_crystal_class_number)
        call assert(spg_type%arithmetic_crystal_class_symbol, arithmetic_crystal_class_symbol)
    end subroutine test_spacegroup_type_rutile

end module test_spg_get_spacegroup_type
