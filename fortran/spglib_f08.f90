module spglib_f08

  use iso_c_binding, only:  c_char, c_int, c_double, c_ptr, c_null_char, c_f_pointer, c_associated

  implicit none

  private

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

  type :: SpglibSpacegroupType
     integer(c_int) :: number
     character(len=11) :: international_short
     character(len=20) :: international_full
     character(len=32) :: international
     character(len=7) :: schoenflies
     character(len=17) :: hall_symbol
     character(len=6) :: choice
     character(len=6) :: pointgroup_international
     character(len=4) :: pointgroup_schoenflies
     integer(c_int) :: arithmetic_crystal_class_number
     character(len=7) :: arithmetic_crystal_class_symbol
  end type SpglibSpacegroupType

  type :: SpglibDataset
     integer(c_int) :: spacegroup_number
     integer(c_int) :: hall_number
     character(len=11) :: international_symbol
     character(len=17) :: hall_symbol
     character(len=6) :: choice
     real(c_double)  :: transformation_matrix(3,3)
     real(c_double)  :: origin_shift(3)
     integer(c_int) :: n_operations
     integer(c_int), allocatable :: rotations(:,:,:)
     real(c_double), allocatable :: translations(:,:)
     integer(c_int) :: n_atoms
     integer(c_int), allocatable :: wyckoffs(:)
     character(len=7), allocatable :: site_symmetry_symbols(:)
     integer(c_int), allocatable :: equivalent_atoms(:) !Beware mapping refers to positions starting at 0
     integer(c_int), allocatable :: crystallographic_orbits(:) !Beware mapping refers to positions starting at 0
     real(c_double) :: primitive_lattice(3,3)
     integer(c_int), allocatable :: mapping_to_primitive(:) !Beware mapping refers to positions starting at 0
     integer(c_int) :: n_std_atoms
     real(c_double) :: std_lattice(3,3)
     integer(c_int), allocatable :: std_types(:)
     real(c_double), allocatable :: std_positions(:,:)
     real(c_double)  :: std_rotation_matrix(3,3)
     integer(c_int), allocatable :: std_mapping_to_primitive(:) !Beware mapping refers to positions starting at 0
     character(len=6) :: pointgroup_symbol
     integer(kind(SPGLIB_SUCCESS)) :: spglib_error
  end type SpglibDataset

  interface

     function spg_get_symmetry( rotation, translation, max_size, lattice, &
          & position, types, num_atom, symprec) bind(c)
       import c_int, c_double
       integer(c_int), intent(inout) :: rotation(3,3,*)
       real(c_double), intent(inout) :: translation(3,*)
       integer(c_int), intent(in), value :: max_size
       real(c_double), intent(in) :: lattice(3,3), position(3,*)
       integer(c_int), intent(in) :: types(*)
       integer(c_int), intent(in), value :: num_atom
       real(c_double), intent(in), value :: symprec
       integer(c_int) :: spg_get_symmetry
     end function spg_get_symmetry


     function spgat_get_symmetry( rotation, translation, max_size, lattice, &
          & position, types, num_atom, symprec, angle_tolerance) bind(c)
       import c_int, c_double
       integer(c_int), intent(out) :: rotation(3,3,*)
       real(c_double), intent(out) :: translation(3,*)
       integer(c_int), intent(in), value :: max_size
       real(c_double), intent(in) :: lattice(3,3), position(3,*)
       integer(c_int), intent(in) :: types(*)
       integer(c_int), intent(in), value :: num_atom
       real(c_double), intent(in), value :: symprec, angle_tolerance
       integer(c_int) :: spgat_get_symmetry
     end function spgat_get_symmetry


     function spg_get_symmetry_with_collinear_spin( rotation, translation, &
          & equivalent_atoms, max_size, lattice, position, types, spins, &
          & num_atom, symprec) bind(c)
       import c_int, c_double
       integer(c_int), intent(out) :: rotation(3,3,*)
       real(c_double), intent(out) :: translation(3,*)
       integer(c_int), intent(out) :: equivalent_atoms(*)
       integer(c_int), intent(in), value :: max_size
       real(c_double), intent(in) :: lattice(3,3), position(3,*)
       integer(c_int), intent(in) :: types(*)
       real(c_double), intent(in) :: spins(*)
       integer(c_int), intent(in), value :: num_atom
       real(c_double), intent(in), value :: symprec
       integer(c_int) :: spg_get_symmetry_with_collinear_spin
     end function spg_get_symmetry_with_collinear_spin


     function spgat_get_symmetry_with_collinear_spin( rotation, translation, &
          & equivalent_atoms, max_size, lattice, position, types, spins, &
          & num_atom, symprec, angle_tolerance) bind(c)
       import c_int, c_double
       integer(c_int), intent(out) :: rotation(3,3,*)
       real(c_double), intent(out) :: translation(3,*)
       integer(c_int), intent(out) :: equivalent_atoms(*)
       integer(c_int), intent(in), value :: max_size
       real(c_double), intent(in) :: lattice(3,3), position(3,*)
       integer(c_int), intent(in) :: types(*)
       real(c_double), intent(in) :: spins(*)
       integer(c_int), intent(in), value :: num_atom
       real(c_double), intent(in), value :: symprec, angle_tolerance
       integer(c_int) :: spgat_get_symmetry_with_collinear_spin
     end function spgat_get_symmetry_with_collinear_spin


     function spg_get_symmetry_with_site_tensors( rotation, translation, &
          & equivalent_atoms, primitive_lattice, spin_flips, max_size, lattice, &
          & position, types, tensors, tensor_rank, num_atom, is_magnetic, &
          & symprec) bind(c)
       import c_int, c_double
       integer(c_int), intent(inout) :: rotation(3,3,*)
       real(c_double), intent(inout) :: translation(3,*)
       integer(c_int), intent(out) :: equivalent_atoms(*)
       real(c_double), intent(out) :: primitive_lattice(3,3)
       integer(c_int), intent(out) :: spin_flips(*)
       integer(c_int), intent(in), value :: max_size
       real(c_double), intent(in) :: lattice(3,3), position(3,*)
       integer(c_int), intent(in) :: types(*)
       real(c_double), intent(in) :: tensors(*)
       integer(c_int), intent(in), value :: tensor_rank
       integer(c_int), intent(in), value :: num_atom
       integer(c_int), intent(in), value :: is_magnetic
       real(c_double), intent(in), value :: symprec
       integer(c_int) :: spg_get_symmetry_with_site_tensors
     end function spg_get_symmetry_with_site_tensors


     function spgat_get_symmetry_with_site_tensors( rotation, translation, &
          & equivalent_atoms, primitive_lattice, spin_flips, max_size, lattice, &
          & position, types, tensors, tensor_rank, num_atom, is_magnetic, &
          & symprec, angle_tolerance) bind(c)
       import c_int, c_double
       integer(c_int), intent(inout) :: rotation(3,3,*)
       real(c_double), intent(inout) :: translation(3,*)
       integer(c_int), intent(out) :: equivalent_atoms(*)
       real(c_double), intent(out) :: primitive_lattice(3,3)
       integer(c_int), intent(out) :: spin_flips(*)
       integer(c_int), intent(in), value :: max_size
       real(c_double), intent(in) :: lattice(3,3), position(3,*)
       integer(c_int), intent(in) :: types(*)
       real(c_double), intent(in) :: tensors(*)
       integer(c_int), intent(in), value :: tensor_rank
       integer(c_int), intent(in), value :: num_atom
       integer(c_int), intent(in), value :: is_magnetic
       real(c_double), intent(in), value :: symprec, angle_tolerance
       integer(c_int) :: spgat_get_symmetry_with_site_tensors
     end function spgat_get_symmetry_with_site_tensors


     function spg_get_multiplicity( lattice, position, types, num_atom, &
          & symprec) bind(c)
       import c_int, c_double
       real(c_double), intent(in) :: lattice(3,3), position(3,*)
       integer(c_int), intent(in) :: types(*)
       integer(c_int), intent(in), value :: num_atom
       real(c_double), intent(in), value :: symprec
       integer(c_int) :: spg_get_multiplicity
     end function spg_get_multiplicity


     function spgat_get_multiplicity( lattice, position, types, num_atom, &
          & symprec, angle_tolerance) bind(c)
       import c_int, c_double
       real(c_double), intent(in) :: lattice(3,3), position(3,*)
       integer(c_int), intent(in) :: types(*)
       integer(c_int), intent(in), value :: num_atom
       real(c_double), intent(in), value :: symprec, angle_tolerance
       integer(c_int) :: spgat_get_multiplicity
     end function spgat_get_multiplicity


     function spg_delaunay_reduce( lattice, symprec) bind(c)
       import c_int, c_double
       real(c_double), intent(inout) :: lattice(3,3)
       real(c_double), intent(in), value :: symprec
       integer(c_int) :: spg_delaunay_reduce
     end function spg_delaunay_reduce


     function spg_niggli_reduce( lattice, symprec) bind(c)
       import c_int, c_double
       real(c_double), intent(inout) :: lattice(3,3)
       real(c_double), intent(in), value :: symprec
       integer(c_int) :: spg_niggli_reduce
     end function spg_niggli_reduce


     function spg_find_primitive(lattice, position, types, num_atom, symprec) &
       & bind(c)
       import c_int, c_double
       real(c_double), intent(inout) :: lattice(3,3), position(3,*)
       integer(c_int), intent(inout) :: types(*)
       integer(c_int), intent(in), value :: num_atom
       real(c_double), intent(in), value :: symprec
       integer(c_int) :: spg_find_primitive
     end function spg_find_primitive


     function spgat_find_primitive(lattice, position, types, num_atom, symprec, &
          & angle_tolerance) bind(c)
       import c_int, c_double
       real(c_double), intent(inout) :: lattice(3,3), position(3,*)
       integer(c_int), intent(inout) :: types(*)
       integer(c_int), intent(in), value :: num_atom
       real(c_double), intent(in), value :: symprec, angle_tolerance
       integer(c_int) :: spgat_find_primitive
     end function spgat_find_primitive


     function spg_get_international( symbol, lattice, position, types, num_atom, &
          & symprec) bind(c)
       import c_char, c_int, c_double
       character(kind=c_char), intent(out) :: symbol(11)
       real(c_double), intent(in) :: lattice(3,3), position(3, *)
       integer(c_int), intent(in) :: types(*)
       integer(c_int), intent(in), value :: num_atom
       real(c_double), intent(in), value :: symprec
       integer(c_int) :: spg_get_international ! the number corresponding to 'symbol'. 0 on failure
     end function spg_get_international


     function spgat_get_international( symbol, lattice, position, types, num_atom, &
          & symprec, angle_tolerance) bind(c)
       import c_char, c_int, c_double
       character(kind=c_char), intent(out) :: symbol(11)
       real(c_double), intent(in) :: lattice(3,3), position(3, *)
       integer(c_int), intent(in) :: types(*)
       integer(c_int), intent(in), value :: num_atom
       real(c_double), intent(in), value :: symprec, angle_tolerance
       integer(c_int) :: spgat_get_international ! the number corresponding to 'symbol'. 0 on failure
     end function spgat_get_international


     function spg_get_schoenflies( symbol, lattice, position, types, num_atom, &
          & symprec) bind(c)
       import c_char, c_int, c_double
       character(kind=c_char), intent(out) :: symbol(7)
       real(c_double), intent(in) :: lattice(3,3), position(3, *)
       integer(c_int), intent(in) :: types(*)
       integer(c_int), intent(in), value :: num_atom
       real(c_double), intent(in), value :: symprec
       integer(c_int) :: spg_get_schoenflies ! the number corresponding to 'symbol'. 0 on failure
     end function spg_get_schoenflies


     function spgat_get_schoenflies( symbol, lattice, position, types, num_atom, &
          & symprec, angle_tolerance) bind(c)
       import c_char, c_int, c_double
       character(kind=c_char), intent(out) :: symbol(7)
       real(c_double), intent(in) :: lattice(3,3), position(3, *)
       integer(c_int), intent(in) :: types(*)
       integer(c_int), intent(in), value :: num_atom
       real(c_double), intent(in), value :: symprec, angle_tolerance
       integer(c_int) :: spgat_get_schoenflies ! the number corresponding to 'symbol'. 0 on failure
     end function spgat_get_schoenflies


     function spg_get_pointgroup( symbol, trans_mat, rotations, num_rotations) &
          & bind(c)
       import c_char, c_int, c_double
       character(kind=c_char) :: symbol(6)
       integer(c_int), intent(inout) :: trans_mat(3,3)
       integer(c_int), intent(in) :: rotations(3,3,*)
       integer(c_int), intent(in), value :: num_rotations
       integer(c_int) :: spg_get_pointgroup
     end function spg_get_pointgroup


     function spg_refine_cell( lattice, position, types, num_atom, symprec) bind(c)
       import c_int, c_double
       real(c_double), intent(inout) :: lattice(3,3), position(3,*)
       integer(c_int), intent(inout) :: types(*)
       integer(c_int), intent(in), value :: num_atom
       real(c_double), intent(in), value :: symprec
       integer(c_int) :: spg_refine_cell
     end function spg_refine_cell


     function spgat_refine_cell( lattice, position, types, num_atom, symprec, &
          & angle_tolerance) bind(c)
       import c_int, c_double
       real(c_double), intent(inout) :: lattice(3,3), position(3,*)
       integer(c_int), intent(inout) :: types(*)
       integer(c_int), intent(in), value :: num_atom
       real(c_double), intent(in), value :: symprec, angle_tolerance
       integer(c_int) :: spgat_refine_cell
     end function spgat_refine_cell


     function spg_standardize_cell( lattice, position, types, num_atom, &
          & to_primitive, no_idealize, symprec) bind(c)
       import c_int, c_double
       real(c_double), intent(inout) :: lattice(3,3), position(3,*)
       integer(c_int), intent(inout) :: types(*)
       integer(c_int), intent(in), value :: num_atom
       integer(c_int), intent(in), value :: to_primitive, no_idealize
       real(c_double), intent(in), value :: symprec
       integer(c_int) :: spg_standardize_cell
     end function spg_standardize_cell


     function spgat_standardize_cell( lattice, position, types, num_atom, &
          & to_primitive, no_idealize, symprec, angle_tolerance) bind(c)
       import c_int, c_double
       real(c_double), intent(inout) :: lattice(3,3), position(3,*)
       integer(c_int), intent(inout) :: types(*)
       integer(c_int), intent(in), value :: num_atom
       integer(c_int), intent(in), value :: to_primitive, no_idealize
       real(c_double), intent(in), value :: symprec, angle_tolerance
       integer(c_int) :: spgat_standardize_cell
     end function spgat_standardize_cell


     function spg_get_ir_reciprocal_mesh(grid_point, map, mesh, is_shift, &
          & is_time_reversal, lattice, position, types, num_atom, symprec) bind(c)
       import c_int, c_double
       !   Beware the map refers to positions starting at 0
       integer(c_int), intent(out) :: grid_point(3, *), map(*) ! size is product(mesh)
       integer(c_int), intent(in) :: mesh(3), is_shift(3)
       integer(c_int), intent(in), value :: is_time_reversal
       real(c_double), intent(in) :: lattice(3,3), position(3, *)
       integer(c_int), intent(in) :: types(*)
       integer(c_int), intent(in), value :: num_atom
       real(c_double), intent(in), value :: symprec
       integer(c_int) :: spg_get_ir_reciprocal_mesh ! the number of points in the reduced mesh
     end function spg_get_ir_reciprocal_mesh


     function spg_get_stabilized_reciprocal_mesh(grid_point, map, mesh, is_shift, &
          & is_time_reversal, lattice, num_rot, rotations, num_q, qpoints) bind(c)
       import c_int, c_double
       !   Beware the map refers to positions starting at 0
       integer(c_int), intent(out) :: grid_point(3,*), map(*)
       integer(c_int), intent(in) :: mesh(3)
       integer(c_int), intent(in) :: is_shift(3)
       integer(c_int), intent(in), value :: is_time_reversal
       real(c_double), intent(in) :: lattice(3,3)
       integer(c_int), intent(in), value :: num_rot
       integer(c_int), intent(in) :: rotations(3,3,*)
       integer(c_int), intent(in), value :: num_q
       real(c_double), intent(in) :: qpoints(3,*)
       integer(c_int) :: spg_get_stabilized_reciprocal_mesh
     end function spg_get_stabilized_reciprocal_mesh


     function spg_get_error_code() bind(c, name='spg_get_error_code')
       import SPGLIB_SUCCESS
       integer(kind(SPGLIB_SUCCESS)) :: spg_get_error_code
     end function spg_get_error_code

  end interface


  public :: SpglibDataset, spg_get_dataset, &
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
       & SpglibSpacegroupType, spg_get_spacegroup_type


