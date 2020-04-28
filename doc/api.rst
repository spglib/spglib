C-APIs
======

.. contents:: List of C-APIs
   :depth: 3
   :local:

``spg_get_major_version``, ``spg_get_minor_version``, ``spg_get_micro_version``
--------------------------------------------------------------------------------

**New in version 1.8.3**

Version number of spglib is obtained. These three functions return
integers that correspond to spglib version [major].[minor].[micro].

|

``spg_get_error_code`` and ``spg_get_error_message``
-----------------------------------------------------

**New in version 1.9.5**

**Be careful. These are not thread safe, i.e., only safely usable
when calling one spglib function per process.**

These functions is used to see roughly why spglib failed.

::

   SpglibError spg_get_error_code(void);

::

   char * spg_get_error_message(SpglibError spglib_error);

The ``SpglibError`` type is a enum type as shown below.

::

   typedef enum {
     SPGLIB_SUCCESS = 0,
     SPGERR_SPACEGROUP_SEARCH_FAILED,
     SPGERR_CELL_STANDARDIZATION_FAILED,
     SPGERR_SYMMETRY_OPERATION_SEARCH_FAILED,
     SPGERR_ATOMS_TOO_CLOSE,
     SPGERR_POINTGROUP_NOT_FOUND,
     SPGERR_NIGGLI_FAILED,
     SPGERR_DELAUNAY_FAILED,
     SPGERR_ARRAY_SIZE_SHORTAGE,
     SPGERR_NONE,
   } SpglibError;

The usage is as follows::

   SpglibError error;
   error = spg_get_error_code();
   printf("%s\n", spg_get_error_message(error));

|

.. _api_spg_get_symmetry:

``spg_get_symmetry``
---------------------

This function finds a set of representative symmetry operations for
primitive cells or its extension with lattice translations for
supercells. 0 is returned if it failed.

::

  int spg_get_symmetry(int rotation[][3][3],
                       double translation[][3],
                       const int max_size,
                       const double lattice[3][3],
                       const double position[][3],
                       const int types[],
                       const int num_atom,
                       const double symprec);

The operations are stored in ``rotation`` and ``translation``. The
number of operations is return as the return value. Rotations and
translations are given in fractional coordinates, and ``rotation[i]``
and ``translation[i]`` with same index give a symmetry operations,
i.e., these have to be used together.

As an exceptional case, if a supercell (or non-primitive cell) has the
basis vectors whose lattice breaks crsytallographic point group, the
crystallographic symmetry operations are searched within this broken
symmetry, i.e., at most the crystallographic point group found in this
case is the point group of the lattice. For example, this happens for
the :math:`2\times 1\times 1` supercell of a conventional cubic unit
cell. This may not be understandable in crystallographic sense, but is
practically useful treatment for research in computational materials
science.

|

``spg_get_international``
--------------------------

Space group type is found and returned in international table symbol
to ``symbol`` and also as a number (return value). 0 is returned if
it failed.

::

  int spg_get_international(char symbol[11],
                            const double lattice[3][3],
                            const double position[][3],
                            const int types[],
                            const int num_atom,
                            const double symprec);


|

``spg_get_schoenflies``
-------------------------

Space group type is found and returned in schoenflies to ``symbol``
and also as a number (return value). 0 is returned if it failed.

::

  int spg_get_schoenflies(char symbol[7],
                          const double lattice[3][3],
                          const double position[][3],
                          const int types[],
                          const int num_atom,
                          const double symprec);



|

``spg_standardize_cell``
-------------------------

The standardized unit cell (see :ref:`def_standardized_unit_cell`) is
generated from an input unit cell structure and its symmetry found by
the symmetry search. The choice of the setting for each space group
type is as explained for :ref:`spg_get_dataset <api_spg_get_dataset>`.
Usually ``to_primitive=0`` and ``no_idealize=0`` are recommended to
set and this setting results in the same behavior as
``spg_refine_cell``. 0 is returned if it failed.

::

   int spg_standardize_cell(double lattice[3][3],
                            double position[][3],
                            int types[],
                            const int num_atom,
                            const int to_primitive,
                            const int no_idealize,
                            const double symprec);

Number of atoms in the found standardized unit (primitive) cell is
returned.

``to_primitive=1`` is used to create the standardized primitive cell
with the transformation matricies shown at
:ref:`def_standardized_primitive_cell`, otherwise ``to_primitive=0``
must be specified. The found basis vectors and
atomic point coordinates and types are overwritten in ``lattice``,
``position``, and ``types``, respectively. Therefore with
``to_primitive=0``, at a maximum four times larger array size for
``position`` and ``types`` than the those size of the input unit cell
is required to store a standardized unit cell with face centring found
in the case that the input unit cell is a primitive cell.

