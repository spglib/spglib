module test_utils
    use, intrinsic :: iso_c_binding
    implicit none

    interface assert
        module procedure assert_int, assert_1D_array_c_int, assert_2D_array_c_int, assert_character
    end interface
contains

    subroutine assert_int(val, ref_val)
        integer, intent(in) :: val, ref_val
        if (val /= ref_val) then
            print '()'
            print '(i0, "/=", i0)', val, ref_val
            error stop
        end if
    end subroutine assert_int

    subroutine assert_1D_array_c_int(vals, ref_vals, size_of_array)
        integer(c_int), intent(in) :: vals(:)
        integer(c_int), intent(in) :: ref_vals(:)
        integer, intent(in) :: size_of_array
        integer :: i

        do i = 1, size_of_array
            if (vals(i) /= ref_vals(i)) then
                print '()'
                print '(i0, ":", i0, " ", i0)', i, vals(i), ref_vals(i)
                error stop
            end if
        end do
    end subroutine assert_1D_array_c_int

    subroutine assert_2D_array_c_int(vals, ref_vals, shape_of_array)
        integer(c_int), intent(in) :: vals(:, :)
        integer(c_int), intent(in) :: ref_vals(:, :)
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
    end subroutine assert_2D_array_c_int

    subroutine assert_character(vals, ref_vals)
        character(*), intent(in) :: vals
        character(*), intent(in) :: ref_vals

        if (vals /= ref_vals) then
            print '()'
            print '(a, "/=", a)', vals, ref_vals
            error stop
        end if
    end subroutine assert_character

end module test_utils
