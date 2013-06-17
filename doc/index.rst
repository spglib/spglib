Spglib
------

Spglib is a C library for finding and handling crystal symmetries.

Features
^^^^^^^^

* Find symmetry operations
* Identify space-group type
* Wyckoff position assignment
* Refine crystal structure
* Search irreducible k-points
* Find a primitive cell

Algorithms
^^^^^^^^^^^

* Space-group type finding algorithm: R. W. Grosse-Kunstleve, Acta
  Cryst., A55, 383-395 (1999)

* Crystal refinement algorithm: R. W. Grosse-Kunstleve and
  P. D. Adams, Acta Cryst., A58, 60-65 (2002)

Install
^^^^^^^

1. `Download spglib
   <https://sourceforge.net/project/showfiles.php?group_id=215020>`_.

2. Unzip, configure, and make::

   % tar xvfz spglib-1.4.tar.gz
   % cd spglib-1.4
   % ./configure
   % make

3. Copy the libraries where you want using OpenMP. The libraries may
   be in ``src/.libs`` if you don't ``make install``,

OpenMP
~~~~~~

Bottle neck of symmetry operation search may be eased using the OpenMP
threading. In the case of gcc (> 4.2), set the following environment
variables before running configure script::

   % export LIBS='-lgomp'
   % export CFLAGS='-fopenmp'

Overhead of threading is relatively large when the number of atoms is
small. Therefore the threading is activated when the number of atoms
>= 1000.

Usage
^^^^^

1. Include ``spglib.h``
2. Link ``libsymspg.a`` or ``libsymspg.so``
3. Examples are shown in ``example.c``.

Example
^^^^^^^

A few examples are stored in ``example`` directory. The fortran
interface 

Testing
^^^^^^^

The ruby code ``symPoscar.rb`` in ``test`` directory reads a VASP
POSCAR formatted file and finds the space-group type. The ``-l``
option refines a slightly distorted structure within tolerance
spacified with the ``-s`` option. The ``-o`` option gives the symmetry
operations of the input structure.

::

   ruby symPoscar.rb POSCAR -s 0.1 -o -l


How to compile is found in the README file.

Python extension
^^^^^^^^^^^^^^^^

Python extension for ASE is prepared in the ``python/ase`` directory.
See http://spglib.sourceforge.net/pyspglibForASE/

Fortran interface
^^^^^^^^^^^^^^^^^

Fortran interface ``spglib_f08.f90`` is found in ``example``
directory. This fortran interface is the contribution from Dimitar
Pashov. ``spglib_f.c`` is obsolete and is not recommended to use.

Mailing list
^^^^^^^^^^^^^^^^^

For questions, bug reports, and comments, please visit following
mailing list:

https://lists.sourceforge.net/lists/listinfo/spglib-users

For more information
^^^^^^^^^^^^^^^^^^^^

* Project web pages: https://sourceforge.net/projects/spglib
* Download: https://sourceforge.net/project/showfiles.php?group_id=215020
* Subversion: svn co https://spglib.svn.sourceforge.net/svnroot/spglib/trunk spglib
* License: New BSD after version 1.0.beta-1. The older versions are
  under GPLv2
* Contact: atz.togo@gmail.com
* Authour: Atsushi Togo

Acknowledgments
^^^^^^^^^^^^^^^^^

Spglib project acknowledges Yusuke Seto for the Crystallographic
database and Dimitar Pashov for the fortran interface.

Important variables
--------------------

lattice
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Basis vectors (in Cartesian)

::

  [ [ a_x, b_x, c_x ],
    [ a_y, b_y, c_y ],
    [ a_z, b_z, c_z ] ]

Cartesian position :math:`\mathbf{x}_\mathrm{Cart}` is obtained by

.. math::

  \mathbf{x}_\mathrm{Cart} = \mathrm{L}\cdot\mathbf{x}_\mathrm{frac}

