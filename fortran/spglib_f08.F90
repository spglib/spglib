module spglib_f08

    use iso_c_binding, only: c_char, c_int, c_double, c_ptr, c_null_char, c_f_pointer, c_associated
    use spglib_version

    implicit none

    private
    public :: spg_get_dataset, spg_get_magnetic_dataset, &
            & spg_get_symmetry, spgat_get_symmetry, &
            & spg_get_symmetry_with_collinear_spin, &
            & spgat_get_symmetry_with_collinear_spin, &
            & spg_get_symmetry_with_site_tensors, &
            & spgat_get_symmetry_with_site_tensors, &
            & spg_get_multiplicity, spgat_get_multiplicity, &
            & spg_delaunay_reduce, spg_niggli_reduce, &
            & spg_find_primitive, spgat_find_primitive, &
            & spg_get_international, spgat_get_international, &
            & spg_get_schoenflies, spgat_get_schoenflies, &
            & spg_get_pointgroup, spg_refine_cell, spgat_refine_cell, &
            & spg_standardize_cell, spgat_standardize_cell, &
            & spg_get_ir_reciprocal_mesh, &
            & spg_get_stabilized_reciprocal_mesh, &
            & spg_get_error_code, spg_get_error_message, &
            & spg_get_spacegroup_type, spg_get_spacegroup_type_from_symmetry, &
            & spg_get_symmetry_from_database, &
            & spg_get_magnetic_spacegroup_type, &
            & version, version_full, commit, &
            & spg_get_version, spg_get_version_full, spg_get_commit

    enum, bind(C)
        enumerator ::  SPGLIB_SUCCESS = 0
        enumerator ::  SPGERR_SPACEGROUP_SEARCH_FAILED
        enumerator ::  SPGERR_CELL_STANDARDIZATION_FAILED
        enumerator ::  SPGERR_SYMMETRY_OPERATION_SEARCH_FAILED
        enumerator ::  SPGERR_ATOMS_TOO_CLOSE
        enumerator ::  SPGERR_POINTGROUP_NOT_FOUND
        enumerator ::  SPGERR_NIGGLI_FAILED
        enumerator ::  SPGERR_DELAUNAY_FAILED
        enumerator ::  SPGERR_ARRAY_SIZE_SHORTAGE
        enumerator ::  SPGERR_NONE
    end enum

    type, public :: SpglibSpacegroupType
        integer(c_int) :: number
        character(len=11) :: international_short
        character(len=20) :: international_full
        character(len=32) :: international
        character(len=7) :: schoenflies
        integer(c_int) :: hall_number
        character(len=17) :: hall_symbol
        character(len=6) :: choice
        character(len=6) :: pointgroup_international
        character(len=4) :: pointgroup_schoenflies
        integer(c_int) :: arithmetic_crystal_class_number
        character(len=7) :: arithmetic_crystal_class_symbol
    end type SpglibSpacegroupType

    type, public :: SpglibDataset
        integer(c_int) :: spacegroup_number
        integer(c_int) :: hall_number
        character(len=11) :: international_symbol
        character(len=17) :: hall_symbol
        character(len=6) :: choice
        real(c_double)  :: transformation_matrix(3, 3)
        real(c_double)  :: origin_shift(3)
        integer(c_int) :: n_operations
        integer(c_int), allocatable :: rotations(:, :, :)
        real(c_double), allocatable :: translations(:, :)
        integer(c_int) :: n_atoms
        integer(c_int), allocatable :: wyckoffs(:)
        character(len=7), allocatable :: site_symmetry_symbols(:)
        integer(c_int), allocatable :: equivalent_atoms(:) !Beware mapping refers to positions starting at 0
        integer(c_int), allocatable :: crystallographic_orbits(:) !Beware mapping refers to positions starting at 0
        real(c_double) :: primitive_lattice(3, 3)
        integer(c_int), allocatable :: mapping_to_primitive(:) !Beware mapping refers to positions starting at 0
        integer(c_int) :: n_std_atoms
        real(c_double) :: std_lattice(3, 3)
        integer(c_int), allocatable :: std_types(:)
        real(c_double), allocatable :: std_positions(:, :)
        real(c_double)  :: std_rotation_matrix(3, 3)
        integer(c_int), allocatable :: std_mapping_to_primitive(:) !Beware mapping refers to positions starting at 0
        character(len=6) :: pointgroup_symbol
        integer(kind(SPGLIB_SUCCESS)) :: spglib_error
    end type SpglibDataset

    type, public :: SpglibMagneticSpacegroupType
        integer(c_int) :: uni_number
        integer(c_int) :: litvin_number
        character(len=8) :: bns_number
        character(len=12) :: og_number
        integer(c_int) :: number
        integer(c_int) :: type
    end type SpglibMagneticSpacegroupType

    type, public :: SpglibMagneticDataset
        integer(c_int) :: uni_number
        integer(c_int) :: msg_type
        integer(c_int) :: hall_number
        integer(c_int) :: tensor_rank
        integer(c_int) :: n_operations
        integer(c_int), allocatable :: rotations(:, :, :)
        real(c_double), allocatable :: translations(:, :)
        integer(c_int), allocatable :: time_reversals(:)
        integer(c_int) :: n_atoms
        integer(c_int), allocatable :: equivalent_atoms(:)
        real(c_double) :: transformation_matrix(3, 3)
        real(c_double) :: origin_shift(3)
        integer(c_int) :: n_std_atoms
        real(c_double) :: std_lattice(3, 3)
        integer(c_int), allocatable :: std_types(:)
        real(c_double), allocatable :: std_positions(:, :)
        real(c_double), allocatable :: std_tensors(:)
        real(c_double) :: std_rotation_matrix(3, 3)
        real(c_double) :: primitive_lattice(3, 3)
    end type SpglibMagneticDataset

    ! Constructors
    interface SpglibDataset
        procedure spg_get_dataset
    end interface SpglibDataset

    interface SpglibMagneticDataset
        procedure spg_get_magnetic_dataset
    end interface SpglibMagneticDataset

    interface SpglibSpacegroupType
        procedure spg_get_spacegroup_type
    end interface SpglibSpacegroupType

    interface SpglibMagneticSpacegroupType
        procedure spg_get_magnetic_spacegroup_type
    end interface SpglibMagneticSpacegroupType

    ! Main fortran interface
    interface
        module function spg_get_dataset(lattice, position, types, num_atom, &
                                        symprec) result(dset)
            real(c_double), intent(in) :: lattice(3, 3)
            real(c_double), intent(in) :: position(3, *)
            integer(c_int), intent(in) :: types(*)
            integer(c_int), intent(in), value :: num_atom
            real(c_double), intent(in), value :: symprec
            type(SpglibDataset) :: dset
        end function spg_get_dataset

        module function spg_get_error_message(spglib_error) result(error_message)
            integer(kind(SPGLIB_SUCCESS)) :: spglib_error
            character(len=32) :: error_message
        end function spg_get_error_message

        module function spg_get_version() result(version)
            character(len=16) :: version
        end function spg_get_version

        module function spg_get_version_full() result(version_full)
            character(len=64) :: version_full
        end function spg_get_version_full

        module function spg_get_commit() result(commit)
            character(len=40) :: commit
        end function spg_get_commit

        module function spg_get_magnetic_dataset(lattice, position, types, &
                                                 tensors, tensor_rank, num_atom, is_axial, symprec) result(dset)
            real(c_double), intent(in) :: lattice(3, 3)
            real(c_double), intent(in) :: position(3, *)
            integer(c_int), intent(in) :: types(*)
            real(c_double), intent(in) :: tensors(*)
            integer(c_int), intent(in), value :: tensor_rank
            integer(c_int), intent(in), value :: num_atom
            integer(c_int), intent(in), value :: is_axial
            real(c_double), intent(in), value :: symprec
            type(SpglibMagneticDataset) :: dset
        end function spg_get_magnetic_dataset

        module function spg_get_spacegroup_type(hall_number) result(spgtype)
            integer(c_int), intent(in) :: hall_number
            type(SpglibSpacegroupType) :: spgtype
        end function spg_get_spacegroup_type

        module function spg_get_spacegroup_type_from_symmetry(rotation, translation, max_size, &
                                                              lattice, symprec) result(spgtype)
            integer(c_int), intent(in) :: rotation(3, 3, *)
            real(c_double), intent(in) :: translation(3, *)
            integer(c_int), intent(in), value :: max_size
            real(c_double), intent(in) :: lattice(3, 3)
            real(c_double), intent(in), value :: symprec
            type(SpglibSpacegroupType) :: spgtype
        end function spg_get_spacegroup_type_from_symmetry

        module function spg_get_magnetic_spacegroup_type(uni_number) &
            result(magspgtype)
            integer(c_int), intent(in) :: uni_number
            type(SpglibMagneticSpacegroupType) :: magspgtype
        end function spg_get_magnetic_spacegroup_type
        module function spg_get_stabilized_reciprocal_mesh(grid_point, map, mesh, is_shift, &
             & is_time_reversal, lattice, num_rot, rotations, num_q, qpoints) result(retval)
            !   Beware the map refers to positions starting at 0
            integer(c_int), intent(out) :: grid_point(3, *), map(*)
            integer(c_int), intent(in) :: mesh(3)
            integer(c_int), intent(in) :: is_shift(3)
            integer(c_int), intent(in), value :: is_time_reversal
            real(c_double), intent(in) :: lattice(3, 3)
            integer(c_int), intent(in), value :: num_rot
            integer(c_int), intent(in) :: rotations(3, 3, *)
            integer(c_int), intent(in), value :: num_q
            real(c_double), intent(in) :: qpoints(3, *)
            integer(c_int) :: retval
        end function spg_get_stabilized_reciprocal_mesh
    end interface

    ! Exposed C interface
    interface
        function spg_get_symmetry(rotation, translation, max_size, lattice, &
             & position, types, num_atom, symprec) bind(c) result(retval)
            import c_int, c_double
            integer(c_int), intent(out) :: rotation(3, 3, *)
            real(c_double), intent(out) :: translation(3, *)
            integer(c_int), intent(in), value :: max_size
            real(c_double), intent(in) :: lattice(3, 3), position(3, *)
            integer(c_int), intent(in) :: types(*)
            integer(c_int), intent(in), value :: num_atom
            real(c_double), intent(in), value :: symprec
            integer(c_int) :: retval
        end function spg_get_symmetry

        function spg_get_symmetry_from_database(rotation, translation, &
            & hall_number) bind(c) result(retval)
            import c_int, c_double
            integer(c_int), intent(out) :: rotation(3, 3, *)
            real(c_double), intent(out) :: translation(3, *)
            integer(c_int), intent(in), value :: hall_number
            integer(c_int) :: retval
        end function spg_get_symmetry_from_database

        function spgat_get_symmetry(rotation, translation, max_size, lattice, &
             & position, types, num_atom, symprec, angle_tolerance) bind(c) &
                result(retval)
            import c_int, c_double
            integer(c_int), intent(out) :: rotation(3, 3, *)
            real(c_double), intent(out) :: translation(3, *)
            integer(c_int), intent(in), value :: max_size
            real(c_double), intent(in) :: lattice(3, 3), position(3, *)
            integer(c_int), intent(in) :: types(*)
            integer(c_int), intent(in), value :: num_atom
            real(c_double), intent(in), value :: symprec, angle_tolerance
            integer(c_int) :: retval
        end function spgat_get_symmetry

        function spg_get_symmetry_with_collinear_spin(rotation, translation, &
             & equivalent_atoms, max_size, lattice, position, types, spins, &
             & num_atom, symprec) bind(c) result(retval)
            import c_int, c_double
            integer(c_int), intent(out) :: rotation(3, 3, *)
            real(c_double), intent(out) :: translation(3, *)
            integer(c_int), intent(out) :: equivalent_atoms(*)
            integer(c_int), intent(in), value :: max_size
            real(c_double), intent(in) :: lattice(3, 3), position(3, *)
            integer(c_int), intent(in) :: types(*)
            real(c_double), intent(in) :: spins(*)
            integer(c_int), intent(in), value :: num_atom
            real(c_double), intent(in), value :: symprec
            integer(c_int) :: retval
        end function spg_get_symmetry_with_collinear_spin

        function spgat_get_symmetry_with_collinear_spin(rotation, translation, &
             & equivalent_atoms, max_size, lattice, position, types, spins, &
             & num_atom, symprec, angle_tolerance) bind(c) result(retval)
            import c_int, c_double
            integer(c_int), intent(out) :: rotation(3, 3, *)
            real(c_double), intent(out) :: translation(3, *)
            integer(c_int), intent(out) :: equivalent_atoms(*)
            integer(c_int), intent(in), value :: max_size
            real(c_double), intent(in) :: lattice(3, 3), position(3, *)
            integer(c_int), intent(in) :: types(*)
            real(c_double), intent(in) :: spins(*)
            integer(c_int), intent(in), value :: num_atom
            real(c_double), intent(in), value :: symprec, angle_tolerance
            integer(c_int) :: retval
        end function spgat_get_symmetry_with_collinear_spin

        function spg_get_symmetry_with_site_tensors(rotation, translation, &
             & equivalent_atoms, primitive_lattice, spin_flips, max_size, lattice, &
             & position, types, tensors, tensor_rank, num_atom, with_time_reversal, &
             & is_axial, symprec) bind(c) result(retval)
            import c_int, c_double
            integer(c_int), intent(out) :: rotation(3, 3, *)
            real(c_double), intent(out) :: translation(3, *)
            integer(c_int), intent(out) :: equivalent_atoms(*)
            real(c_double), intent(out) :: primitive_lattice(3, 3)
            integer(c_int), intent(out) :: spin_flips(*)
            integer(c_int), intent(in), value :: max_size
            real(c_double), intent(in) :: lattice(3, 3), position(3, *)
            integer(c_int), intent(in) :: types(*)
            real(c_double), intent(in) :: tensors(*)
            integer(c_int), intent(in), value :: tensor_rank
            integer(c_int), intent(in), value :: num_atom
            integer(c_int), intent(in), value :: with_time_reversal
            integer(c_int), intent(in), value :: is_axial
            real(c_double), intent(in), value :: symprec
            integer(c_int) :: retval
        end function spg_get_symmetry_with_site_tensors

        function spgat_get_symmetry_with_site_tensors(rotation, translation, &
             & equivalent_atoms, primitive_lattice, spin_flips, max_size, lattice, &
             & position, types, tensors, tensor_rank, num_atom, with_time_reversal, &
             & is_axial, symprec, angle_tolerance) bind(c) result(retval)
            import c_int, c_double
            integer(c_int), intent(out) :: rotation(3, 3, *)
            real(c_double), intent(out) :: translation(3, *)
            integer(c_int), intent(out) :: equivalent_atoms(*)
            real(c_double), intent(out) :: primitive_lattice(3, 3)
            integer(c_int), intent(out) :: spin_flips(*)
            integer(c_int), intent(in), value :: max_size
            real(c_double), intent(in) :: lattice(3, 3), position(3, *)
            integer(c_int), intent(in) :: types(*)
            real(c_double), intent(in) :: tensors(*)
            integer(c_int), intent(in), value :: tensor_rank
            integer(c_int), intent(in), value :: num_atom
            integer(c_int), intent(in), value :: with_time_reversal
            integer(c_int), intent(in), value :: is_axial
            real(c_double), intent(in), value :: symprec, angle_tolerance
            integer(c_int) :: retval
        end function spgat_get_symmetry_with_site_tensors

        function spg_get_multiplicity(lattice, position, types, num_atom, &
             & symprec) bind(c) result(retval)
            import c_int, c_double
            real(c_double), intent(in) :: lattice(3, 3), position(3, *)
            integer(c_int), intent(in) :: types(*)
            integer(c_int), intent(in), value :: num_atom
            real(c_double), intent(in), value :: symprec
            integer(c_int) :: retval
        end function spg_get_multiplicity

        function spgat_get_multiplicity(lattice, position, types, num_atom, &
             & symprec, angle_tolerance) bind(c) result(retval)
            import c_int, c_double
            real(c_double), intent(in) :: lattice(3, 3), position(3, *)
            integer(c_int), intent(in) :: types(*)
            integer(c_int), intent(in), value :: num_atom
            real(c_double), intent(in), value :: symprec, angle_tolerance
            integer(c_int) :: retval
        end function spgat_get_multiplicity

        function spg_delaunay_reduce(lattice, symprec) bind(c) result(retval)
            import c_int, c_double
            real(c_double), intent(inout) :: lattice(3, 3)
            real(c_double), intent(in), value :: symprec
            integer(c_int) :: retval
        end function spg_delaunay_reduce

        function spg_niggli_reduce(lattice, symprec) bind(c) result(retval)
            import c_int, c_double
            real(c_double), intent(inout) :: lattice(3, 3)
            real(c_double), intent(in), value :: symprec
            integer(c_int) :: retval
        end function spg_niggli_reduce

        function spg_find_primitive(lattice, position, types, num_atom, symprec) &
          & bind(c) result(retval)
            import c_int, c_double
            real(c_double), intent(inout) :: lattice(3, 3), position(3, *)
            integer(c_int), intent(inout) :: types(*)
            integer(c_int), intent(in), value :: num_atom
            real(c_double), intent(in), value :: symprec
            integer(c_int) :: retval
        end function spg_find_primitive

        function spgat_find_primitive(lattice, position, types, num_atom, symprec, &
             & angle_tolerance) bind(c) result(retval)
            import c_int, c_double
            real(c_double), intent(inout) :: lattice(3, 3), position(3, *)
            integer(c_int), intent(inout) :: types(*)
            integer(c_int), intent(in), value :: num_atom
            real(c_double), intent(in), value :: symprec, angle_tolerance
            integer(c_int) :: retval
        end function spgat_find_primitive

        function spg_get_international(symbol, lattice, position, types, num_atom, &
             & symprec) bind(c) result(retval)
            import c_char, c_int, c_double
            character(kind=c_char), intent(out) :: symbol(11)
            real(c_double), intent(in) :: lattice(3, 3), position(3, *)
            integer(c_int), intent(in) :: types(*)
            integer(c_int), intent(in), value :: num_atom
            real(c_double), intent(in), value :: symprec
            integer(c_int) :: retval ! the number corresponding to 'symbol'. 0 on failure
        end function spg_get_international

        function spgat_get_international(symbol, lattice, position, types, num_atom, &
             & symprec, angle_tolerance) bind(c) result(retval)
            import c_char, c_int, c_double
            character(kind=c_char), intent(out) :: symbol(11)
            real(c_double), intent(in) :: lattice(3, 3), position(3, *)
            integer(c_int), intent(in) :: types(*)
            integer(c_int), intent(in), value :: num_atom
            real(c_double), intent(in), value :: symprec, angle_tolerance
            integer(c_int) :: retval ! the number corresponding to 'symbol'. 0 on failure
        end function spgat_get_international

        function spg_get_schoenflies(symbol, lattice, position, types, num_atom, &
             & symprec) bind(c) result(retval)
            import c_char, c_int, c_double
            character(kind=c_char), intent(out) :: symbol(7)
            real(c_double), intent(in) :: lattice(3, 3), position(3, *)
            integer(c_int), intent(in) :: types(*)
            integer(c_int), intent(in), value :: num_atom
            real(c_double), intent(in), value :: symprec
            integer(c_int) :: retval ! the number corresponding to 'symbol'. 0 on failure
        end function spg_get_schoenflies

        function spgat_get_schoenflies(symbol, lattice, position, types, num_atom, &
             & symprec, angle_tolerance) bind(c) result(retval)
            import c_char, c_int, c_double
            character(kind=c_char), intent(out) :: symbol(7)
            real(c_double), intent(in) :: lattice(3, 3), position(3, *)
            integer(c_int), intent(in) :: types(*)
            integer(c_int), intent(in), value :: num_atom
            real(c_double), intent(in), value :: symprec, angle_tolerance
            integer(c_int) :: retval ! the number corresponding to 'symbol'. 0 on failure
        end function spgat_get_schoenflies

        function spg_get_pointgroup(symbol, trans_mat, rotations, num_rotations) &
             & bind(c) result(retval)
            import c_char, c_int, c_double
            character(kind=c_char) :: symbol(6)
            integer(c_int), intent(out) :: trans_mat(3, 3)
            integer(c_int), intent(in) :: rotations(3, 3, *)
            integer(c_int), intent(in), value :: num_rotations
            integer(c_int) :: retval
        end function spg_get_pointgroup

        function spg_refine_cell(lattice, position, types, num_atom, symprec) bind(c) result(retval)
            import c_int, c_double
            real(c_double), intent(inout) :: lattice(3, 3), position(3, *)
            integer(c_int), intent(inout) :: types(*)
            integer(c_int), intent(in), value :: num_atom
            real(c_double), intent(in), value :: symprec
            integer(c_int) :: retval
        end function spg_refine_cell

        function spgat_refine_cell(lattice, position, types, num_atom, symprec, &
             & angle_tolerance) bind(c) result(retval)
            import c_int, c_double
            real(c_double), intent(inout) :: lattice(3, 3), position(3, *)
            integer(c_int), intent(inout) :: types(*)
            integer(c_int), intent(in), value :: num_atom
            real(c_double), intent(in), value :: symprec, angle_tolerance
            integer(c_int) :: retval
        end function spgat_refine_cell

        function spg_standardize_cell(lattice, position, types, num_atom, &
             & to_primitive, no_idealize, symprec) bind(c) result(retval)
            import c_int, c_double
            real(c_double), intent(inout) :: lattice(3, 3), position(3, *)
            integer(c_int), intent(inout) :: types(*)
            integer(c_int), intent(in), value :: num_atom
            integer(c_int), intent(in), value :: to_primitive, no_idealize
            real(c_double), intent(in), value :: symprec
            integer(c_int) :: retval
        end function spg_standardize_cell

        function spgat_standardize_cell(lattice, position, types, num_atom, &
             & to_primitive, no_idealize, symprec, angle_tolerance) bind(c) result(retval)
            import c_int, c_double
            real(c_double), intent(inout) :: lattice(3, 3), position(3, *)
            integer(c_int), intent(inout) :: types(*)
            integer(c_int), intent(in), value :: num_atom
            integer(c_int), intent(in), value :: to_primitive, no_idealize
            real(c_double), intent(in), value :: symprec, angle_tolerance
            integer(c_int) :: retval
        end function spgat_standardize_cell

        function spg_get_ir_reciprocal_mesh(grid_point, map, mesh, is_shift, &
             & is_time_reversal, lattice, position, types, num_atom, symprec) bind(c) result(retval)
            import c_int, c_double
            !   Beware the map refers to positions starting at 0
            integer(c_int), intent(out) :: grid_point(3, *), map(*) ! size is product(mesh)
            integer(c_int), intent(in) :: mesh(3), is_shift(3)
            integer(c_int), intent(in), value :: is_time_reversal
            real(c_double), intent(in) :: lattice(3, 3), position(3, *)
            integer(c_int), intent(in) :: types(*)
            integer(c_int), intent(in), value :: num_atom
            real(c_double), intent(in), value :: symprec
            integer(c_int) :: retval ! the number of points in the reduced mesh
        end function spg_get_ir_reciprocal_mesh

        function spg_get_error_code() bind(c) result(error_code)
            import SPGLIB_SUCCESS
            integer(kind(SPGLIB_SUCCESS)) :: error_code
        end function spg_get_error_code

    end interface

