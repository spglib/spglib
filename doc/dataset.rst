.. _spglib_dataset:

Spglib dataset
===============

.. contents::
   :depth: 2
   :local:

**At version 1.9.4, SpglibDataset was modified.** The member
name ``setting`` is changed to ``choice`` and ``pointgroup_number`` is
removed.

The dataset is accessible through the C-structure given by

::

   typedef struct {
     int spacegroup_number;
     int hall_number;
     char international_symbol[11];
     char hall_symbol[17];
     char choice[6];
     double transformation_matrix[3][3];
     double origin_shift[3];
     int n_operations;
     int (*rotations)[3][3];
     double (*translations)[3];
     int n_atoms;
     int *wyckoffs;
     char (*site_symmetry_symbols)[7];
     int *equivalent_atoms;
     int *crystallographic_orbits;
     double primitive_lattice[3][3];
     int *mapping_to_primitive;
     int n_std_atoms;
     double std_lattice[3][3];
     int *std_types;
     double (*std_positions)[3];
     double std_rotation_matrix[3][3];
     int *std_mapping_to_primitive;
     char pointgroup_symbol[6];
   } SpglibDataset;

.. _dataset_spg_get_dataset_spacegroup_type:

Space group type
----------------

``spacegroup_number``
^^^^^^^^^^^^^^^^^^^^^^

The space group type number defined in International Tables for
Crystallography (ITA).

``hall_number``
^^^^^^^^^^^^^^^

The serial number from 1 to 530 which are found at `list of space
groups (Seto's web site)
<http://pmsl.planet.sci.kobe-u.ac.jp/~seto/?page_id=37&lang=en>`_. Be
sure that this is not a standard crystallographic defition as far as
the author of spglib knows.

``international_symbol``
^^^^^^^^^^^^^^^^^^^^^^^^^

The (full) Hermann–Mauguin notation of space group type is given by .

``hall_symbol``
^^^^^^^^^^^^^^^^

The Hall symbol is stored here.

``choice``
^^^^^^^^^^^

The information on unique axis, setting or cell choices.

Symmetry operations
--------------------

``rotations``, ``translations``, and ``n_operations``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The symmetry operations of the input unit cell are stored in
``rotations`` and ``translations``. A crystallographic symmetry
operation :math:`(\boldsymbol{W}, \boldsymbol{w})` is made from a pair
of rotation :math:`\boldsymbol{W}` and translation
:math:`\boldsymbol{w}` parts with the same index. Number of symmetry
operations is given as ``n_operations``. The detailed explanation of
the values is found at :ref:`api_spg_get_symmetry`.

.. _dataset_spg_get_dataset_site_symmetry:

Wyckoff positions and symmetrically equivalent atoms
-----------------------------------------------------

``n_atoms``
^^^^^^^^^^^^

Number of atoms in the input unit cell. This gives the numbers of
elements in ``wyckoffs`` and ``equivalent_atoms``.

``wyckoffs``
^^^^^^^^^^^^^

This gives the information of Wyckoff letters by integer
numbers, where 0, 1, 2, :math:`\ldots`, represent the Wyckoff letters
of a, b, c, :math:`\ldots`. These are assigned to all atomic positions
of the input unit cell in this order. Therefore the number of elements in
``wyckoffs`` is same as the number of atoms in the input unit cell,
which is given by ``n_atoms``.

This is determined from the symmetry of the primitive cell.

``site_symmetry_symbols``
^^^^^^^^^^^^^^^^^^^^^^^^^^

This gives site-symmetry symbols. These are valid for the standard
settings. For different settings and choices belonging to the same
space group type, the same set of the symbols is returned.

This is determined from the symmetry of the primitive cell.

``equivalent_atoms``
^^^^^^^^^^^^^^^^^^^^^

This gives the mapping table from the atomic indices of the input unit
cell to the atomic indices of symmetrically independent atom, such as
``[0, 0, 0, 0, 4, 4, 4, 4]``, where the symmetrically independent
atomic indices are 0 and
4. We can see that the atoms from 0 to 3 are mapped to 0 and those
from 4 to 7 are mapped to 4.  The number of elements in
``equivalent_atoms`` is same as the number of atoms in the input unit
cell, which is given by ``n_atoms``.

Symmetry operations found for the input cell are used to determine the
equivalent atoms. ``equivalent_atoms`` and ``crystallographic_orbits``
are almost equivalent, but they can be different in a special
case as written in :ref:`api_spg_get_symmetry`.

``crystallographic_orbits``
^^^^^^^^^^^^^^^^^^^^^^^^^^^

**New at version 1.15**

This is almost equivalent to ``equivalent_atoms``. But symmetry of the
primitive cell is used to determine the symmetrically equivalent atoms.

.. _dataset_origin_shift_and_transformation:

Transformation matrix and origin shift
---------------------------------------

``transformation_matrix`` and ``origin_shift``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``transformation_matrix`` (:math:`\boldsymbol{P}`) and
``origin_shift`` (:math:`\boldsymbol{p}`) are obtained as a result of
space-group-type matching under a set of unique axis, setting and cell
choices. These are operated to the basis vectors and atomic point
coordinates of the input unit cell as