``no_idealize=0`` is used to idealize the lengths and angles of basis
vectors with adjusting the positions of atoms to nearest exact
positions according to crystal symmetry. However the crystal can be
rotated in Cartesian coordinates by the idealization of the basis
vectors.  ``no_idealize=1`` disables this. The detail of the
idealization (``no_idealize=0``) is written at
:ref:`def_idealize_cell`. ``no_idealize=1`` may be useful when we want
to leave basis vectors and atomic positions in Cartesianl coordinates
fixed.

|

``spg_find_primitive``
-----------------------

**Behavior is changed. This function is now a shortcut of**
``spg_standardize_cell`` **with**
``to_primitive=1`` **and** ``no_idealize=0``.

A primitive cell is found from an input unit cell. 0 is returned if it
failed.

::

  int spg_find_primitive(double lattice[3][3],
                         double position[][3],
                         int types[],
                         const int num_atom,
                         const double symprec);

``lattice``, ``position``, and ``types`` are overwritten. Number of
atoms in the found primitive cell is returned. The crystal can be
rotated by this function. To avoid this, please use
``spg_standardize_cell`` with ``to_primitive=1`` and ``no_idealize=1``
although the crystal structure is not idealized.

|

``spg_refine_cell``
--------------------

**This function exists for backward compatibility since it is same
as** ``spg_standardize_cell`` **with** ``to_primitive=0`` **and**
``no_idealize=0``.

The standardized crystal structure is obtained from a non-standard
crystal structure which may be slightly distorted within a symmetry
recognition tolerance, or whose primitive vectors are differently
chosen, etc. 0 is returned if it failed.

::

  int spg_refine_cell(double lattice[3][3],
                      double position[][3],
                      int types[],
                      const int num_atom,
                      const double symprec);

The calculated standardized lattice and atomic positions overwrites
``lattice``, ``position``, and ``types``. The number of atoms in the
standardized unit cell is returned as the return value. When the input
unit cell is a primitive cell and is the face centring symmetry, the
number of the atoms returned becomes four times large. Since this
function does not have any means of checking the array size (memory
space) of these variables, the array size (memory space) for
``position`` and ``types`` should be prepared **four times more** than
those required for the input unit cell in general.

|

.. _api_spg_get_dataset:

``spg_get_dataset`` and ``spg_get_dataset_with_hall_number``
--------------------------------------------------------------

**Changed in version 1.8.1**

For an input unit cell structure, symmetry operations of the crystal
are searched. Then they are compared with the crsytallographic
database and the space group type is determined.  The result is
returned as the ``SpglibDataset`` structure as a dataset.

The detail of the dataset is given at :ref:`spglib_dataset`.

Dataset corresponding to the space group type in the standard setting
is obtained by ``spg_get_dataset``. Here the standard setting means
the first top one among the Hall symbols listed for each space group
type. For example, H setting (hexagonal lattice) is chosen for
rhombohedral crystals. ``spg_get_dataset_with_hall_number`` explained
below is used to choose different settings such as R setting of
rhombohedral crystals. If this symmetry search fails, ``NULL`` is
returned in version 1.8.1 or later (spacegroup_number = 0 is returned
in the previous versions). In this function, the other
crystallographic setting is not obtained.

::

   SpglibDataset * spg_get_dataset(const double lattice[3][3],
                                   const double position[][3],
                                   const int types[],
                                   const int num_atom,
                                   const double symprec);

To specify the other crystallographic choice (setting, origin, axis,
or cell choice), ``spg_get_dataset_with_hall_number`` is used.

::

   SpglibDataset * spg_get_dataset_with_hall_number(SPGCONST double lattice[3][3],
                                                    SPGCONST double position[][3],
                                                    const int types[],
                                                    const int num_atom,
                                                    const int hall_number,
                                                    const double symprec)

where ``hall_number`` is used to specify the choice. The possible
choices and those serial numbers are found at `list of space groups
(Seto's web site)
<http://pmsl.planet.sci.kobe-u.ac.jp/~seto/?page_id=37&lang=en>`_.
The crystal structure has to possess the space-group type of the Hall
symbol. If the symmetry search fails or the specified ``hall_number``
is not in the list of Hall symbols for the space group type of the
crystal structure, ``spacegroup_number`` in the ``SpglibDataset``
structure is set 0.

