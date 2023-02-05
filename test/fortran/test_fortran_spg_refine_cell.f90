program test_fortran_spg_refine_cell
    use, intrinsic :: iso_c_binding
    use spglib_f08, only: spg_refine_cell
    use test_utils
    implicit none

    write (*, '("test_BCC")')
    call test_BCC()
contains
    subroutine test_BCC() bind(C)
        real(c_double) :: lattice(3, 3)
        real(c_double) :: position(3, 1)
        integer(c_int) :: types(4)
        integer :: num_atom_bravais
        integer(c_int) :: num_atom
        real(c_double) :: symprec

        lattice(:, :) = reshape([0, 2, 2, 2, 0, 2, 2, 2, 0], [3, 3])
        position(:, :) = reshape([0, 0, 0], [3, 1])
        types(1) = 0  ! overwritten (maximum 4 times of number of input atoms)
        num_atom = 1
        symprec = 1e-5

        num_atom_bravais = spg_refine_cell(lattice, position, types, num_atom, symprec)

        call assert_int(num_atom_bravais, 4)
    end subroutine test_BCC
end program test_fortran_spg_refine_cell