contains


  function spg_get_error_message(spglib_error)
    integer(kind(SPGLIB_SUCCESS)) :: spglib_error
    character(len=32) :: spg_get_error_message

    character, pointer, dimension(:) :: message
    type(c_ptr) :: message_ptr

    integer :: i

    interface

       function spg_get_error_message_c(spglib_error) &
            & bind(c, name='spg_get_error_message')
         import c_ptr, SPGLIB_SUCCESS

         integer(kind(SPGLIB_SUCCESS)), value :: spglib_error
         type(c_ptr) :: spg_get_error_message_c

       end function spg_get_error_message_c

    end interface

    message_ptr = spg_get_error_message_c(spglib_error)
    call c_f_pointer(message_ptr, message, [len(spg_get_error_message)])

    spg_get_error_message = ' '
    do i = 1, len(spg_get_error_message)
       if (message(i) == C_NULL_CHAR) exit
       spg_get_error_message(i:i) = message(i)
    end do

  end function spg_get_error_message


  function spg_get_spacegroup_type(hall_number) result(spgtype)
    integer(c_int), intent(in) :: hall_number
    type(SpglibSpacegroupType) :: spgtype

    type, bind(c) :: SpglibSpacegroupType_c
       integer(c_int) :: number
       character(kind=c_char) :: international_short(11)
       character(kind=c_char) :: international_full(20)
       character(kind=c_char) :: international(32)
       character(kind=c_char) :: schoenflies(7)
       character(kind=c_char) :: hall_symbol(17)
       character(kind=c_char) :: choice(6)
       character(kind=c_char) :: pointgroup_international(6)
       character(kind=c_char) :: pointgroup_schoenflies(4)
       integer(c_int) :: arithmetic_crystal_class_number
       character(kind=c_char) :: arithmetic_crystal_class_symbol(7)
    end type SpglibSpacegroupType_c

    interface

       function spg_get_spacegroup_type_c(hall_number) &
            & bind(c, name='spg_get_spacegroup_type')
         import c_int, SpglibSpacegroupType_c
         integer(c_int), intent(in), value :: hall_number
         type(SpglibSpacegroupType_c) :: spg_get_spacegroup_type_c
       end function spg_get_spacegroup_type_c

    end interface

    type(SpglibSpacegroupType_c):: spgtype_c
    integer :: i

    spgtype_c = spg_get_spacegroup_type_c(hall_number)

    spgtype % number = spgtype_c % number
    spgtype % arithmetic_crystal_class_number = &
         & spgtype_c % arithmetic_crystal_class_number

    do i = 1, size(spgtype_c % international_short)
       if (spgtype_c % international_short(i) == C_NULL_CHAR) then
          spgtype % international_short(i:) = ' '
          exit
       end if
       spgtype % international_short(i:i) = spgtype_c % international_short(i)
    end do

    do i = 1, size(spgtype_c % international_full)
       if (spgtype_c % international_full(i) == C_NULL_CHAR) then
          spgtype % international_full(i:) = ' '
          exit
       end if
       spgtype % international_full(i:i) = spgtype_c % international_full(i)
    end do

    do i = 1, size(spgtype_c % international)
       if (spgtype_c % international(i) == C_NULL_CHAR) then
          spgtype % international(i:) = ' '
          exit
       end if
       spgtype % international(i:i) = spgtype_c % international(i)
    end do

    do i = 1, size(spgtype_c % schoenflies)
       if (spgtype_c % schoenflies(i) == C_NULL_CHAR) then
          spgtype % schoenflies(i:) = ' '
          exit
       end if
       spgtype % schoenflies(i:i) = spgtype_c % schoenflies(i)
    end do

    do i = 1, size(spgtype_c % hall_symbol)
       if (spgtype_c % hall_symbol(i) == C_NULL_CHAR) then
          spgtype % hall_symbol(i:) = ' '
          exit
       end if
       spgtype % hall_symbol(i:i) = spgtype_c % hall_symbol(i)
    end do

    do i = 1, size(spgtype_c % choice)
       if (spgtype_c % choice(i) == C_NULL_CHAR) then
          spgtype % choice(i:) = ' '
          exit
       end if
       spgtype % choice(i:i) = spgtype_c % choice(i)
    end do

    do i = 1, size(spgtype_c % pointgroup_international)
       if (spgtype_c % pointgroup_international(i) == C_NULL_CHAR) then
          spgtype % pointgroup_international(i:) = ' '
          exit
       end if
       spgtype % pointgroup_international(i:i) = &
            & spgtype_c % pointgroup_international(i)
    end do

    do i = 1, size(spgtype_c % pointgroup_schoenflies)
       if (spgtype_c % pointgroup_schoenflies(i) == C_NULL_CHAR) then
          spgtype % pointgroup_schoenflies(i:) = ' '
          exit
       end if
       spgtype % pointgroup_schoenflies(i:i) = &
            & spgtype_c % pointgroup_schoenflies(i)
    end do

    do i = 1, size(spgtype_c % arithmetic_crystal_class_symbol)
       if (spgtype_c % arithmetic_crystal_class_symbol(i) == C_NULL_CHAR) then
          spgtype % arithmetic_crystal_class_symbol(i:) = ' '
          exit
       end if
       spgtype % arithmetic_crystal_class_symbol(i:i) = &
            & spgtype_c % arithmetic_crystal_class_symbol(i)
    end do

  end function spg_get_spacegroup_type

  function spg_get_dataset(lattice, position, types, num_atom, symprec) result(dset)

    real(c_double), intent(in) :: lattice(3,3)
    real(c_double), intent(in) :: position(3,*)
    integer(c_int), intent(in) :: types(*)
    integer(c_int), intent(in), value :: num_atom
    real(c_double), intent(in), value :: symprec
    type(SpglibDataset) :: dset

    type, bind(c) :: SpglibDataset_c
       integer(c_int) :: spacegroup_number
       integer(c_int) :: hall_number
       character(kind=c_char) :: international_symbol(11)
       character(kind=c_char) :: hall_symbol(17)
       character(kind=c_char) :: choice(6)
       real(c_double) :: transformation_matrix(3,3)
       real(c_double) :: origin_shift(3)
       integer(c_int) :: n_operations
       type(c_ptr) :: rotations
       type(c_ptr) :: translations
       integer(c_int) :: n_atoms
       type(c_ptr) :: wyckoffs
       type(c_ptr) :: site_symmetry_symbols
       type(c_ptr) :: equivalent_atoms
       type(c_ptr) :: crystallographic_orbits
       real(c_double) :: primitive_lattice(3,3)
       type(c_ptr) :: mapping_to_primitive
       integer(c_int) :: n_std_atoms
       real(c_double) :: std_lattice(3,3)
       type(c_ptr) :: std_types
       type(c_ptr) :: std_positions
       real(c_double)  :: std_rotation_matrix(3,3)
       type(c_ptr) :: std_mapping_to_primitive
       character(kind=c_char) :: pointgroup_symbol(6)
    end type SpglibDataset_c

    interface
       function spg_get_dataset_c(lattice, position, types, num_atom, symprec) &
            & bind(c, name='spg_get_dataset')
         import c_int, c_double, c_ptr
         real(c_double), intent(in) :: lattice(3,3)
         real(c_double), intent(in) :: position(3,*)
         integer(c_int), intent(in) :: types(*)
         integer(c_int), intent(in), value :: num_atom
         real(c_double), intent(in), value :: symprec
         type(c_ptr) :: spg_get_dataset_c
       end function spg_get_dataset_c

       subroutine spg_free_dataset_c(dataset) bind(c, name='spg_free_dataset')
         import SpglibDataset_c
         type(SpglibDataset_c), intent(inout) :: dataset
       end subroutine spg_free_dataset_c

    end interface

    type(SpglibDataset_c), pointer :: dset_c
    type(c_ptr) :: dataset_ptr_c
    integer(c_int) :: n_operations, n_atoms, n_std_atoms
    integer :: i
    integer(kind(SPGLIB_SUCCESS)) :: SpglibErrcode
    real(c_double), pointer :: translations(:,:), std_positions(:,:)
    integer(c_int), pointer :: rotations(:,:,:), wyckoffs(:), equivalent_atoms(:)
    integer(c_int), pointer :: crystallographic_orbits(:), std_types(:)

    dataset_ptr_c = spg_get_dataset_c(lattice, position, types, num_atom, symprec)

    if (c_associated(dataset_ptr_c)) then

       dset % spglib_error = SPGLIB_SUCCESS

       call c_f_pointer(dataset_ptr_c , dset_c)

       dset % spacegroup_number     = dset_c % spacegroup_number
       dset % hall_number           = dset_c % hall_number
       dset % transformation_matrix = dset_c % transformation_matrix
       dset % origin_shift          = dset_c % origin_shift
       dset % n_operations          = dset_c % n_operations
       dset % n_atoms               = dset_c % n_atoms
       dset % n_std_atoms           = dset_c % n_std_atoms
       dset % std_lattice           = dset_c % std_lattice
       dset % primitive_lattice     = dset_c % primitive_lattice

       ! Copy C strings to Fortran characters, converting C NULL to Fortran space padded strings
       do i = 1, size(dset_c % international_symbol)
          if (dset_c % international_symbol(i) == C_NULL_CHAR) then
             dset % international_symbol(i:) = ' '
             exit
          end if
          dset % international_symbol(i:i) = dset_c % international_symbol(i)
       end do

       do i = 1, size(dset_c % hall_symbol)
          if (dset_c % hall_symbol(i) == C_NULL_CHAR) then
             dset % hall_symbol(i:) = ' '
             exit
          end if
          dset % hall_symbol(i:i) = dset_c % hall_symbol(i)
       end do

       do i = 1, size(dset_c % choice)
          if (dset_c % choice(i) == C_NULL_CHAR) then
             dset % choice(i:) = ' '
             exit
          end if
          dset % choice(i:i) = dset_c % choice(i)
       end do

       do i = 1, size(dset_c % pointgroup_symbol)
          if (dset_c % pointgroup_symbol(i) == C_NULL_CHAR) then
             dset % pointgroup_symbol(i:) = ' '
             exit
          end if
          dset % pointgroup_symbol(i:i) = dset_c % pointgroup_symbol(i)
       end do

       n_operations = dset_c % n_operations
       n_atoms      = dset_c % n_atoms
       n_std_atoms  = dset_c % n_std_atoms

       call c_f_pointer (dset_c % rotations       , rotations       , shape = [3, 3, n_operations])
       call c_f_pointer (dset_c % translations    , translations    , shape = [3,    n_operations])
       call c_f_pointer (dset_c % wyckoffs        , wyckoffs        , shape = [n_atoms])
       call c_f_pointer (dset_c % equivalent_atoms, equivalent_atoms, shape = [n_atoms])
       call c_f_pointer (dset_c % crystallographic_orbits, crystallographic_orbits, shape = [n_atoms])
       call c_f_pointer (dset_c % std_types       , std_types       , shape = [n_std_atoms])
       call c_f_pointer (dset_c % std_positions   , std_positions   , shape = [3, n_std_atoms])

       allocate( dset % rotations       (3, 3, n_operations))
       allocate( dset % translations    (3,    n_operations))
       allocate( dset % wyckoffs        (n_atoms))
       allocate( dset % equivalent_atoms(n_atoms))
       allocate( dset % crystallographic_orbits(n_atoms))
       allocate( dset % std_types       (n_std_atoms))
       allocate( dset % std_positions   (3, n_std_atoms))

       dset % rotations        = rotations
       dset % translations     = translations
       dset % wyckoffs         = wyckoffs
       dset % equivalent_atoms = equivalent_atoms
       dset % crystallographic_orbits = crystallographic_orbits
       dset % std_types        = std_types
       dset % std_positions    = std_positions

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
       dset%std_lattice = 0.0_c_double
       dset%primitive_lattice = 0.0_c_double
       dset%pointgroup_symbol = ' '

    end if

  end function spg_get_dataset


end module spglib_f08
