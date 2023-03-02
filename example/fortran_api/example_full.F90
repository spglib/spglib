program spglib_example
    use, intrinsic :: iso_c_binding
    implicit none

    call run_examples()

contains

    subroutine run_examples()
        print '("Rutile:")'
        call example_spg_get_dataset_rutile()
        print '("")'
        print '("FCC:")'
        call example_spg_get_dataset_FCC()
        print '("")'
        print '("Wurtzite:")'
        call example_spg_get_dataset_wurtzite()
    end subroutine run_examples

    subroutine example_spg_get_dataset_rutile()
        integer(c_int) :: num_atom
        real(c_double) :: symprec = 1e-6
        real(c_double) :: lattice(3, 3)
        real(c_double) :: positions(3, 6)
        integer(c_int) :: atom_types(6)

        integer(c_int) :: mesh(3) = [4, 4, 6]
        integer(c_int) :: shifted(3)
        integer(c_int) :: grid_point(3, 96)
        integer(c_int) :: map(96)
        integer(c_int) :: is_time_reversal = 1

        real :: a

        num_atom = 6
        lattice(1, :) = [4.0, 0.0, 0.0]
        lattice(2, :) = [0.0, 4.0, 0.0]
        lattice(3, :) = [0.0, 0.0, 2.6]
        positions(:, 1) = [0.0, 0.0, 0.0]
        positions(:, 2) = [0.5, 0.5, 0.5]
        positions(:, 3) = [0.3, 0.3, 0.0]
        positions(:, 4) = [0.7, 0.7, 0.0]
        positions(:, 5) = [0.2, 0.8, 0.5]
        positions(:, 6) = [0.8, 0.2, 0.5]
        atom_types(:) = [1, 1, 2, 2, 2, 2]
        shifted(:) = [1, 1, 1]

        call show_syminfo(num_atom, lattice, symprec, atom_types, positions)
        call show_grid_info(num_atom, lattice, symprec, atom_types, positions, &
                            mesh, shifted, is_time_reversal)

    end subroutine example_spg_get_dataset_rutile

    subroutine example_spg_get_dataset_FCC()
        integer(c_int) :: num_atom
        real(c_double) :: symprec = 1e-6
        real(c_double) :: lattice(3, 3)
        real(c_double) :: positions(3, 1)
        integer(c_int) :: atom_types(1)

        integer(c_int) :: mesh(3) = [4, 4, 4]
        integer(c_int) :: shifted(3)
        integer(c_int) :: grid_point(3, 64)
        integer(c_int) :: map(64)
        integer(c_int) :: is_time_reversal = 1

        num_atom = 1
        lattice(1, :) = [0.0, 2.0, 2.0]
        lattice(2, :) = [2.0, 0.0, 2.0]
        lattice(3, :) = [2.0, 2.0, 0.0]
        positions(:, 1) = [0.0, 0.0, 0.0]
        atom_types(1) = 1
        shifted = [0, 0, 0]

        call show_syminfo(num_atom, lattice, symprec, atom_types, positions)
        call show_grid_info(num_atom, lattice, symprec, atom_types, positions, &
                            mesh, shifted, is_time_reversal)

    end subroutine example_spg_get_dataset_FCC

    subroutine example_spg_get_dataset_wurtzite
        integer(c_int) :: num_atom
        real(c_double) :: symprec = 1e-6
        real(c_double) :: lattice(3, 3)
        real(c_double) :: positions(3, 4)
        integer(c_int) :: atom_types(4)

        integer(c_int) :: mesh(3) = [6, 6, 4]
        integer(c_int) :: shifted(3)
        integer(c_int) :: grid_point(3, 144)
        integer(c_int) :: map(144)
        integer(c_int) :: is_time_reversal = 1

        num_atom = 4
        lattice(:, 1) = [3.111, -1.5555, 0.0]
        lattice(:, 2) = [0.0, 2.6942050311733885, 0.0]
        lattice(:, 3) = [0.0, 0.0, 4.988]
        positions(:, 1) = [1.0/3, 2.0/3, 0.0]
        positions(:, 2) = [2.0/3, 1.0/3, 0.5]
        positions(:, 3) = [1.0/3, 2.0/3, 0.6181]
        positions(:, 4) = [2.0/3, 1.0/3, 0.1181]
        atom_types(:) = [1, 1, 2, 2]
        shifted(:) = [0, 0, 1]

        call show_syminfo(num_atom, lattice, symprec, atom_types, positions)
        call show_grid_info(num_atom, lattice, symprec, atom_types, positions, &
                            mesh, shifted, is_time_reversal)

    end subroutine example_spg_get_dataset_wurtzite

    subroutine show_syminfo(num_atom, lattice, symprec, atom_types, positions)
        use spglib_f08, only: &
            SpglibDataset, &
            SpglibSpacegroupType, &
            spg_get_dataset, &
            spg_get_spacegroup_type, &
            spg_get_ir_reciprocal_mesh

        ! Arguments ------------------------------------
        ! scalars
        integer(c_int), intent(in) :: num_atom
        real(c_double), intent(in) :: symprec
        ! arrays
        integer(c_int), intent(in) :: atom_types(num_atom)
        real(c_double), intent(in) :: lattice(3, 3)
        real(c_double), intent(in) :: positions(3, num_atom)
        ! Local variables-------------------------------
        ! scalars
        integer :: i, j, space_group, num_sym, indent
        character(len=21) :: international
        character(len=7) :: schoenflies
        character(len=30) :: space
        character(len=27) :: wyckoffs

        type(SpglibDataset) :: dset
        type(SpglibSpacegroupType) :: spgtype

        !**************************************************************************

        !   The allocatable components of dset get deallocated on scope exit
        dset = spg_get_dataset(lattice, positions, atom_types, num_atom, symprec)

        num_sym = dset%n_operations

        wyckoffs = "abcdefghijklmnopqrstuvwxyz"
        space = "                              "
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
        print "(a,'wyckoffs:')", space(1:indent*2)
        do i = 1, dset%n_atoms
            print('(a, "- ", i0, ": ", a)'), space(1:indent*2), i, wyckoffs(dset%wyckoffs(i) + 1:dset%wyckoffs(i) + 1)
        end do
        print "(a,'equivalent_atoms:')", space(1:indent*2)
        do i = 1, dset%n_atoms
            print('(a, "- ", i0, ": ", i0)'), space(1:indent*2), i, dset%equivalent_atoms(i) + 1
        end do
    end subroutine show_syminfo

    subroutine show_grid_info(num_atom, lattice, symprec, atom_types, positions, &
                              mesh, shifted, is_time_reversal)
        use spglib_f08, only: spg_get_spacegroup_type, spg_get_ir_reciprocal_mesh

        ! Arguments ------------------------------------
        ! scalars
        integer(c_int), intent(in) :: num_atom, is_time_reversal
        real(c_double), intent(in) :: symprec
        ! arrays
        integer(c_int), intent(in) :: atom_types(num_atom)
        integer(c_int), intent(in) :: mesh(3), shifted(3)
        real(c_double), intent(in) :: lattice(3, 3)
        real(c_double), intent(in) :: positions(3, num_atom)
        ! Local variables-------------------------------
        ! scalars
        integer :: i, counter, weight, indent
        integer :: num_ir_grid
        character(len=30) :: space
        ! arrays
        integer(c_int) :: grid_point(3, mesh(1)*mesh(2)*mesh(3))
        integer(c_int) :: map(mesh(1)*mesh(2)*mesh(3))

        !**************************************************************************

        num_ir_grid = spg_get_ir_reciprocal_mesh(grid_point, map, &
            & mesh, shifted, is_time_reversal, lattice, positions, &
            & atom_types, num_atom, symprec)

        space = "                              "
        indent = 1

        print('(a, "reciprocal-mesh: [", i3, ", ", i3, ", ", i3, "]")'), space(1:indent*2), mesh(:)
        print('(a, "- shifted: [", i3, ", ", i3, ", ", i3, "]")'), space(1:indent*2), shifted(:)
        print('(a, "- is_time_reversal: ", i3)'), space(1:indent*2), is_time_reversal

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

    end subroutine show_grid_info

end program spglib_example
