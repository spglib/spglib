! How to make
! % gfortran example_f08.f90 ../lib/libspglib_f08.a ../lib/libsymspg.a -I../include

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
        print '("D0_{24}:")'
        print '("")'
        call example_spg_get_dataset_D0_24()
    end subroutine run_examples

    subroutine example_spg_get_dataset_rutile()
        integer(c_int) :: num_atom
        real(c_double) :: symprec = 1e-6
        real(c_double) :: lattice(3, 3)
        real(c_double) :: positions(3, 6)
        integer(c_int) :: atom_types(6)

        integer(c_int) :: mesh(3) = [20, 20, 20]
        integer(c_int) :: shifted(3)
        integer(c_int) :: grid_point(3, 8000)
        integer(c_int) :: map(8000)
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
        shifted(:) = [0, 0, 0]

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

        integer(c_int) :: mesh(3) = [20, 20, 20]
        integer(c_int) :: shifted(3)
        integer(c_int) :: grid_point(3, 8000)
        integer(c_int) :: map(8000)
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

    subroutine example_spg_get_dataset_D0_24
        integer(c_int) :: num_atom
        real(c_double) :: symprec = 1e-6
        real(c_double) :: lattice(3, 3)
        real(c_double) :: positions(3, 16)
        integer(c_int) :: atom_types(16)

        integer(c_int) :: mesh(3) = [20, 20, 20]
        integer(c_int) :: shifted(3)
        integer(c_int) :: grid_point(3, 8000)
        integer(c_int) :: map(8000)
        integer(c_int) :: is_time_reversal = 1

        real :: a

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

        positions(1:2, :) = positions(1:2, :)/6
        positions(3, :) = positions(3, :)/4

        atom_types(:) = [1, 2, 2, 2, 1, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1]
        mesh = [12, 12, 6]
        shifted = [1, 1, 1]

        call show_syminfo(num_atom, lattice, symprec, atom_types, positions)
        call show_grid_info(num_atom, lattice, symprec, atom_types, positions, &
                            mesh, shifted, is_time_reversal)

    end subroutine example_spg_get_dataset_D0_24

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
        character(len=128) :: fmt

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
    end subroutine show_syminfo

    subroutine show_grid_info(num_atom, lattice, symprec, atom_types, positions, &
                              mesh, shifted, is_time_reversal)
        use spglib_f08, only: spg_get_spacegroup_type,  spg_get_ir_reciprocal_mesh

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