end module spglib_f08

submodule(spglib_f08) impl
    type, bind(c) :: SpglibDataset_c
        integer(c_int) :: spacegroup_number
        integer(c_int) :: hall_number
        character(kind=c_char) :: international_symbol(11)
        character(kind=c_char) :: hall_symbol(17)
        character(kind=c_char) :: choice(6)
        real(c_double) :: transformation_matrix(3, 3)
        real(c_double) :: origin_shift(3)
        integer(c_int) :: n_operations
        type(c_ptr) :: rotations
        type(c_ptr) :: translations
        integer(c_int) :: n_atoms
        type(c_ptr) :: wyckoffs
        type(c_ptr) :: site_symmetry_symbols
        type(c_ptr) :: equivalent_atoms
        type(c_ptr) :: crystallographic_orbits
        real(c_double) :: primitive_lattice(3, 3)
        type(c_ptr) :: mapping_to_primitive
        integer(c_int) :: n_std_atoms
        real(c_double) :: std_lattice(3, 3)
        type(c_ptr) :: std_types
        type(c_ptr) :: std_positions
        real(c_double)  :: std_rotation_matrix(3, 3)
        type(c_ptr) :: std_mapping_to_primitive
        character(kind=c_char) :: pointgroup_symbol(6)
    end type SpglibDataset_c

    type, bind(c) :: SpglibSpacegroupType_c
        integer(c_int) :: number
        character(kind=c_char) :: international_short(11)
        character(kind=c_char) :: international_full(20)
        character(kind=c_char) :: international(32)
        character(kind=c_char) :: schoenflies(7)
        integer(c_int) :: hall_number
        character(kind=c_char) :: hall_symbol(17)
        character(kind=c_char) :: choice(6)
        character(kind=c_char) :: pointgroup_international(6)
        character(kind=c_char) :: pointgroup_schoenflies(4)
        integer(c_int) :: arithmetic_crystal_class_number
        character(kind=c_char) :: arithmetic_crystal_class_symbol(7)
    end type SpglibSpacegroupType_c

    type, bind(c) :: SpglibMagneticDataset_c
        integer(c_int) :: uni_number
        integer(c_int) :: msg_type
        integer(c_int) :: hall_number
        integer(c_int) :: tensor_rank
        integer(c_int) :: n_operations
        type(c_ptr) :: rotations
        type(c_ptr) :: translations
        type(c_ptr) :: time_reversals
        integer(c_int) :: n_atoms
        type(c_ptr) :: equivalent_atoms
        real(c_double) :: transformation_matrix(3, 3)
        real(c_double) :: origin_shift(3)
        integer(c_int) :: n_std_atoms
        real(c_double) :: std_lattice(3, 3)
        type(c_ptr) :: std_types
        type(c_ptr) :: std_positions
        type(c_ptr) :: std_tensors
        real(c_double) :: std_rotation_matrix(3, 3)
        real(c_double) :: primitive_lattice(3, 3)
    end type SpglibMagneticDataset_c

    type, bind(c) :: SpglibMagneticSpacegroupType_c
        integer(c_int) :: uni_number
        integer(c_int) :: litvin_number
        character(kind=c_char) :: bns_number(8)
        character(kind=c_char) :: og_number(12)
        integer(c_int) :: number
        integer(c_int) :: type
    end type SpglibMagneticSpacegroupType_c

    interface
        function spg_get_version_c() &
                & bind(c, name='spg_get_version') result(version)
            import c_ptr
            type(c_ptr) :: version
        end function spg_get_version_c

        function spg_get_version_full_c() &
                & bind(c, name='spg_get_version_full') result(version_full)
            import c_ptr
            type(c_ptr) :: version_full
        end function spg_get_version_full_c

        function spg_get_commit_c() &
                & bind(c, name='spg_get_commit') result(commit)
            import c_ptr
            type(c_ptr) :: commit
        end function spg_get_commit_c

        function spg_get_error_message_c(spglib_error_c) &
                & bind(c, name='spg_get_error_message') result(error_message_c)
            import c_ptr, SPGLIB_SUCCESS

            integer(kind(SPGLIB_SUCCESS)), value :: spglib_error_c
            type(c_ptr) :: error_message_c

        end function spg_get_error_message_c

        function spg_get_spacegroup_type_c(hall_number_c) &
                & bind(c, name='spg_get_spacegroup_type') result(spgtype_c)
            import c_int, SpglibSpacegroupType_c
            integer(c_int), intent(in), value :: hall_number_c
            type(SpglibSpacegroupType_c) :: spgtype_c
        end function spg_get_spacegroup_type_c

        function spg_get_spacegroup_type_from_symmetry_c(rotation_c, translation_c, max_size_c, lattice_c, symprec_c) &
                & bind(c, name='spg_get_spacegroup_type_from_symmetry') result(spgtype_c)
            import c_int, c_double, SpglibSpacegroupType_c
            integer(c_int), intent(in) :: rotation_c(3, 3, *)
            real(c_double), intent(in) :: translation_c(3, *)
            integer(c_int), intent(in), value :: max_size_c
            real(c_double), intent(in) :: lattice_c(3, 3)
            real(c_double), intent(in), value :: symprec_c

            type(SpglibSpacegroupType_c) :: spgtype_c
        end function spg_get_spacegroup_type_from_symmetry_c

        function spg_get_stabilized_reciprocal_mesh_c(grid_point, map, mesh, is_shift, &
             & is_time_reversal, num_rot, rotations, num_q, qpoints) &
            bind(c, name="spg_get_stabilized_reciprocal_mesh") result(retval)
            import c_int, c_double
            !   Beware the map refers to positions starting at 0
            integer(c_int), intent(out) :: grid_point(3, *), map(*)
            integer(c_int), intent(in) :: mesh(3)
            integer(c_int), intent(in) :: is_shift(3)
            integer(c_int), intent(in), value :: is_time_reversal
            integer(c_int), intent(in), value :: num_rot
            integer(c_int), intent(in) :: rotations(3, 3, *)
            integer(c_int), intent(in), value :: num_q
            real(c_double), intent(in) :: qpoints(3, *)
            integer(c_int) :: retval
        end function spg_get_stabilized_reciprocal_mesh_c

        function spg_get_dataset_c(lattice_c, position_c, types_c, num_atom_c, symprec_c) &
                & bind(c, name='spg_get_dataset') result(retval)
            import c_int, c_double, c_ptr
            real(c_double), intent(in) :: lattice_c(3, 3)
            real(c_double), intent(in) :: position_c(3, *)
            integer(c_int), intent(in) :: types_c(*)
            integer(c_int), intent(in), value :: num_atom_c
            real(c_double), intent(in), value :: symprec_c
            type(c_ptr) :: retval
        end function spg_get_dataset_c

        subroutine spg_free_dataset_c(dataset) bind(c, name='spg_free_dataset')
            import SpglibDataset_c
            type(SpglibDataset_c), intent(inout) :: dataset
        end subroutine spg_free_dataset_c

        function spg_get_magnetic_spacegroup_type_c(uni_number_c) &
                & bind(c, name='spg_get_magnetic_spacegroup_type') result(magspgtype_c)
            import c_int, SpglibMagneticSpacegroupType_c
            integer(c_int), intent(in), value :: uni_number_c
            type(SpglibMagneticSpacegroupType_c) :: magspgtype_c
        end function spg_get_magnetic_spacegroup_type_c

        function spg_get_magnetic_dataset_c &
                & (lattice_c, position_c, types_c, tensors_c, tensor_rank_c, num_atom_c, is_axial_c, symprec_c) &
                & bind(c, name='spg_get_magnetic_dataset') result(mag_dset_c)
            import c_int, c_double, c_ptr
            real(c_double), intent(in) :: lattice_c(3, 3)
            real(c_double), intent(in) :: position_c(3, *)
            integer(c_int), intent(in) :: types_c(*)
            real(c_double), intent(in) :: tensors_c(*)
            integer(c_int), intent(in), value :: tensor_rank_c
            integer(c_int), intent(in), value :: num_atom_c
            integer(c_int), intent(in), value :: is_axial_c
            real(c_double), intent(in), value :: symprec_c
            type(c_ptr) :: mag_dset_c
        end function spg_get_magnetic_dataset_c

        subroutine spg_free_magnetic_dataset_c(dataset) bind(c, name='spg_free_magnetic_dataset')
            import SpglibMagneticDataset_c
            type(SpglibMagneticDataset_c), intent(inout) :: dataset
        end subroutine spg_free_magnetic_dataset_c
    end interface
