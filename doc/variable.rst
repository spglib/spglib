Variables
==========

Variables frequently used in C-API are explained. In interfaces of
other languages, the definition may be different, e.g., ``lattice``
for Python interface is transposed.

.. contents::
   :depth: 2
   :local:

.. _variables_lattice:

``lattice``
-------------

**Attention**: In Python interface, ``lattice`` is transposed
(:ref:`py_variables_crystal_structure`.)

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


``position``
--------------

Atomic points (in fractional coordinates with respect to basis vectors)

::

  [ [ x1_a, x1_b, x1_c ],
    [ x2_a, x2_b, x2_c ],
    [ x3_a, x3_b, x3_c ],
    ...                   ]


``types``
----------

Atomic species are differenciated by integers. The nubmers are just
used to distinguishes atoms, and is not releated to atomic numbers.

::

  [ type_1, type_2, type_3, ... ]

The number of elements is same as the number of atoms.

``rotation``
--------------

Rotation matricies (ingeger) of symmetry operations.

::

    [ [ r_aa, r_ab, r_ac ],
      [ r_ba, r_bb, r_bc ],
      [ r_ca, r_cb, r_cc ] ]

``translation``
-----------------

Translation vectors corresponding to symmetry operations in fractional
coordinates.

::

    [ t_a, t_b, t_c ]

.. _variables_symprec:

``symprec``
------------

Distance tolerance in Cartesian coordinates to find crystal
symmetry.

For atomic positions, roughly speaking, two position vectors `x` and
`x'` in Cartesian coordinates are considered to be the same if ``|x' -
x| < symprec``. For more details, see the `spglib paper
<https://arxiv.org/abs/1808.01590>`_, Sec. II-A.

The angle distortion between basis vectors is converted to a length
and compared with this distance tolerance. For more details, see the
`spglib paper <https://arxiv.org/abs/1808.01590>`_, Sec. IV-A. It is
possible to specify angle tolerance explicitly, see
:ref:`variables_angle_tolerance`.

.. _variables_angle_tolerance:

``angle_tolerance``
--------------------

Tolerance of angle between basis vectors in degrees to be tolerated
in the symmetry finding. To use angle tolerance, another set of
functions are prepared for C-API as follows

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
of ``const double angle_tolerance`` in degrees. By specifying a
negative value, the behavior becomes the same as usual functions. The
default value of ``angle_tolerance`` is a negative value. For
python-API, the angle tolernce is given with a keyward argument of
`angle_tolerance`.