Finally, its allocated memory space must be freed by calling
``spg_free_dataset``.

|

``spg_free_dataset``
---------------------

Allocated memoery space of the C-structure of ``SpglibDataset`` is
freed by calling ``spg_free_dataset``.

::

  void spg_free_dataset(SpglibDataset *dataset);

|

.. _api_spg_spacegroup_type:

``spg_get_spacegroup_type``
-----------------------------

**Changed at version 1.9.4: Some members are added and the member name 'setting' is changed to 'choice'.**

This function allows to directly access to the space-group-type
database in spglib (spg_database.c). To specify the space group type
with a specific choice, ``hall_number`` is used. The definition of
``hall_number`` is found at
:ref:`dataset_spg_get_dataset_spacegroup_type`.
``number = 0`` is returned when it failed.

::

   SpglibSpacegroupType spg_get_spacegroup_type(const int hall_number)

``SpglibSpacegroupType`` structure is as follows:

::

   typedef struct {
     int number;
     char international_short[11];
     char international_full[20];
     char international[32];
     char schoenflies[7];
     char hall_symbol[17];
     char choice[6];
     char pointgroup_schoenflies[4];
     char pointgroup_international[6];
     int arithmetic_crystal_class_number;
     char arithmetic_crystal_class_symbol[7];
   } SpglibSpacegroupType;

|

``spg_get_symmetry_from_database``
-----------------------------------

This function allows to directly access to the space group operations
in the spglib database (spg_database.c). To specify the space group
type with a specific choice, ``hall_number`` is used. The definition
of ``hall_number`` is found at
:ref:`dataset_spg_get_dataset_spacegroup_type`. 0 is returned when it
failed.

::

   int spg_get_symmetry_from_database(int rotations[192][3][3],
                                      double translations[192][3],
                                      const int hall_number);

The returned value is the number of space group operations. The space
group operations are stored in ``rotations`` and ``translations``.

|

``spg_get_multiplicity``
-------------------------

This function returns exact number of symmetry operations. 0 is
returned when it failed.

::

  int spg_get_multiplicity(const double lattice[3][3],
                           const double position[][3],
                           const int types[],
                           const int num_atom,
                           const double symprec);

This function may be used in advance to allocate memoery space for
symmetry operations.

|

``spg_get_symmetry_with_collinear_spin``
-----------------------------------------

This function finds symmetry operations with collinear polarizations
(spins) on atoms. Except for the argument of ``const double spins[]``,
the usage is basically the same as ``spg_get_symmetry``, but as an
output, ``equivalent_atoms`` are obtained. The size of this array is
the same of ``num_atom``. See :ref:`dataset_spg_get_dataset_site_symmetry`
for the definition ``equivalent_atoms``. 0 is returned when it failed.

::

  int spg_get_symmetry_with_collinear_spin(int rotation[][3][3],
                                           double translation[][3],
                                           int equivalent_atoms[],
                                           const int max_size,
                                           SPGCONST double lattice[3][3],
                                           SPGCONST double position[][3],
                                           const int types[],
                                           const double spins[],
                                           const int num_atom,
                                           const double symprec);


|

``spg_niggli_reduce``
----------------------

Niggli reduction is applied to input basis vectors ``lattice`` and the
reduced basis vectors are overwritten to ``lattice``. 0 is returned if
it failed.

::

   int spg_niggli_reduce(double lattice[3][3], const double symprec);

The transformation from original basis vectors :math:`( \mathbf{a}
\; \mathbf{b} \; \mathbf{c} )` to final basis vectors :math:`(
\mathbf{a}' \; \mathbf{b}' \; \mathbf{c}' )` is achieved by linear
combination of basis vectors with integer coefficients without
rotating coordinates. Therefore the transformation matrix is obtained
by :math:`\boldsymbol{P} = ( \mathbf{a} \; \mathbf{b} \; \mathbf{c} )
( \mathbf{a}' \; \mathbf{b}' \; \mathbf{c}' )^{-1}` and the matrix
elements have to be almost integers.

|

``spg_delaunay_reduce``
------------------------

Delaunay reduction is applied to input basis vectors ``lattice`` and
the reduced basis vectors are overwritten to ``lattice``. 0 is
returned if it failed.

::

   int spg_delaunay_reduce(double lattice[3][3], const double symprec);

