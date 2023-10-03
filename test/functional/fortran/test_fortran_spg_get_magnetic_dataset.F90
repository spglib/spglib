module test_spg_get_magnetic_dataset
    use spglib_f08, only: spg_get_magnetic_dataset, SpglibMagneticDataset
    use test_utils
    use C_interface_module
    use, intrinsic :: iso_fortran_env
    implicit none
contains
    function test_fortran_spg_get_magnetic_dataset(argc, argv) bind(C) result(ret)
        use, intrinsic :: iso_c_binding
        integer(c_int), value, intent(in) :: argc
        type(c_ptr), intent(in) :: argv(argc)
        integer(c_int) :: ret

        integer :: len
        character(len=:), allocatable :: str

        if (argc < 2) then
            write (output_unit, *) "test_rutile_type_III"
            call test_rutile_type_III()
            write (output_unit, *) "test_Cr_type_IV"
            call test_Cr_type_IV()
            write (output_unit, *) "test_magnetic_dataset_non_collinear"
            call test_magnetic_dataset_non_collinear()
        else
            len = c_strlen(argv(2))
            allocate (character(len=len) :: str)
            call C_F_string(argv(2), str)
            select case (str)
            case ("test_rutile_type_III")
                call test_rutile_type_III()
            case ("test_Cr_type_IV")
                call test_Cr_type_IV()
            case ("test_magnetic_dataset_non_collinear")
                call test_magnetic_dataset_non_collinear()
            case default
                write (error_unit, *) "No known sub-test passed"
                ret = 1
                return
            end select
            deallocate (str)
        end if

        ret = 0
    end function test_fortran_spg_get_magnetic_dataset

    subroutine test_rutile_type_III() bind(C)
        real(c_double) :: lattice(3, 3)
        real(c_double) :: position(3, 6)
        real(c_double) :: tensors(6)
        integer(c_int) :: types(6)
        integer(c_int) :: tensor_rank, is_axial

        integer :: i, j, num_atom, num_timerev
        real :: origin_shift(3)
        real(c_double) :: symprec
        type(SpglibMagneticDataset) :: dataset

        num_atom = 6
        tensor_rank = 0
        is_axial = 0

        lattice(:, :) = reshape([4, 0, 0, 0, 4, 0, 0, 0, 3], [3, 3])
        position(:, :) = reshape( &
                         [ &
                         0.0, 0.0, 0.0, 0.5, 0.5, 0.5, 0.3, 0.3, 0.0, 0.7, 0.7, 0.0, &
                         0.2, 0.8, 0.5, 0.8, 0.2, 0.5 &
                         ], [3, 6])
        types(:) = [1, 1, 2, 2, 2, 2]
        origin_shift(:) = [0.1, 0.1, 0.0]
        tensors(:) = [1, -1, 0, 0, 0, 0]
        symprec = 1e-5

        do i = 1, num_atom
            position(:, i) = position(:, i) + origin_shift(:)
        end do

        dataset = spg_get_magnetic_dataset(lattice, position, types, tensors, tensor_rank, &
                                           num_atom, is_axial, symprec)

        num_timerev = 0
        do i = 1, dataset%n_operations
            num_timerev = num_timerev + dataset%time_reversals(i)
        end do

        call assert(dataset%n_operations, 16)
        call assert(num_timerev, 8)
    end subroutine test_rutile_type_III

    subroutine test_Cr_type_IV() bind(C)
        real(c_double) :: lattice(3, 3)
        real(c_double) :: position(3, 2)
        real(c_double) :: tensors(2)
        integer(c_int) :: types(2)
        integer(c_int) :: tensor_rank, is_axial

        integer :: i, num_atom, num_timerev
        real :: origin_shift(3)
        real(c_double) :: symprec
        type(SpglibMagneticDataset) :: dataset

        num_atom = 2
        tensor_rank = 0
        is_axial = 0

        lattice(:, :) = reshape([4, 0, 0, 0, 4, 0, 0, 0, 4], [3, 3])
        position(:, :) = reshape([0.0, 0.0, 0.0, 0.5, 0.5, 0.5], [3, 2])
        types(:) = [1, 1]
        origin_shift(:) = [0.1, 0.1, 0.0]
        tensors(:) = [1, -1]
        symprec = 1e-5

        do i = 1, num_atom
            position(:, i) = position(:, i) + origin_shift(:)
        end do

        dataset = spg_get_magnetic_dataset(lattice, position, types, tensors, tensor_rank, &
                                           num_atom, is_axial, symprec)

        num_timerev = 0
        do i = 1, dataset%n_operations
            num_timerev = num_timerev + dataset%time_reversals(i)
        end do
        call assert(dataset%n_operations, 96)
        call assert(num_timerev, 48)
    end subroutine test_Cr_type_IV

    subroutine test_magnetic_dataset_non_collinear() bind(C)
        real(c_double) :: lattice(3, 3)
        real(c_double) :: position(3, 1)
        real(c_double) :: tensors(3)
        integer(c_int) :: types(1)
        integer(c_int) :: tensor_rank, is_axial

        integer :: i, j, num_atom
        real(c_double) :: symprec
        type(SpglibMagneticDataset) :: dataset

        num_atom = 1
        tensor_rank = 1
        is_axial = 1

        lattice(:, :) = reshape([10, 0, 0, 0, 10, 0, 0, 0, 10], [3, 3])
        position(:, :) = reshape([0.0, 0.0, 0.0], [3, 1])
        types(:) = [1]
        tensors(:) = [1, 0, 0]
        symprec = 1e-5

        dataset = spg_get_magnetic_dataset(lattice, position, types, tensors, tensor_rank, &
                                           num_atom, is_axial, symprec)

        call assert(dataset%n_operations, 16)
    end subroutine test_magnetic_dataset_non_collinear
end module test_spg_get_magnetic_dataset