where :math:`\mathrm{L}` is the basis vectors defined above and is
:math:`\mathrm{L}=(\mathbf{a},\mathbf{b},\mathbf{c})`, and
:math:`\mathbf{x}_\mathrm{frac}` is the atomic position in fractional
coordinates given below.


position
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Atomic positions (in fractional coordinates)

::

  [ [ x1_a, x1_b, x1_c ], 
    [ x2_a, x2_b, x2_c ], 
    [ x3_a, x3_b, x3_c ], 
    ...                   ]

types
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Atom types, i.e., species are differenciated by integer numbers.

::

  [ type_1, type_2, type_3, ... ]

rotation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Rotation matricies (ingeger) of symmetry operations.

::

    [ [ r_aa, r_ab, r_ac ],
      [ r_ba, r_bb, r_bc ],
      [ r_ca, r_cb, r_cc ] ]

translation
^^^^^^^^^^^^

Translation vectors corresponding to symmetry operations in fractional
coordinates.

::

    [ t_a, t_b, t_c ]

symprec
^^^^^^^

Tolerance of distance between atomic positions and between lengths of
lattice vectors to be tolerated in the symmetry finding. The angle
distortion between lattice vectors is converted to a length and
compared with this distance tolerance. If the explicit angle tolerance
is expected, see ``angle_tolerance``.

angle_tolerance
^^^^^^^^^^^^^^^^

**Experimental**

Tolerance of angle between lattice vectors in degrees to be tolerated
in the symmetry finding. To use angle tolerance, another set of
functions are prepared as follows

::

   spgat_get_dataset
   spgat_get_symmetry
   spgat_get_symmetry_with_collinear_spin
   spgat_get_multiplicity
   spgat_find_primitive
   spgat_get_international
   spgat_get_schoenflies
   spgat_refine_cell

These functions are called by the same way with an additional argument
of 'const double angle_tolerance' in degrees. By specifying a negative
value, the behavior becomes the same as usual functions. The default
value of ``angle_tolerance`` is a negative value.

Definition of symmetry operation
---------------------------------

Definition of the operation:

