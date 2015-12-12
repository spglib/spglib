.. _pyspglib:

Spglib for python
==================

Pyspglib is the python module for spglib. This is written to work with
Atomistic Simulation Environment (ASE) Atoms class object. An
alternative Atoms class that contains minimum set of methods is
prepared in the ``examples`` directory.

How to build spglib python module
=================================

The C sources of spglib and interface for the python C/API are
compiled. The development environments for python and gcc are required
before starting to build.

1. Go to the :file:`pyspglib` directory
2. Type the command::

      % python setup.py install --home=<my-directory>

   The :file:`{<my-directory>}` is possibly current directory, :file:`.`
   or maybe::

      % python setup.py install --user

   to use the user scheme (see the python document.)

3. Put ``lib/python`` path into :envvar:`$PYTHONPATH`, e.g., in your .bashrc.

How to use it
=============

1. Import spglib::

      from pyspglib import spglib

2. Call the methods with ASE Atoms object.

In version 1.8.3 or later, the spglib version is obtained as
``__version__`` in ``pyspglib/__init__.py``, i.e., the version number
is known by::

   from pyspglib import __version__
   print(__version__)

Example
========

Examples are found in ``examples`` directory. Instead using ASE's
``Atoms`` class, an alternative ``Atoms`` class in ``atoms.py`` in
this directory can be used with ``pyspglib``. To use this ``atoms.py``::

   from atoms import Atoms

Methods
=======

The tolerance is given in Cartesian coordinates.

``get_version``
----------------

**New in version 1.8.3**

::

    version = get_version()

This returns version number of spglib by tuple with three numbers.

``get_spacegroup``
------------------
::

    spacegroup = get_spacegroup(atoms, symprec=1e-5)

``atoms`` is the object of ASE Atoms class. ``symprec`` is the float
variable, which is used as tolerance in symmetry search.

International space group symbol and the number are obtained as a string.

``get_symmetry``
----------------
::

    symmetry = get_symmetry(atoms, symprec=1e-5)

``atoms`` is the object of ASE Atoms class. ``symprec`` is the float
variable, which is used as tolerance in symmetry search.

Symmetry operations are obtained as a dictionary. The key ``rotation``
contains a numpy array of integer, which is "number of symmetry
operations" x "3x3 matrices". The key ``translation`` contains a numpy
array of float, which is "number of symmetry operations" x
"vectors". The orders of the rotation matrices and the translation
vectors correspond with each other, e.g. , the second symmetry
operation is organized by the second rotation matrix and second
translation vector in the respective arrays. The operations are
applied for the fractional coordinates (not for Cartesian
coordinates).

The rotation matrix and translation vector are used as follows::

    new_vector[3x1] = rotation[3x3] * vector[3x1] + translation[3x1]

The three values in the vector are given for the a, b, and c axes,
respectively.

``refine_cell``
---------------

**Behaviour changed in version 1.8.x**

::

    lattice, scaled_positions, numbers = refine_cell(atoms, symprec=1e-5)

``atoms`` is the object of ASE Atoms class. ``symprec`` is the float
variable, which is used as tolerance in symmetry search. 

Bravais lattice (3x3 numpy array), atomic scaled positions (a numpy
array of [number_of_atoms,3]), and atomic numbers (a 1D numpy array)
that are symmetrized following space group type are returned.

The detailed control of standardization of unit cell may be done using
``standardize_cell``.

``find_primitive``
------------------

**Behaviour changed in version 1.8.x**

::

   lattice, scaled_positions, numbers = find_primitive(atoms, symprec=1e-5)

``atoms`` is the object of ASE Atoms class. ``symprec`` is the float
variable, which is used as tolerance in symmetry search.

When a primitive cell is found, lattice parameters (3x3 numpy array),
scaled positions (a numpy array of [number_of_atoms,3]), and atomic
numbers (a 1D numpy array) is returned. When no primitive cell is
found, (``None``, ``None``, ``None``) is returned.

The detailed control of standardization of unit cell can be done using
``standardize_cell``.

``standardize_cell``
---------------------

**New in version 1.8.x**

::

   lattice, scaled_positions, numbers = \\
          standardize_cell(bulk, to_primitive=0, no_idealize=0, symprec=1e-5)

``to_primitive=1`` is used to create the standardized primitive cell,
and ``no_idealize=1`` disables to idealize lengths and angles of basis
vectors and positions of atoms according to crystal symmetry. Now
``refine_cell`` and ``find_primitive`` are shorthands of this method
with combinations of these options. More detailed explanation is
shown in the spglib (C-API) document.

``get_symmetry_dataset``
----------------------------
::

    dataset = get_symmetry_dataset(atoms, symprec=1e-5)

``dataset`` is a dictionary. The keys are:

* ``number``: International space group number
* ``international``: International symbol
* ``hall``: Hall symbol
* ``transformation_matrix``: Transformation matrix from lattice of input cell to Bravais lattice :math:`L^{bravais} = L^{original} * T`
* ``origin shift``: Origin shift in the setting of Bravais lattice
* ``wyckoffs``: Wyckoff letters
* ``equivalent_atoms``: Mapping table to equivalent atoms
* ``rotations`` and ``translations``: Rotation matrices and translation vectors. Space group operations are obtained by::

    [(r, t) for r, t in zip(dataset['rotations'], dataset['translations'])]


``get_ir_reciprocal_mesh``
--------------------------

::

   mapping, grid = get_ir_reciprocal_mesh(mesh, atoms, is_shift=[0,0,0])

Irreducible k-points are obtained from a sampling mesh of k-points.
``mesh`` is given by three integers by array and specifies mesh
numbers along reciprocal primitive axis. ``atoms`` is an Atoms object
of ASE. ``is_shift`` is given by the three integers by array. When
``is_shift`` is set for each reciprocal primitive axis, the mesh is
shifted along the axis in half of adjacent mesh points irrespective of
the mesh numbers. When the value is not 0, ``is_shift`` is set.

``mapping`` and ``grid`` are returned. ``grid`` gives the mesh points in
fractional coordinates in reciprocal space. ``mapping`` gives mapping to
the irreducible k-point indices that are obtained by ::

   np.unique( mapping )

Here ``np`` is the imported numpy module. The grid point is accessed
by ``grid[ index ]``.

For example, the irreducible k-points in fractional coordinates are
obtained by ::

   ir_grid = []
   mapping, grid = get_ir_reciprocal_mesh( [ 8, 8, 8 ], atoms, [ 1, 1, 1 ] )
   for i in np.unique( mapping ):
     ir_grid.append( grid[ i ] )


.. |sflogo| image:: http://sflogo.sourceforge.net/sflogo.php?group_id=161614&type=1
            :target: http://sourceforge.net



|sflogo|
