program spglib_example
    use, intrinsic :: iso_c_binding
    use spglib_f08, only: SpglibDataset, spg_get_dataset
    implicit none

    ! Wurtzite structure (P6_3mc)
    real(c_double) :: lattice(3, 3)
    real(c_double) :: positions(3, 4)
    integer(c_int) :: atom_types(4)
    integer(c_int) :: num_atom = 4
    real(c_double) :: symprec = 1e-5
    type(SpglibDataset) :: dset
    integer :: number

    ! Transposed arrays with respect to those in C format
    ! to keep the same memory order.
    lattice(:, 1) = [3.111, -1.5555, 0.0]
    lattice(:, 2) = [0.0, 2.6942050311733885, 0.0]
    lattice(:, 3) = [0.0, 0.0, 4.988]
    positions(:, 1) = [1.0/3, 2.0/3, 0.0]
    positions(:, 2) = [2.0/3, 1.0/3, 0.5]
    positions(:, 3) = [1.0/3, 2.0/3, 0.6181]
    positions(:, 4) = [2.0/3, 1.0/3, 0.1181]
    atom_types(:) = [1, 1, 2, 2]

    ! No need to deallocate SpglibDataset manually as contrasted to C
    dset = spg_get_dataset(lattice, positions, atom_types, num_atom, symprec)
    print '(a)', dset%international_symbol
    print '(i0)', dset%spacegroup_number
end program spglib_example
