! How to make
! % gfortran example_f08.f90 ../lib/libspglib_f08.a ../lib/libsymspg.a -I../include

subroutine write_syminfo(max_num_sym, num_atom, &
     & lattice, symprec, atom_types, positions, &
     & mesh, shifted, is_time_reversal)
    use, intrinsic :: iso_c_binding
    use spglib_f08, only: &
        SpglibDataset, &
        SpglibSpacegroupType, &
        spg_get_dataset, &
        spg_get_spacegroup_type, &
        spg_get_ir_reciprocal_mesh

    implicit none

    ! Arguments ------------------------------------
    ! scalars
    integer(c_int), intent(in) :: num_atom, max_num_sym, is_time_reversal
    real(c_double), intent(in) :: symprec
    ! arrays
    integer(c_int), intent(in) :: atom_types(num_atom)
    integer(c_int), intent(in), dimension(3) :: mesh(3), shifted(3)
    real(c_double), intent(in) :: lattice(3, 3)
    real(c_double), intent(in) :: positions(3, num_atom)
    ! Local variables-------------------------------
    ! scalars
    integer :: i, j, counter, weight, space_group, num_sym, indent
    integer :: num_ir_grid
    character(len=21) :: international
    character(len=7) :: schoenflies
    character(len=30) :: space
    character(len=128) :: fmt
    ! arrays
    integer(c_int) :: rotations(3, 3, max_num_sym)
    integer(c_int) :: grid_point(3, mesh(1)*mesh(2)*mesh(3))
    integer(c_int) :: map(mesh(1)*mesh(2)*mesh(3))
    real(c_double)  :: translations(3, max_num_sym)

    type(SpglibDataset) :: dset
    type(SpglibSpacegroupType) :: spgtype

    !**************************************************************************

    space = "                              "

!   The allocatable components of dset get deallocated on scope exit
    dset = spg_get_dataset(lattice, positions, atom_types, num_atom, symprec)

    num_sym = dset%n_operations

    indent = 1
    if (dset%spacegroup_number /= 0) then
        spgtype = spg_get_spacegroup_type(dset%hall_number)

        print('(a, "space_group: ", i3)'), space(1:indent*2), dset%spacegroup_number
        print('(a, "international: ", a, a)'), space(1:indent*2), trim(dset%international_symbol)
        print('(a, "schoenflies: ", a)'), space(1:indent*2), trim(spgtype%schoenflies)
    else
        print '("Space group could not be found,")'
    end if

    print('(a, "atom-type:")'), space(1:indent*2)
    do i = 1, num_atom
        print('(a, "- { type: ", i3, "}")'), space(1:indent*2), atom_types(i)
    end do
    print('(a, "real-basis:")'), space(1:indent*2)
    do i = 1, 3
        print('(a, "- [", f19.14, ", ", f19.14, ", ", f19.14, "]")'), space(1:indent*2), lattice(i, :)
    end do
    print('(a, "position:")'), space(1:indent*2)
    do i = 1, num_atom
        print('(a, "- [", f17.14, ", ", f17.14, ", ", f17.14, "]")'), space(1:indent*2), positions(:, i)
    end do
    print('(a, "operation:")'), space(1:indent*2)
    do i = 1, num_sym
        print('(a, "- rotation: #", i4)'), space(1:indent*2), i
        do j = 1, 3
            print('(a, "  - [", i3,",", i3,",", i3,"]")'), space(1:indent*2), dset%rotations(:, j, i)
        end do
        print('(a, "  translation: [ ", f10.7,", ", f10.7,", ", f10.7,"]")'), space(1:indent*2), dset%translations(:, i)
    end do

    write (fmt, '(i0,a)') num_atom, "(x,i0,':',i0))"
    print "(a,'wyckoffs:         ', "//fmt, space(1:indent*2), (i, dset%wyckoffs(i), i=1, dset%n_atoms)
    print "(a,'equivalent_atoms: ', "//fmt, space(1:indent*2), (i, dset%equivalent_atoms(i) + 1, i=1, dset%n_atoms)

    print('(a, "reciprocal-mesh: [", i3, ", ", i3, ", ", i3, "]")'), space(1:indent*2), mesh(:)
    print('(a, "- shifted: [", i3, ", ", i3, ", ", i3, "]")'), space(1:indent*2), shifted(:)
    print('(a, "- is_time_reversal: ", i3)'), space(1:indent*2), is_time_reversal

    num_ir_grid = spg_get_ir_reciprocal_mesh(grid_point, map, &
        & mesh, shifted, is_time_reversal, lattice, positions, &
        & atom_types, num_atom, symprec)

    print('(a, "- num-ir-grid-point:", i4)'), space(1:indent*2), num_ir_grid
    print('(a, "- ir-grid-point:")'), space(1:indent*2)
    counter = 0

    do i = 1, product(mesh)
        if (i - 1 == map(i)) then
            ! Ad-hoc and intuitive implementation of weight. Not optimal for very large size
            weight = count(map == i - 1)
            counter = counter + 1

            print '(i5, 3(x,f9.6), 2x,i4,2x, f9.6, 3(x,i4))', counter, &
             & real(shifted + 2*grid_point(:, i))/real(2*mesh), map(i) + 1, &
             & real(weight)/real(product(mesh)), grid_point(:, i)
        end if
    end do

