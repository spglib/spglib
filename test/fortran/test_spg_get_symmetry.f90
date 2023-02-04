function test_spg_get_symmetry() bind(C) result(ret)
    use, intrinsic :: iso_c_binding
    use spglib_f08, only: spg_get_symmetry
    use test_utils
    implicit none

    real(c_double) :: lattice(3, 3)
    real(c_double) :: position(3, 12)
    integer(c_int) :: types(12)

    integer :: i, j, n_sym
    real :: origin_shift(3)
    integer(c_int), allocatable :: rotation(:, :, :)
    real(c_double), allocatable ::translation(:, :)
    real(c_double) :: symprec
    integer(c_int) :: num_atom, max_size, ret

    num_atom = 12
    max_size = num_atom * 48

    allocate(rotation(3, 3, max_size))
    allocate(translation(3, max_size))

    lattice(:, :) = reshape([4, 0, 0, 0, 4, 0, 0, 0, 3], [3, 3])
    position(:, :) = reshape( &
                     [ &
                     0.0, 0.0, 0.0, 0.5, 0.5, 0.25, 0.3, 0.3, 0.0, 0.7, 0.7, 0.0, &
                     0.2, 0.8, 0.25, 0.8, 0.2, 0.25, 0.0, 0.0, 0.5, 0.5, 0.5, 0.75, &
                     0.3, 0.3, 0.5, 0.7, 0.7, 0.5, 0.2, 0.8, 0.75, 0.8, 0.2, 0.75 &
                     ], [3, 12])
    types(:) = [1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2]
    origin_shift(:) = [0.1, 0.1, 0.0]
    symprec = 1e-5

    do i = 1, num_atom
        position(:, i) = position(:, i) + origin_shift(:)
    end do

    n_sym = spg_get_symmetry(rotation, translation, max_size, lattice, position, &
                            types, num_atom, symprec)

    deallocate(translation)
    deallocate(rotation)

    call assert_int(n_sym, 32)

    ret = 0

end function test_spg_get_symmetry
