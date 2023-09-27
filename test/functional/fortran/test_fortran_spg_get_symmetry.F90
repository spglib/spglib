module test_spg_get_symmetry
    use spglib_f08, only: spg_get_symmetry
    use test_utils
    use C_interface_module
    use, intrinsic :: iso_fortran_env
    implicit none
contains
    function test_fortran_spg_get_symmetry(argc, argv) bind(C) result(ret)
        use, intrinsic :: iso_c_binding
        integer(c_int), value, intent(in) :: argc
        type(c_ptr), intent(in) :: argv(argc)
        integer(c_int) :: ret

        integer :: len
        character(len=:), allocatable :: str

        if (argc < 2) then
            write (output_unit, *) "test_rutile112"
            call test_rutile112()
            write (output_unit, *) "test_zincblende"
            call test_zincblende()
        else
            len = c_strlen(argv(2))
            allocate (character(len=len) :: str)
            call C_F_string(argv(2), str)
            select case (str)
            case ("test_rutile112")
                call test_rutile112()
            case ("test_zincblende")
                call test_zincblende()
            case default
                write (error_unit, *) "No known sub-test passed"
                ret = 1
                return
            end select
            deallocate (str)
        end if

        ret = 0
    end function test_fortran_spg_get_symmetry

    subroutine test_rutile112() bind(C)
        real(c_double) :: lattice(3, 3)
        real(c_double) :: position(3, 12)
        integer(c_int) :: types(12)

        integer :: i, j, n_sym
        real :: origin_shift(3)
        integer(c_int), allocatable :: rotation(:, :, :)
        real(c_double), allocatable :: translation(:, :)
        real(c_double) :: symprec
        integer(c_int) :: num_atom, max_size

        num_atom = 12
        max_size = num_atom*48

        allocate (rotation(3, 3, max_size))
        allocate (translation(3, max_size))

        lattice(:, :) = reshape([4, 0, 0, 0, 4, 0, 0, 0, 6], [3, 3])
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

        deallocate (translation)
        deallocate (rotation)

        call assert(n_sym, 32)
    end subroutine test_rutile112

    subroutine test_zincblende() bind(C)
        real(c_double) :: lattice(3, 3)
        real(c_double) :: position(3, 4)
        integer(c_int) :: types(4)

        integer :: i, j, n_sym
        real :: origin_shift(3)
        integer(c_int), allocatable :: rotation(:, :, :)
        real(c_double), allocatable :: translation(:, :)
        real(c_double) :: symprec
        integer(c_int) :: num_atom, max_size

        num_atom = 4
        max_size = num_atom*48

        allocate (rotation(3, 3, max_size))
        allocate (translation(3, max_size))

        lattice(:, :) = reshape([ &
                                3.2871687359128612, -1.6435843679564306, 0.0, &
                                0.0, 2.8467716318265182, 0.0, &
                                0.0, 0.0, 5.3045771064003047], [3, 3])
        position(:, :) = reshape([ &
                                 0.3333333333333357, 0.6666666666666643, 0.9996814330926364, &
                                 0.6666666666666643, 0.3333333333333357, 0.4996814330926362, &
                                 0.3333333333333357, 0.6666666666666643, 0.3787615522102606, &
                                 0.6666666666666643, 0.3333333333333357, 0.8787615522102604], [3, 4])
        types(:) = [1, 1, 2, 2]
        origin_shift(:) = [0.1, 0.1, 0.0]
        symprec = 1e-5

        do i = 1, num_atom
            position(:, i) = position(:, i) + origin_shift(:)
        end do

        n_sym = spg_get_symmetry(rotation, translation, max_size, lattice, position, &
                                 types, num_atom, symprec)

        deallocate (translation)
        deallocate (rotation)

        call assert(n_sym, 12)
    end subroutine test_zincblende
end module test_spg_get_symmetry