The transformation from original basis vectors :math:`( \mathbf{a}
\; \mathbf{b} \; \mathbf{c} )` to final basis vectors :math:`(
\mathbf{a}' \; \mathbf{b}' \; \mathbf{c}' )` is achieved by linear
combination of basis vectors with integer coefficients without
rotating coordinates. Therefore the transformation matrix is obtained
by :math:`\boldsymbol{P} = ( \mathbf{a} \; \mathbf{b} \; \mathbf{c} )
( \mathbf{a}' \; \mathbf{b}' \; \mathbf{c}' )^{-1}` and the matrix
elements have to be almost integers.


``spg_get_ir_reciprocal_mesh``
-------------------------------

Irreducible reciprocal grid points are searched from uniform mesh grid
points specified by ``mesh`` and ``is_shift``.

::

   int spg_get_ir_reciprocal_mesh(int grid_address[][3],
                                  int map[],
                                  const int mesh[3],
                                  const int is_shift[3],
                                  const int is_time_reversal,
                                  const double lattice[3][3],
                                  const double position[][3],
                                  const int types[],
                                  const int num_atom,
                                  const double symprec)

``mesh`` stores three integers. Reciprocal primitive vectors are
divided by the number stored in ``mesh`` with (0,0,0) point
centering. The center of grid mesh is shifted +1/2 of a grid spacing
along corresponding reciprocal axis by setting 1 to a ``is_shift``
element. No grid mesh shift is made if 0 is set for ``is_shift``.

The reducible uniform grid points are returned in fractional coordinates
as ``grid_address``. A map between reducible and irreducible points are
returned as ``map`` as in the indices of ``grid_address``. The number of
the irreducible k-points are returned as the return value.  The time
reversal symmetry is imposed by setting ``is_time_reversal`` 1.

Grid points are stored in the order that runs left most element
first, e.g. (4x4x4 mesh).::

   [[ 0  0  0]
    [ 1  0  0]
    [ 2  0  0]
    [-1  0  0]
    [ 0  1  0]
    [ 1  1  0]
    [ 2  1  0]
    [-1  1  0]
    ....      ]

where the first index runs first.  k-qpoints are calculated by
``(grid_address + is_shift / 2) / mesh``. A grid point index is
recovered from ``grid_address`` by ``numpy.dot(grid_address % mesh,
[1, mesh[0], mesh[0] * mesh[1]])`` in Python-numpy notation, where
``%`` always returns non-negative integers. The order of
``grid_address`` can be changed so that the last index runs first by
setting the macro ``GRID_ORDER_XYZ`` in ``kpoint.c``. In this case the
grid point index is recovered by ``numpy.dot(grid_address % mesh,
[mesh[2] * mesh[1], mesh[2], 1])``.

|

``spg_get_stabilized_reciprocal_mesh``
---------------------------------------

The irreducible k-points are searched from unique k-point mesh grids
from direct (real space) basis vectors and a set of rotation parts of
symmetry operations in direct space with one or multiple
stabilizers.

::

   int spg_get_stabilized_reciprocal_mesh(int grid_address[][3],
                                          int map[],
                                          const int mesh[3],
                                          const int is_shift[3],
                                          const int is_time_reversal,
                                          const int num_rot,
                                          const int rotations[][3][3],
                                          const int num_q,
                                          const double qpoints[][3])

The stabilizers are written in fractional coordinates. Number of the
stabilizers are given by ``num_q``. Symmetrically equivalent k-points
(stars) in fractional coordinates are stored in ``map`` as indices of
``grid_address``. The number of reduced k-points with the stabilizers
are returned as the return value.

This function can be used to obtain all mesh grid points by setting
``num_rot = 1``, ``rotations = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}``,
``num_q = 1``, and ``qpoints = {0, 0, 0}``.

|

``spg_get_hall_number_from_symmetry``
--------------------------------------

**experimental**

``hall_number`` is obtained from the set of symmetry operations.  The
definition of ``hall_number`` is found at
:ref:`dataset_spg_get_dataset_spacegroup_type` and the corresponding
space-group-type information is obtained through
:ref:`api_spg_spacegroup_type`.

This is expected to work well for the set of symmetry operations whose
distortion is small. The aim of making this feature is to find
space-group-type for the set of symmetry operations given by the other
source than spglib.

Note that the definition of ``symprec`` is
different from usual one, but is given in the fractional
coordinates and so it should be small like ``1e-5``.

::

   int spg_get_hall_number_from_symmetry(SPGCONST int rotation[][3][3],
                                         SPGCONST double translation[][3],
                                         const int num_operations,
                                         const double symprec)