contains

    function c_to_f_string(c_arr) result(f_str)
        character(kind=c_char), dimension(:), intent(in) :: c_arr
        character(:, kind=c_char), allocatable :: c_str
        character(:), allocatable :: f_str

        integer :: l

        ! Find index of any NULL char or size(c_arr,1)+1 if none
        do l = 1, size(c_arr)
            if (c_arr(l) == C_NULL_CHAR) exit
        end do

        allocate (character(l - 1) :: c_str)     ! c_str is a scalar, so as MOLD requires a size for the transfer
        f_str = transfer(c_arr(1:l - 1), c_str)

    end function c_to_f_string

    module function spg_get_version() result(version)
        character(len=16) :: version
        character(kind=c_char), dimension(:), pointer :: f_ptr

        call c_f_pointer(spg_get_version_c(), f_ptr, [len(version)])

        version = c_to_f_string(f_ptr)
    end function spg_get_version

    module function spg_get_version_full() result(version_full)
        character(len=64) :: version_full
        character(kind=c_char), dimension(:), pointer :: f_ptr

        call c_f_pointer(spg_get_version_full_c(), f_ptr, [len(version_full)])

        version_full = c_to_f_string(f_ptr)
    end function spg_get_version_full

    module function spg_get_commit() result(commit)
        character(len=40) :: commit
        character(kind=c_char), dimension(:), pointer :: f_ptr

        call c_f_pointer(spg_get_commit_c(), f_ptr, [len(commit)])

        commit = c_to_f_string(f_ptr)
    end function spg_get_commit

    module function spg_get_error_message(spglib_error) result(error_message)
        integer(kind(SPGLIB_SUCCESS)) :: spglib_error
        character(len=32) :: error_message
        character(kind=c_char), dimension(:), pointer :: f_ptr

        call c_f_pointer(spg_get_error_message_c(spglib_error), f_ptr, [len(commit)])

        error_message = c_to_f_string(f_ptr)
    end function spg_get_error_message

    module function spg_get_spacegroup_type(hall_number) result(spgtype)
        integer(c_int), intent(in) :: hall_number
        type(SpglibSpacegroupType) :: spgtype

        type(SpglibSpacegroupType_c), allocatable:: spgtype_c

        allocate (spgtype_c, source=spg_get_spacegroup_type_c(hall_number))

        spgtype%number = spgtype_c%number
        spgtype%hall_number = spgtype_c%hall_number
        spgtype%arithmetic_crystal_class_number = &
             & spgtype_c%arithmetic_crystal_class_number

        spgtype%international_short = c_to_f_string(spgtype_c%international_short)
        spgtype%international_full = c_to_f_string(spgtype_c%international_full)
        spgtype%international = c_to_f_string(spgtype_c%international)
        spgtype%schoenflies = c_to_f_string(spgtype_c%schoenflies)
        spgtype%hall_symbol = c_to_f_string(spgtype_c%hall_symbol)
        spgtype%choice = c_to_f_string(spgtype_c%choice)
        spgtype%pointgroup_international = c_to_f_string(spgtype_c%pointgroup_international)
        spgtype%pointgroup_schoenflies = c_to_f_string(spgtype_c%pointgroup_schoenflies)
        spgtype%arithmetic_crystal_class_symbol = c_to_f_string(spgtype_c%arithmetic_crystal_class_symbol)

    end function spg_get_spacegroup_type

    module function spg_get_spacegroup_type_from_symmetry(rotation, translation, max_size, &
                                                        & lattice, symprec) result(spgtype)
        integer(c_int), intent(in) :: rotation(3, 3, *)
        real(c_double), intent(in) :: translation(3, *)
        integer(c_int), intent(in), value :: max_size
        real(c_double), intent(in) :: lattice(3, 3)
        real(c_double), intent(in), value :: symprec
        type(SpglibSpacegroupType) :: spgtype

        type(SpglibSpacegroupType_c), allocatable:: spgtype_c

        allocate (spgtype_c, source=spg_get_spacegroup_type_from_symmetry_c( &
                  rotation, translation, max_size, lattice, symprec))

        spgtype%number = spgtype_c%number
        spgtype%hall_number = spgtype_c%hall_number
        spgtype%arithmetic_crystal_class_number = spgtype_c%arithmetic_crystal_class_number

        spgtype%international_short = c_to_f_string(spgtype_c%international_short)
        spgtype%international_full = c_to_f_string(spgtype_c%international_full)
        spgtype%international = c_to_f_string(spgtype_c%international)
        spgtype%schoenflies = c_to_f_string(spgtype_c%schoenflies)
        spgtype%hall_symbol = c_to_f_string(spgtype_c%hall_symbol)
        spgtype%choice = c_to_f_string(spgtype_c%choice)
        spgtype%pointgroup_international = c_to_f_string(spgtype_c%pointgroup_international)
        spgtype%pointgroup_schoenflies = c_to_f_string(spgtype_c%pointgroup_schoenflies)
        spgtype%arithmetic_crystal_class_symbol = c_to_f_string(spgtype_c%arithmetic_crystal_class_symbol)

    end function spg_get_spacegroup_type_from_symmetry

    module function spg_get_dataset(lattice, position, types, num_atom, symprec) result(dset)

        real(c_double), intent(in) :: lattice(3, 3)
        real(c_double), intent(in) :: position(3, *)
        integer(c_int), intent(in) :: types(*)
        integer(c_int), intent(in), value :: num_atom
        real(c_double), intent(in), value :: symprec
        type(SpglibDataset) :: dset

        type(SpglibDataset_c), pointer :: dset_c
        type(c_ptr) :: dataset_ptr_c
        integer(c_int) :: n_operations, n_atoms, n_std_atoms
        real(c_double), pointer :: translations(:, :), std_positions(:, :)
        integer(c_int), pointer :: rotations(:, :, :), wyckoffs(:), equivalent_atoms(:)
        integer(c_int), pointer :: crystallographic_orbits(:), std_types(:)
        character(len=7), pointer :: site_symmetry_symbols(:)
        integer(c_int), pointer :: mapping_to_primitive(:), std_mapping_to_primitive(:)

        dataset_ptr_c = spg_get_dataset_c(lattice, position, types, num_atom, symprec)

        if (c_associated(dataset_ptr_c)) then

            dset%spglib_error = SPGLIB_SUCCESS

            call c_f_pointer(dataset_ptr_c, dset_c)

            dset%spacegroup_number = dset_c%spacegroup_number
            dset%hall_number = dset_c%hall_number
            dset%transformation_matrix = dset_c%transformation_matrix
            dset%origin_shift = dset_c%origin_shift
            dset%n_operations = dset_c%n_operations
            dset%n_atoms = dset_c%n_atoms
            dset%n_std_atoms = dset_c%n_std_atoms
            dset%std_lattice = dset_c%std_lattice
            dset%std_rotation_matrix = dset_c%std_rotation_matrix
            dset%primitive_lattice = dset_c%primitive_lattice

            ! Copy C strings to Fortran characters, converting C NULL to Fortran space padded strings
            dset%international_symbol = c_to_f_string(dset_c%international_symbol)
            dset%hall_symbol = c_to_f_string(dset_c%hall_symbol)
            dset%choice = c_to_f_string(dset_c%choice)
            dset%pointgroup_symbol = c_to_f_string(dset_c%pointgroup_symbol)

            n_operations = dset_c%n_operations
            n_atoms = dset_c%n_atoms
            n_std_atoms = dset_c%n_std_atoms

            call c_f_pointer(dset_c%rotations, rotations, shape=[3, 3, n_operations])
            call c_f_pointer(dset_c%translations, translations, shape=[3, n_operations])
            call c_f_pointer(dset_c%wyckoffs, wyckoffs, shape=[n_atoms])
            call c_f_pointer(dset_c%site_symmetry_symbols, site_symmetry_symbols, shape=[n_atoms])
            call c_f_pointer(dset_c%equivalent_atoms, equivalent_atoms, shape=[n_atoms])
            call c_f_pointer(dset_c%crystallographic_orbits, crystallographic_orbits, shape=[n_atoms])
            call c_f_pointer(dset_c%mapping_to_primitive, mapping_to_primitive, shape=[n_atoms])
            call c_f_pointer(dset_c%std_types, std_types, shape=[n_std_atoms])
            call c_f_pointer(dset_c%std_positions, std_positions, shape=[3, n_std_atoms])
            call c_f_pointer(dset_c%std_mapping_to_primitive, std_mapping_to_primitive, shape=[n_std_atoms])

            allocate (dset%rotations(3, 3, n_operations))
            allocate (dset%translations(3, n_operations))
            allocate (dset%wyckoffs(n_atoms))
            allocate (dset%site_symmetry_symbols(n_atoms))
            allocate (dset%equivalent_atoms(n_atoms))
            allocate (dset%crystallographic_orbits(n_atoms))
            allocate (dset%mapping_to_primitive(n_atoms))
            allocate (dset%std_types(n_std_atoms))
            allocate (dset%std_positions(3, n_std_atoms))
            allocate (dset%std_mapping_to_primitive(n_std_atoms))

            dset%rotations = rotations
            dset%translations = translations
            dset%wyckoffs = wyckoffs
            dset%site_symmetry_symbols = site_symmetry_symbols
            dset%equivalent_atoms = equivalent_atoms
            dset%crystallographic_orbits = crystallographic_orbits
            dset%mapping_to_primitive = mapping_to_primitive
            dset%std_types = std_types
            dset%std_positions = std_positions
            dset%std_mapping_to_primitive = std_mapping_to_primitive

            call spg_free_dataset_c(dset_c)

        else

            dset%spglib_error = spg_get_error_code()

            dset%spacegroup_number = 0
            dset%hall_number = 0
            dset%international_symbol = ' '
            dset%hall_symbol = ' '
            dset%choice = ' '
            dset%transformation_matrix = 0.0_c_double
            dset%origin_shift = 0.0_c_double
            dset%n_operations = 0
            dset%n_atoms = 0
            dset%n_std_atoms = 0
            dset%primitive_lattice = 0.0_c_double
            dset%std_lattice = 0.0_c_double
            dset%std_rotation_matrix = 0.0_c_double
            dset%pointgroup_symbol = ' '

        end if

    end function spg_get_dataset

    module function spg_get_magnetic_spacegroup_type(uni_number) result(magspgtype)
        integer(c_int), intent(in) :: uni_number
        type(SpglibMagneticSpacegroupType) :: magspgtype

        type(SpglibMagneticSpacegroupType_c), allocatable:: magspgtype_c

        allocate (magspgtype_c, source=spg_get_magnetic_spacegroup_type_c(uni_number))

        magspgtype%uni_number = magspgtype_c%uni_number
        magspgtype%litvin_number = magspgtype_c%litvin_number

        magspgtype%og_number = c_to_f_string(magspgtype_c%og_number)
        magspgtype%bns_number = c_to_f_string(magspgtype_c%bns_number)

        magspgtype%number = magspgtype_c%number
        magspgtype%type = magspgtype_c%type

    end function spg_get_magnetic_spacegroup_type

    module function spg_get_magnetic_dataset(lattice, position, types, &
                                             tensors, tensor_rank, num_atom, is_axial, symprec) result(dset)

        real(c_double), intent(in) :: lattice(3, 3)
        real(c_double), intent(in) :: position(3, *)
        integer(c_int), intent(in) :: types(*)
        real(c_double), intent(in) :: tensors(*)
        integer(c_int), intent(in), value :: tensor_rank
        integer(c_int), intent(in), value :: num_atom
        integer(c_int), intent(in), value :: is_axial
        real(c_double), intent(in), value :: symprec
        type(SpglibMagneticDataset) :: dset

        type(SpglibMagneticDataset_c), pointer :: dset_c
        type(c_ptr) :: dataset_ptr_c
        integer(c_int) :: n_operations, n_atoms, n_std_atoms, n_std_tensors
        real(c_double), pointer :: translations(:, :), std_positions(:, :), std_tensors(:)
        integer(c_int), pointer :: rotations(:, :, :), equivalent_atoms(:), time_reversals(:)
        integer(c_int), pointer :: std_types(:)

        dataset_ptr_c = spg_get_magnetic_dataset_c(lattice, position, types, tensors, tensor_rank, num_atom, is_axial, symprec)

        if (c_associated(dataset_ptr_c)) then

            call c_f_pointer(dataset_ptr_c, dset_c)

            dset%uni_number = dset_c%uni_number
            dset%msg_type = dset_c%msg_type
            dset%hall_number = dset_c%hall_number
            dset%tensor_rank = dset_c%tensor_rank
            dset%n_operations = dset_c%n_operations
            dset%n_atoms = dset_c%n_atoms
            dset%transformation_matrix = dset_c%transformation_matrix
            dset%origin_shift = dset_c%origin_shift
            dset%n_std_atoms = dset_c%n_std_atoms
            dset%std_lattice = dset_c%std_lattice
            dset%std_rotation_matrix = dset_c%std_rotation_matrix
            dset%primitive_lattice = dset_c%primitive_lattice

            n_operations = dset_c%n_operations
            n_atoms = dset_c%n_atoms
            n_std_atoms = dset_c%n_std_atoms
            if (dset_c%tensor_rank == 0) then
                n_std_tensors = dset_c%n_std_atoms
            else if (dset_c%tensor_rank == 1) then
                n_std_tensors = dset_c%n_std_atoms*3
            else
                n_std_tensors = 0  ! This must not happen.
            end if

            call c_f_pointer(dset_c%rotations, rotations, shape=[3, 3, n_operations])
            call c_f_pointer(dset_c%translations, translations, shape=[3, n_operations])
            call c_f_pointer(dset_c%time_reversals, time_reversals, shape=[n_operations])
            call c_f_pointer(dset_c%equivalent_atoms, equivalent_atoms, shape=[n_atoms])
            call c_f_pointer(dset_c%std_types, std_types, shape=[n_std_atoms])
            call c_f_pointer(dset_c%std_positions, std_positions, shape=[3, n_std_atoms])
            call c_f_pointer(dset_c%std_tensors, std_tensors, shape=[n_std_tensors])

            allocate (dset%rotations(3, 3, n_operations))
            allocate (dset%translations(3, n_operations))
            allocate (dset%time_reversals(n_operations))
            allocate (dset%equivalent_atoms(n_atoms))
            allocate (dset%std_types(n_std_atoms))
            allocate (dset%std_positions(3, n_std_atoms))
            allocate (dset%std_tensors(n_std_tensors))

            dset%rotations = rotations
            dset%translations = translations
            dset%time_reversals = time_reversals
            dset%equivalent_atoms = equivalent_atoms
            dset%std_types = std_types
            dset%std_positions = std_positions
            dset%std_tensors = std_tensors

            call spg_free_magnetic_dataset_c(dset_c)

        else

            dset%uni_number = 0
            dset%msg_type = 0
            dset%hall_number = 0
            dset%tensor_rank = 0
            dset%n_operations = 0
            dset%n_atoms = 0
            dset%transformation_matrix = 0.0_c_double
            dset%origin_shift = 0.0_c_double
            dset%n_std_atoms = 0
            dset%std_lattice = 0.0_c_double
            dset%std_rotation_matrix = 0.0_c_double
            dset%primitive_lattice = 0.0_c_double

        end if

    end function spg_get_magnetic_dataset

    module function spg_get_stabilized_reciprocal_mesh(grid_point, map, mesh, is_shift, &
         & is_time_reversal, lattice, num_rot, rotations, num_q, qpoints) result(retval)
        !   Beware the map refers to positions starting at 0
        integer(c_int), intent(out) :: grid_point(3, *), map(*)
        integer(c_int), intent(in) :: mesh(3)
        integer(c_int), intent(in) :: is_shift(3)
        integer(c_int), intent(in), value :: is_time_reversal
        real(c_double), intent(in) :: lattice(3, 3)
        integer(c_int), intent(in), value :: num_rot
        integer(c_int), intent(in) :: rotations(3, 3, *)
        integer(c_int), intent(in), value :: num_q
        real(c_double), intent(in) :: qpoints(3, *)
        integer(c_int) :: retval
        ! Keep the existing Fortran lattice argument for source compatibility.
        ! The C routine determines the mesh from rotations and q-points.
        retval = spg_get_stabilized_reciprocal_mesh_c(grid_point, map, mesh, is_shift, &
                                                      is_time_reversal, num_rot, rotations, num_q, qpoints)
    end function spg_get_stabilized_reciprocal_mesh

end submodule impl