.. math::

   ( \mathbf{a}_\mathrm{s} \; \mathbf{b}_\mathrm{s} \;
   \mathbf{c}_\mathrm{s} ) = ( \mathbf{a} \; \mathbf{b} \; \mathbf{c}
   ) \boldsymbol{P}^{-1} \;\; \text{and} \;\; \boldsymbol{x}_\mathrm{s} =
   \boldsymbol{P}\boldsymbol{x} + \boldsymbol{p}
   \;\;(\mathrm{mod}\; \mathbf{1}),

by which the basis vectors are transformed to those of a
standardized unit cell. Atomic point coordinates are shifted so that
symmetry operations have one of possible standard origins. The
detailed definition is presented at
:ref:`definitions_and_conventions`.


At **versions 1.7.x and 1.8 or before**, the set of
``transformation_matrix`` (:math:`\boldsymbol{P}_\text{old}`) and
``origin_shift`` (:math:`\boldsymbol{p}_\text{old}`) was differently defined from
the current definition as follows:

.. math::

   ( \mathbf{a}_\mathrm{s} \; \mathbf{b}_\mathrm{s} \;
   \mathbf{c}_\mathrm{s} ) = ( \mathbf{a} \; \mathbf{b} \; \mathbf{c}
   ) \boldsymbol{P}_\text{old} \;\; \text{and} \;\; \boldsymbol{x}_\mathrm{s} =
   \boldsymbol{P}^{-1}_\text{old}\boldsymbol{x} - \boldsymbol{p}_\text{old}
   \;\;(\mathrm{mod}\; \mathbf{1}).

.. _dataset_idealized_cell:

Standardized crystal structure after idealization
--------------------------------------------------

``n_std_atoms``, ``std_lattice``, ``std_types``, and ``std_positions``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The standardized crystal structure after :ref:`idealization
<def_idealize_cell>` corresponding to a Hall symbol is stored in
``n_std_atoms``, ``std_lattice``, ``std_types``, and
``std_positions``. These output usually contains the rotation in Cartesian
coordinates and rearrangement of the order atoms with respect to the
input unit cell.

**At versions 1.7.x and 1.8 or before**, the variable names of the
members corresponding to those above are ``n_brv_atoms``,
``brv_lattice``, ``brv_types``, and ``brv_positions``, respectively.

.. _dataset_std_rotation_matrix:

``std_rotation_matrix``
^^^^^^^^^^^^^^^^^^^^^^^

**New in version 1.11**

Rotation matrix that rotates the standardized crystal structure
before idealization :math:`( \mathbf{a}_\mathrm{s} \;
\mathbf{b}_\mathrm{s} \; \mathbf{c}_\mathrm{s} )` to that after
idealization :math:`( \bar{\mathbf{a}}_\mathrm{s} \;
\bar{\mathbf{b}}_\mathrm{s} \; \bar{\mathbf{c}}_\mathrm{s} )` in
Cartesian coordinates of the given input unit cell. The rotation
matrix :math:`\boldsymbol{R}` is defined by

.. math::

   ( \bar{\mathbf{a}}_\mathrm{s} \;
   \bar{\mathbf{b}}_\mathrm{s} \; \bar{\mathbf{c}}_\mathrm{s} )
   = ( \boldsymbol{R} \mathbf{a}_\mathrm{s} \;
   \boldsymbol{R} \mathbf{b}_\mathrm{s} \; \boldsymbol{R}
   \mathbf{c}_\mathrm{s} ).

``std_mapping_to_primitive``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This is available **at versions 1.10 or later**. This gives a list of
atomic indices in the primitive cell of the standardized crystal
structure, where the same number presents the same atom in the
primitive cell. By collective the atoms having the same number, a set
of relative lattice points in the the standardized crystal structure
is obtained.


Crystallographic point group
-----------------------------

``pointgroup_symbol``
^^^^^^^^^^^^^^^^^^^^^^

**New in version 1.8.1**

.. ``pointgroup_number`` is the serial number of the crystallographic
   point group, which refers `list of space
   groups (Seto's web site)
   <http://pmsl.planet.sci.kobe-u.ac.jp/~seto/?page_id=37&lang=en>`_.

``pointgroup_symbol`` is the symbol of the crystallographic point
group in the Hermann–Mauguin notation. There are 32 crystallographic
point groups::

   1, -1, 2, m, 2/m, 222, mm2, mmm, 4, -4, 4/m, 422, 4mm, -42m, 4/mmm,
   3, -3, 32, 3m, -3m, 6, -6, 6/m, 622, 6mm, -6m2, 6/mmm, 23, m-3,
   432, -43m, m-3m

Intermediate data in symmetry search
------------------------------------

A primitive cell is searched from the translational symemtry. This
primitive cell is given by ``primitive_lattice`` and
``mapping_to_primitive`` below.

``primitive_lattice``
^^^^^^^^^^^^^^^^^^^^^

**New at version 1.15**

Non-standardized basis vectors of a primitive cell in the input
cell.

``mapping_to_primitive``
^^^^^^^^^^^^^^^^^^^^^^^^^

In version 1.10 or later, ``mapping_to_primitive`` is available. This
gives a list of atomic indices in the primitive cell of the input
crystal structure, where the same number presents the same atom in the
primitive cell. By collective the atoms having the same number, a set
of relative lattice points in the the input crystal structure is
obtained.