::

    x' = r * x + t:
    [ x'_a ]   [ r_aa r_ab r_ac ]   [ x_a ]   [ t_a ]
    [ x'_b ] = [ r_ba r_bb r_bc ] * [ x_b ] + [ t_b ]
    [ x'_c ]   [ r_ca r_cb r_cc ]   [ x_c ]   [ t_c ]

.. math::

  \mathbf{x}' = \mathrm{R}\cdot\mathbf{x}

where :math:`\mathrm{R}` is the rotation matrix defined above.

API
---

``spg_get_symmetry``
^^^^^^^^^^^^^^^^^^^^

::

  int spg_get_symmetry(int rotation[][3][3],
  		       double translation[][3],
  		       const int max_size,
		       const double lattice[3][3],
  		       const double position[][3],
		       const int types[],
  		       const int num_atom,
		       const double symprec);

Find symmetry operations. The operations are stored in ``rotatiion``
and ``translation``. The number of operations is return as the return
value. Rotations and translations are given in fractional coordinates,
and ``rotation[i]`` and ``translation[i]`` with same index give a
symmetry oprations, i.e., these have to be used togather.

``spg_get_symmetry_with_collinear_spin``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

::

  int spg_get_symmetry_with_collinear_spin(int rotation[][3][3],
                                           double translation[][3],
                                           const int max_size,
                                           SPGCONST double lattice[3][3],
                                           SPGCONST double position[][3],
                                           const int types[],
                                           const double spins[],
                                           const int num_atom,
                                           const double symprec);

Find symmetry operations with collinear spins on atoms. Except for the
argument of ``const double spins[]``, the usage is same as
``spg_get_symmetry``.

``spg_get_international``
^^^^^^^^^^^^^^^^^^^^^^^^^^

::

  int spg_get_international(char symbol[11],
                            const double lattice[3][3],
                            const double position[][3],
                            const int types[],
			    const int num_atom,
                            const double symprec);

Space group is found in international table symbol (``symbol``) and
as number (return value). 0 is returned when it fails.

``spg_get_schoenflies``
^^^^^^^^^^^^^^^^^^^^^^^^

::

  int spg_get_schoenflies(char symbol[10], const double lattice[3][3],
                          const double position[][3],
                          const int types[], const int num_atom,
                          const double symprec);

Space group is found in schoenflies (``symbol``) and as number (return
value).  0 is returned when it fails.


``spg_find_primitive``
^^^^^^^^^^^^^^^^^^^^^^^

::
  
  int spg_find_primitive(double lattice[3][3],
                         double position[][3],
                         int types[],
			 const int num_atom,
			 const double symprec);

A primitive cell is found from an input cell. Be careful that 
``lattice``, ``position``, and ``types`` are overwritten. ``num_atom``
is returned as return value.

``spg_refine_cell``
^^^^^^^^^^^^^^^^^^^^^

::

  int spg_refine_cell(double lattice[3][3],
		      double position[][3],
		      int types[],
		      const int num_atom,
 		      const double symprec);

Returned Bravais lattice and symmetrized atomic positions are
overwritten. The number of atoms in the Bravais lattice is returned as
the return value. The memory space for ``position`` and ``types`` must
be prepared four times more than those required for the input
structures. This is because, when the crystal has the face centering,
four times more atoms than those in primitive cell are generated.

``spg_get_dataset``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Crystallographic information is wrapped in a dataset. The dataset is
accessible through the C-structure as follows:

::

  typedef struct {
    int spacegroup_number;
    char international_symbol[11];
    char hall_symbol[17];
    double transformation_matrix[3][3];
    double origin_shift[3];
    int n_operations;
    int (*rotations)[3][3];
    double (*translations)[3];
    int n_atoms;
    int *wyckoffs;
    int *equivalent_atoms;
  } SpglibDataset;

``transformation_matrix`` (:math:`M`) is the matrix to transform the input lattice
to a Bravais lattice given by

.. math::

   ( \mathbf{a}_\mathrm{B} \; \mathbf{b}_\mathrm{B} \; \mathbf{c}_\mathrm{B} )
   =  ( \mathbf{a} \; \mathbf{b} \; \mathbf{c} ) M

where :math:`\mathbf{a}_\mathrm{B}`, :math:`\mathbf{b}_\mathrm{B}`,
and :math:`\mathbf{c}_\mathrm{B}` are the column vectors of a Bravais
lattice, and :math:`\mathbf{a}`, :math:`\mathbf{b}`, and
:math:`\mathbf{c}` are the column vectors of the input lattice. The
``origin_shift`` is the origin shift in the Bravais lattice. The
symmetry operations of the input cell are stored in ``rotations`` and
``translations``. A space group operation :math:`(R|\tau)` is made
from a set of rotation :math:`R` and translation :math:`\tau` with the
same index. Number of space group operations is found in
``n_operations``. These ``rotations`` and ``translations`` are
generated from database for each hall symbol and can be different from
those obtained by ``spg_get_symmetry`` that doesn't consider if it is
crystallographically correct or not. ``n_atoms`` is the number of
atoms of the input cell. ``wyckoffs`` gives Wyckoff letters that are
assigned to atomic positions of the input cell. The numbers of 0, 1,
2, :math:`\ldots`, correspond to the a, b, c, :math:`\ldots`,
respectively. Number of elements in ``wyckoffs`` is same as
``n_atoms``. The implementation of this Wyckoff position determination
is now testing. Please send e-mail to atz.togo@gmail.com when you find
problems.

The function to obtain the dataset is as follow:

::

  SpglibDataset * spg_get_dataset(const double lattice[3][3],
                                  const double position[][3],
                                  const int types[],
                                  const int num_atom,
                                  const double symprec);


Allocated memory is freed by calling ``spg_free_dataset``.

:: 

  void spg_free_dataset( SpglibDataset *dataset );
  

``spg_get_smallest_lattice``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

::

  int spg_get_smallest_lattice(double smallest_lattice[3][3],
  			       const double lattice[3][3],
			       const double symprec)

Considering periodicity of crystal, one of the possible smallest lattice is
searched. The lattice is stored in ``smallest_lattice``.

``spg_get_multiplicity``
^^^^^^^^^^^^^^^^^^^^^^^^^
  
::

  int spg_get_multiplicity(const double lattice[3][3],
  			   const double position[][3],
  			   const int types[],
			   const int num_atom,
  			   const double symprec);

Return exact number of symmetry operations. This function may be used
in advance to allocate memoery space for symmetry operations. Only
upper bound is required, ``spg_get_max_multiplicity`` can be used
instead of this function and ``spg_get_max_multiplicity`` is faster
than this function.


``spg_get_ir_kpoints``
^^^^^^^^^^^^^^^^^^^^^^^

::

  int spg_get_ir_kpoints(int map[],
                         const double kpoints[][3],
                         const int num_kpoint,
                         const double lattice[3][3],
                         const double position[][3],
			 const int types[],
                         const int num_atom,
                         const int is_time_reversal,
                         const double symprec)

Irreducible k-points are searched from the input k-points
(``kpoints``).  The result is returned as a map of numbers (``map``),
where ``kpoints`` and ``map`` have to have the same number of elements.
The array index of ``map`` corresponds to the reducible k-point numbering.
After finding irreducible k-points, the indices of the irreducible
k-points are mapped to the elements of ``map``, i.e., number of unique
values in ``map`` is the number of the irreducible k-points.
The number of the irreducible k-points is also returned as the return
value.



``spg_get_ir_reciprocal_mesh``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

::

  int spg_get_ir_reciprocal_mesh(int grid_point[][3],
                                 int map[],
                                 const int mesh[3],
				 const int is_shift[3],
                                 const int is_time_reversal,
                                 const double lattice[3][3],
                                 const double position[][3],
				 const int types[],
                                 const int num_atom,
				 const double symprec)

Irreducible reciprocal grid points are searched from uniform mesh grid
points specified by ``mesh`` and ``is_shift``.  ``mesh`` stores three
integers. Reciprocal primitive vectors are divided by the number
stored in ``mesh`` with (0,0,0) point centering. The centering can be
shifted only half of one mesh by setting 1 for each ``is_shift``
element. If 0 is set for ``is_shift``, it means there is no
shift. This limitation of shifting enables the irreducible k-point
search significantly faster when the mesh is very dense.

The reducible uniform grid points are returned in reduced coordinates
as ``grid_point``. A map between reducible and irreducible points are
returned as ``map`` as in the indices of ``grid_point``. The number of
the irreducible k-points are returned as the return value.  The time
reversal symmetry is imposed by setting ``is_time_reversal`` 1.


``spg_get_stabilized_reciprocal_mesh``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**Change in version 1.4**

::

  int spg_get_stabilized_reciprocal_mesh(int grid_point[][3],
                                         int map[],
                                         const int mesh[3],
                                         const int is_shift[3],
                                         const int is_time_reversal,
                                         const int num_rot,
                                         const int rotations[][3][3],
                                         const int num_q,
                                         const double qpoints[][3])

The irreducible k-points are searched from unique k-point mesh grids
from real space lattice vectors and rotation matrices of symmetry
operations in real space with stabilizers. The stabilizers are written
in reduced coordinates. Number of the stabilizers are given by
``num_q``. Reduced k-points are stored in ``map`` as indices of
``grid_point``. The number of the reduced k-points with stabilizers
are returned as the return value.


.. |sflogo| image:: http://sflogo.sourceforge.net/sflogo.php?group_id=161614&type=1
            :target: http://sourceforge.net

|sflogo|