end subroutine write_syminfo

program spglib_example
    use, intrinsic :: iso_c_binding
    implicit none

    ! max_sym is the expected maximum number of symmetry operations.
    ! This can be very big in a supercell.
    integer(c_int) :: max_num_sym = 192

    integer(c_int) :: num_atom
    real(c_double) :: symprec = 1e-6
    integer(c_int), dimension(3, 3, 192) :: rotations
    real(c_double), dimension(3, 192) :: translations
    real(c_double), dimension(3, 3) :: lattice
    real(c_double), dimension(3, 1000) :: positions
    integer(c_int), dimension(1000) :: atom_types

    integer(c_int) :: mesh(3) = [20, 20, 20]
    integer(c_int) :: shifted(3)
    integer(c_int) :: grid_point(3, 8000)
    integer(c_int) :: map(8000)
    integer(c_int) :: is_time_reversal = 1

    real :: a

    print '("Rutile:")'
    num_atom = 6
    lattice(1, :) = (/4.0, 0.0, 0.0/)
    lattice(2, :) = (/0.0, 4.0, 0.0/)
    lattice(3, :) = (/0.0, 0.0, 2.6/)
    positions(1:3, 1) = (/0.0, 0.0, 0.0/)
    positions(1:3, 2) = (/0.5, 0.5, 0.5/)
    positions(1:3, 3) = (/0.3, 0.3, 0.0/)
    positions(1:3, 4) = (/0.7, 0.7, 0.0/)
    positions(1:3, 5) = (/0.2, 0.8, 0.5/)
    positions(1:3, 6) = (/0.8, 0.2, 0.5/)
    atom_types(1:6) = (/1, 1, 2, 2, 2, 2/)
    shifted = [0, 0, 0]

    call write_syminfo(max_num_sym, num_atom, &
                       lattice, symprec, atom_types, positions, &
                       mesh, shifted, is_time_reversal)

    print '("")'
    print '("FCC:")'
    num_atom = 1
    lattice(1, :) = (/0.0, 2.0, 2.0/)
    lattice(2, :) = (/2.0, 0.0, 2.0/)
    lattice(3, :) = (/2.0, 2.0, 0.0/)
    positions(1:3, 1) = (/0.0, 0.0, 0.0/)
    atom_types(1) = 1
    shifted = [0, 0, 0]

    call write_syminfo(max_num_sym, num_atom, &
                       lattice, symprec, atom_types, positions, &
                       mesh, shifted, is_time_reversal)

    print '(/,"D0_{24}:")'
    a = 2.89

    lattice(1, :) = [sqrt(3.0), -1.0, 0.0]
    lattice(2, :) = [sqrt(3.0), 1.0, 0.0]
    lattice(3, :) = [0.0, 0.0, sqrt(32/3.0)]

    num_atom = 16
    positions(:, 1) = [0.0, 0.0, 0.0]
    positions(:, 2) = [3.0, 0.0, 0.0]
    positions(:, 3) = [0.0, 3.0, 0.0]
    positions(:, 4) = [3.0, 3.0, 0.0]
    positions(:, 5) = [2.0, 2.0, 1.0]
    positions(:, 6) = [5.0, 2.0, 1.0]
    positions(:, 7) = [2.0, 5.0, 1.0]
    positions(:, 8) = [5.0, 5.0, 1.0]
    positions(:, 9) = [0.0, 0.0, 2.0]
    positions(:, 10) = [3.0, 0.0, 2.0]
    positions(:, 11) = [0.0, 3.0, 2.0]
    positions(:, 12) = [3.0, 3.0, 2.0]
    positions(:, 13) = [1.0, 1.0, 3.0]
    positions(:, 14) = [4.0, 1.0, 3.0]
    positions(:, 15) = [1.0, 4.0, 3.0]
    positions(:, 16) = [4.0, 4.0, 3.0]

    positions(1:2, :num_atom) = positions(1:2, :num_atom)/6
    positions(3, :num_atom) = positions(3, :num_atom)/4

    atom_types(:num_atom) = [1, 2, 2, 2, 1, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1]
    mesh = [12, 12, 6]
    shifted = [1, 1, 1]

    call write_syminfo(max_num_sym, num_atom, lattice, symprec, atom_types, &
                       positions, mesh, shifted, is_time_reversal)

end program spglib_example
