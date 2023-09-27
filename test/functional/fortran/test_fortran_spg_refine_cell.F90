module test_spg_refine_cell
    use spglib_f08, only: spg_refine_cell
    use test_utils
    use C_interface_module
    use, intrinsic :: iso_fortran_env
    implicit none
contains
    function test_fortran_spg_refine_cell(argc, argv) bind(C) result(ret)
        use, intrinsic :: iso_c_binding
        integer(c_int), value, intent(in) :: argc
        type(c_ptr), intent(in) :: argv(argc)
        integer(c_int) :: ret

        write (output_unit, *) "test_FCC"
        call test_FCC()

        ret = 0
    end function

    !> @brief Find standardaized cell of FCC from its primitive cell
    !> The output structure of spg_refine_cell may contain more number of atoms.
    !> Therefore, larger space than that required for input structure has to
    !> be allocated.
    subroutine test_FCC() bind(C)
        real(c_double) :: lattice(3, 3)
        real(c_double) :: position(3, 4)
        integer(c_int) :: types(4)
        integer :: num_atom_bravais
        integer(c_int) :: num_atom
        real(c_double) :: symprec

        num_atom = 1
        lattice(:, :) = reshape([0, 2, 2, 2, 0, 2, 2, 2, 0], [3, 3])
        ! position and types : 4 times of number of input atoms at maximum
        position(:, 1) = 0.0
        types(1) = 0
        symprec = 1e-5

        num_atom_bravais = spg_refine_cell(lattice, position, types, num_atom, symprec)

        call assert(num_atom_bravais, 4)
    end subroutine test_FCC
end module test_spg_refine_cell
