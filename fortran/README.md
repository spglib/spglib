# Fortran interface

## Design of Fortran interface

Most of the functions are simply thin interfaces to corresponding C functions
using the feature of iso_c_binding added at Fortran 2003. Therefore the memory
order arrays are kept between Fortran and C. This means we have to be careful
that the array dimensions are inverted in use.

In C, since we don't know the length of symmetry operations to be returned, and
the memory spaces are allocated dynamically, we have to free them explicitly by
`spg_free_dataset`. However, when using the Fortran interface, it is unnecessary
to deallocate `SpglibDataset` because it is properly treated inside the Fortran
interface, i.e., data obtained through dynamically allocated arrays by C
`malloc` are copied to Fortran `allocatable` arrays and the C arrays are freed
inside the Fortran interface. Thus, the Fortran allocatable arrays are
automatically deallocated when exiting the scope.

## Conceptual example

A crystal structure is given to a spglib function or subroutine to obtain
symmetry information. `spg_get_dataset` is the function that returns most
fruitful space group information. This returns the result in the Fortran-type of
`SpglibDataset`. To tolerate tiny distortion of crystal structure,
it has to be specified when calling symmetry search function.
No detailed documentation about crystal structure format and APIs for Fortran.
Please read the section above about the design of the Fortran interface, and
then refer documentation for C. The following pages show the details for C.

- [Crystal structure format](https://spglib.github.io/spglib/variable.html)
- [Functions](https://spglib.github.io/spglib/api.html)
- [Spglib dataset](https://spglib.github.io/spglib/dataset.html)

```fortran
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
    ! to keep data order in memory unchanged.
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
    ! The space group number 186 corresponds to the wurtzite-type (P6_3mc).
    ! See https://www.cryst.ehu.es/cgi-bin/cryst/programs/nph-table
    print '(a)', dset%international_symbol
    print '(i0)', dset%spacegroup_number
end program spglib_example
```
