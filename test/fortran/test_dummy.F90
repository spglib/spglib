function test_dummy(argc, argv) bind(C) result(ret)
    use, intrinsic :: iso_c_binding
    use C_interface_module
    integer(c_int), value, intent(in) :: argc
    type(c_ptr), intent(in) :: argv(argc)
    integer(c_int) :: ret

    integer :: i, len
    character(len = :), allocatable :: str

    do i = 1, argc
        len = c_strlen(argv(i))
        allocate(character(len = len) :: str)
        call C_F_string(argv(i), str)
        print *, 'arg[', i, '] = ', str
        deallocate(str)
    end do

    ret = 0

end function test_dummy
