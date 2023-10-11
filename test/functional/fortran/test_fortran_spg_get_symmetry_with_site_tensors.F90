module test_spg_get_symmetry_with_site_tensors
    use spglib_f08, only: spg_get_symmetry_with_site_tensors
    use test_utils
    use C_interface_module
    use, intrinsic :: iso_fortran_env
    implicit none
contains
    function test_fortran_spg_get_symmetry_with_site_tensors(argc, argv) bind(C) result(ret)
        use, intrinsic :: iso_c_binding
        integer(c_int), value, intent(in) :: argc
        type(c_ptr), intent(in) :: argv(argc)
        integer(c_int) :: ret

        integer :: len
        character(len=:), allocatable :: str

        if (argc < 2) then
            write (output_unit, *) "test_site_tensors_rutile_type_III"
            call test_site_tensors_rutile_type_III()
            write (output_unit, *) "test_site_tensors_Cr_type_IV"
            call test_site_tensors_Cr_type_IV()
            write (output_unit, *) "test_magnetic_dataset_non_collinear"
            call test_site_tensors_non_collinear()
        else
            len = c_strlen(argv(2))
            allocate (character(len=len) :: str)
            call C_F_string(argv(2), str)
            select case (str)
            case ("test_site_tensors_rutile_type_III")
                call test_site_tensors_rutile_type_III()
            case ("test_site_tensors_Cr_type_IV")
                call test_site_tensors_Cr_type_IV()
            case ("test_site_tensors_non_collinear")
                call test_site_tensors_non_collinear()
            case default
                write (error_unit, *) "No known sub-test passed"
                ret = 1
                return
            end select
            deallocate (str)
        end if

        ret = 0
    end function test_fortran_spg_get_symmetry_with_site_tensors

    subroutine test_site_tensors_rutile_type_III() bind(C)
        real(c_double) :: lattice(3, 3)
        real(c_double) :: positions(3, 6)
        real(c_double) :: tensors(6)
        real(c_double) :: primitive_lattice(3, 3)
        real(c_double) :: symprec = 1e-5
        integer(c_int) :: types(6)
        integer(c_int) :: n_operations
        integer(c_int) :: num_atom = 6
        integer(c_int) :: with_time_reversal = 1
        integer(c_int) :: tensor_rank = 0
        integer(c_int) :: is_axial = 0
        integer(c_int) :: equivalent_atoms(6)
        ! 96  = 48 * 2
        ! = (max number of order of point group) * (spin degrees of freedom)
        ! 6 = num_atom
        integer(c_int) :: max_size = 6*96
        integer(c_int) :: rotations(3, 3, 6*96)
        real(c_double) :: translations(3, 6*96)
        integer(c_int) :: spin_flips(6*96)

        integer :: i, num_timerev
        real :: origin_shift(3)

        lattice(:, :) = reshape([4, 0, 0, 0, 4, 0, 0, 0, 3], [3, 3])
        positions(:, :) = reshape( &
                          [ &
                          0.0, 0.0, 0.0, 0.5, 0.5, 0.5, 0.3, 0.3, 0.0, 0.7, 0.7, 0.0, &
                          0.2, 0.8, 0.5, 0.8, 0.2, 0.5 &
                          ], [3, 6])
        types(:) = [1, 1, 2, 2, 2, 2]
        origin_shift(:) = [0.1, 0.1, 0.0]
        tensors(:) = [1, -1, 0, 0, 0, 0]

        do i = 1, num_atom
            positions(:, i) = positions(:, i) + origin_shift(:)
        end do

        n_operations = spg_get_symmetry_with_site_tensors( &
                       rotations, translations, equivalent_atoms, &
                       primitive_lattice, spin_flips, max_size, &
                       lattice, positions, types, tensors, tensor_rank, &
                       num_atom, with_time_reversal, is_axial, symprec)

        num_timerev = 0
        do i = 1, n_operations
            num_timerev = num_timerev - (spin_flips(i) - 1)/2
        end do

        call assert(n_operations, 16)
        call assert(num_timerev, 8)
    end subroutine test_site_tensors_rutile_type_III

    subroutine test_site_tensors_Cr_type_IV() bind(C)
        real(c_double) :: lattice(3, 3)
        real(c_double) :: positions(3, 2)
        real(c_double) :: tensors(2)
        real(c_double) :: primitive_lattice(3, 3)
        real(c_double) :: symprec = 1e-5
        integer(c_int) :: types(2)
        integer(c_int) :: n_operations
        integer(c_int) :: num_atom = 2
        integer(c_int) :: with_time_reversal = 1
        integer(c_int) :: tensor_rank = 0
        integer(c_int) :: is_axial = 0
        integer(c_int) :: equivalent_atoms(2)
        ! 96  = 48 * 2
        ! = (max number of order of point group) * (spin degrees of freedom)
        ! 2 = num_atom
        integer(c_int) :: max_size = 2*96
        integer(c_int) :: rotations(3, 3, 2*96)
        real(c_double) :: translations(3, 2*96)
        integer(c_int) :: spin_flips(2*96)

        integer :: i, num_timerev
        real :: origin_shift(3)

        lattice(:, :) = reshape([4, 0, 0, 0, 4, 0, 0, 0, 4], [3, 3])
        positions(:, :) = reshape([0.0, 0.0, 0.0, 0.5, 0.5, 0.5], [3, 2])
        types(:) = [1, 1]
        origin_shift(:) = [0.1, 0.1, 0.0]
        tensors(:) = [1, -1]

        do i = 1, num_atom
            positions(:, i) = positions(:, i) + origin_shift(:)
        end do

        n_operations = spg_get_symmetry_with_site_tensors( &
                       rotations, translations, equivalent_atoms, &
                       primitive_lattice, spin_flips, max_size, &
                       lattice, positions, types, tensors, tensor_rank, &
                       num_atom, with_time_reversal, is_axial, symprec)

        num_timerev = 0
        do i = 1, n_operations
            num_timerev = num_timerev - (spin_flips(i) - 1)/2
        end do

        call assert(n_operations, 96)
        call assert(num_timerev, 48)
    end subroutine test_site_tensors_Cr_type_IV

    subroutine test_site_tensors_non_collinear() bind(C)
        real(c_double) :: lattice(3, 3)
        real(c_double) :: positions(3, 1)
        real(c_double) :: tensors(3)
        real(c_double) :: primitive_lattice(3, 3)
        real(c_double) :: symprec = 1e-5
        integer(c_int) :: types(1)
        integer(c_int) :: n_operations
        integer(c_int) :: num_atom = 1
        integer(c_int) :: with_time_reversal = 1
        integer(c_int) :: tensor_rank = 1
        integer(c_int) :: is_axial = 1
        integer(c_int) :: equivalent_atoms(1)
        ! 96  = 48 * 2
        ! = (max number of order of point group) * (spin degrees of freedom)
        ! 1 = num_atom
        integer(c_int) :: max_size = 1*96
        integer(c_int) :: rotations(3, 3, 1*96)
        real(c_double) :: translations(3, 1*96)
        integer(c_int) :: spin_flips(1*96)

        lattice(:, :) = reshape([10, 0, 0, 0, 10, 0, 0, 0, 10], [3, 3])
        positions(:, :) = reshape([0.0, 0.0, 0.0], [3, 1])
        types(:) = [1]
        tensors(:) = [1, 0, 0]

        n_operations = spg_get_symmetry_with_site_tensors( &
                       rotations, translations, equivalent_atoms, &
                       primitive_lattice, spin_flips, max_size, &
                       lattice, positions, types, tensors, tensor_rank, &
                       num_atom, with_time_reversal, is_axial, symprec)

        call assert(n_operations, 16)
    end subroutine test_site_tensors_non_collinear
end module test_spg_get_symmetry_with_site_tensors
