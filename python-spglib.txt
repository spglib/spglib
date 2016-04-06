.. _python_spglib:

Spglib for python
==================

This is written to work with Atomistic Simulation Environment (ASE)
Atoms class object. An alternative Atoms class (`atoms.py
<https://github.com/atztogo/spglib/blob/master/python/examples/atoms.py>`_)
that contains minimum
set of methods is prepared in the `examples
<https://github.com/atztogo/spglib/tree/master/python/examples>`_ directory.

|

Installation
-------------

Full source codes, examples, and the test are downloaded `here
<https://sourceforge.net/project/showfiles.php?group_id=215020>`_.

Using package distribution service
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The easiest way to install python-spglib is to use the pypi package,
for which numpy is required to be installed before the
installation. A command to install spglib is::

   % pip install spglib

Previous versions < 1.8.x are installed by::

   % pip install pyspglib

Conda is another choice::

   % conda install -c https://conda.anaconda.org/jochym spglib

These packages are made by Paweł T. Jochym.

Building from setup.py
^^^^^^^^^^^^^^^^^^^^^^^

To manually install spglib using ``setup.py``, python header files
(python-dev), gcc, and numpy are required before the build. The
installation steps are shown as follows:

1. Go to the :file:`python` directory
2. Type the command::

      % python setup.py install --home=<my-directory>

   The :file:`{<my-directory>}` is possibly current directory, :file:`.`
   or maybe::

      % python setup.py install --user

   to use the user scheme (see the python document.)

3. Put :file:`./lib/python` path into :envvar:`$PYTHONPATH`, e.g., in your
   .washrag.

|

Test
-----

The test script :file:`test_spglib.py` is found in :file:`python/test`
directory. Got to this directory and run this script. It will be like below::

   % python test_spglib.py
   test_get_symmetry_dataset (__main__.TestSpglib) ... ok
   
   ----------------------------------------------------------------------
   Ran 1 test in 2.132s
   
   OK

|

How to import spglib module
---------------------------

**Change in version 1.9.0!**

For versions 1.9.x or later::

   import spglib     

For versions 1.8.x or before::

   from pyspglib import spglib

If the version is not sure::

   try:
       import spglib as spg
   except ImportError:
       from pyspglib import spglib as spg   

|

Version number
--------------

In version 1.8.3 or later, the version number is obtained by
``spglib.__version__`` or :ref:`method_get_version`.

|

Example
--------

Examples are found in `examples
<https://github.com/atztogo/spglib/tree/master/python/examples>`_
directory. In this directory, an alternative Atoms class in `atoms.py
<https://github.com/atztogo/spglib/blob/master/python/examples/atoms.py>`_
can be used instead of the ``Atoms`` class in ASE for spglib.

|

Variables
----------

Cristal structure (``atoms``)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Crsytal structure information is given either in an **Atoms object**
or a **tuple**, where the tuple is supported at version 1.9.1 or
later. In the case given by a tuple, it has to follow the format as
written below.

The elements of a tuple are ``atoms = (lattice, positions, numbers,
magmoms)`` where ``magmoms`` is optional, three elements of the tuple
are accepted.

Lattice parameters ``lattice`` are given by a 3x3 matrix with floating
point values. Fractional atomic positions ``positions`` are given by a
Nx3 matrix with floating point values, where N is the number of
atoms. Numbers to distinguish atomic species ``numbers`` are given
by a list of N integers. Collinear magnetic moments ``magmoms`` are
given by a list of N floating point values.

::

   lattice = [[a_x, a_y, a_z],
              [b_x, b_y, b_z],
              [c_x, c_y, c_z]]
   positions = [[a_1, b_1, c_1],
                [a_2, b_2, c_2],
                [a_3, b_3, c_3],
                ...]
   numbers = [n_1, n_2, n_3, ...]
   magmoms = [m_1, m_2, m_3, ...]

Symmetry tolerance (``symprec``)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Distance tolerance in Cartesian coordinates to find crystal symmetry.

|

Methods
--------

.. _method_get_version:

``get_version``
^^^^^^^^^^^^^^^^

**New in version 1.8.3**

::

    version = get_version()

This returns version number of spglib by tuple with three numbers.

|

``get_spacegroup``
^^^^^^^^^^^^^^^^^^^

::

    spacegroup = get_spacegroup(atoms, symprec=1e-5)

International space group symbol and the number are obtained as a
string.

|

``get_symmetry``
^^^^^^^^^^^^^^^^^^

::

    symmetry = get_symmetry(atoms, symprec=1e-5)

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

|

``refine_cell``
^^^^^^^^^^^^^^^^

**Behaviour changed in version 1.8.x**

::

    lattice, scaled_positions, numbers = refine_cell(atoms, symprec=1e-5)

Bravais lattice (3x3 numpy array), atomic scaled positions (a numpy
array of [number_of_atoms,3]), and atomic numbers (a 1D numpy array)
that are symmetrized following space group type are returned.

The detailed control of standardization of unit cell may be done using
``standardize_cell``.

|

``find_primitive``
^^^^^^^^^^^^^^^^^^^

**Behaviour changed in version 1.8.x**

::

   lattice, scaled_positions, numbers = find_primitive(atoms, symprec=1e-5)

When a primitive cell is found, lattice parameters (3x3 numpy array),
scaled positions (a numpy array of [number_of_atoms,3]), and atomic
numbers (a 1D numpy array) is returned. When no primitive cell is
found, (``None``, ``None``, ``None``) is returned.

The detailed control of standardization of unit cell can be done using
``standardize_cell``.

|

``standardize_cell``
^^^^^^^^^^^^^^^^^^^^^

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

|

``get_symmetry_dataset``
^^^^^^^^^^^^^^^^^^^^^^^^^^

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

|

``get_symmetry_from_database``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

::

   get_symmetry_from_database(hall_number):

A set of crystallographic symmetry operations corresponding to
``hall_number`` is returned by a dictionary where rotation parts and
translation parts are accessed by the keys ``rotations`` and
``translations``, respectively. The definition of ``hall_number`` is
found at :ref:`api_spg_get_dataset_spacegroup_type`.

|

``get_ir_reciprocal_mesh``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

::

   mapping, grid = get_ir_reciprocal_mesh(mesh, atoms, is_shift=[0, 0, 0])

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

   np.unique(mapping)

Here ``np`` is the imported numpy module. The grid point is accessed
by ``grid[index]``.

For example, the irreducible k-points in fractional coordinates are
obtained by ::

   ir_grid = []
   mapping, grid = get_ir_reciprocal_mesh([ 8, 8, 8 ], atoms, [1, 1, 1])
   for i in np.unique(mapping):
       ir_grid.append(grid[i])
