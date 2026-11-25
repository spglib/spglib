module test_stabilized_reciprocal_mesh
    use spglib_f08, only: spg_get_stabilized_reciprocal_mesh
    use test_utils
    use, intrinsic :: iso_c_binding
    implicit none

    interface
        function stabilized_mesh_c(grid, mapping, mesh, shift, time_reversal, &
                                   num_rot, rotations, num_q, qpoints) &
            bind(C, name="spg_get_stabilized_reciprocal_mesh") result(num_ir)
            import c_int, c_double
            integer(c_int), intent(out) :: grid(3, *), mapping(*)
            integer(c_int), intent(in) :: mesh(3), shift(3), rotations(3, 3, *)
            integer(c_int), value :: time_reversal, num_rot, num_q
            real(c_double), intent(in) :: qpoints(3, *)
            integer(c_int) :: num_ir
        end function stabilized_mesh_c
    end interface
contains
    function test_fortran_stabilized_reciprocal_mesh(argc, argv) bind(C) result(ret)
        integer(c_int), value, intent(in) :: argc
        type(c_ptr), intent(in) :: argv(argc)
        integer(c_int) :: ret
        integer(c_int) :: grid(3, 64), mapping(64), ref_grid(3, 64), ref_mapping(64)
        integer(c_int) :: mesh(3), shift(3), rotations(3, 3, 1), num_ir, ref_num_ir
        integer(c_int) :: time_reversal, shifted
        real(c_double) :: lattice(3, 3), qpoints(3, 1)

        lattice = 0.0_c_double
        rotations = 0
        do ret = 1, 3
            lattice(ret, ret) = 1.0_c_double
            rotations(ret, ret, 1) = 1
        end do
        mesh = 4
        qpoints = 0.0_c_double

        do shifted = 0, 1
            shift = shifted
            do time_reversal = 0, 1
                ref_num_ir = stabilized_mesh_c(ref_grid, ref_mapping, mesh, shift, &
                                               time_reversal, 1_c_int, rotations, 1_c_int, qpoints)
                num_ir = spg_get_stabilized_reciprocal_mesh(grid, mapping, mesh, shift, &
                                                            time_reversal, lattice, 1_c_int, rotations, 1_c_int, qpoints)
                call assert(num_ir, ref_num_ir)
                call assert(grid, ref_grid, [3, 64])
                call assert(mapping, ref_mapping, 64)
                if (time_reversal == 0) then
                    call assert(num_ir, 64)
                else if (shifted == 0) then
                    ! Eight time-reversal-invariant points and 28 pairs.
                    call assert(num_ir, 36)
                else
                    call assert(num_ir, 32)
                end if
            end do
        end do
        ret = 0
    end function test_fortran_stabilized_reciprocal_mesh
end module test_stabilized_reciprocal_mesh
