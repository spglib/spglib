module test_utils
    use, intrinsic :: iso_c_binding
    implicit none

contains

subroutine assert_int(val, ref_val)
    integer, intent(in) :: val, ref_val
    if (val /= ref_val) then
        print '()'
        print '(i0, "/=", i0)', val, ref_val
        error stop
    end if
end subroutine assert_int

subroutine assert_1D_array_c_long(vals, ref_vals, size_of_array)
    integer(c_long), intent(in) :: vals(:)
    integer(c_long), intent(in) :: ref_vals(:)
    integer, intent(in) :: size_of_array
    integer :: i

    do i = 1, size_of_array
        if (vals(i) /= ref_vals(i)) then
            print '()'
            print '(i0, ":", i0, " ", i0)', i, vals(i), ref_vals(i)
            error stop
        end if
    end do
end subroutine assert_1D_array_c_long

subroutine assert_2D_array_c_long(vals, ref_vals, shape_of_array)
    integer(c_long), intent(in) :: vals(:, :)
    integer(c_long), intent(in) :: ref_vals(:, :)
    integer, intent(in) :: shape_of_array(:)
    integer :: i, j

    do i = 1, shape_of_array(1)
        do j = 1, shape_of_array(2)
            if (vals(j, i) /= ref_vals(j, i)) then
                print '()'
                print '("(", i0, ",", i0, "):", i0, " ", i0)', i, j, vals(j, i), ref_vals(j, i)
                error stop
            end if
        end do
    end do
end subroutine assert_2D_array_c_long
end module test_utils
