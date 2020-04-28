.. _definitions_and_conventions:

Definitions and conventions
============================

Information in this page is valid for spglib 1.8.1 or later. The
definitions of transformation matrix and origin shift were different
in the previous versions.

.. contents::
   :depth: 2
   :local:


References
-----------

Some references about crystallographic definitions and conventions are
shown below. Though spglib may not follow them fully, it doesn't mean
spglib doesn't respect them, rather it is due to spglib-author's lack of
understanding the crystallography ashamedly.

* `International Tables for Crystallography <http://it.iucr.org/>`_.
* `Bilbao Crystallographic Server <http://www.cryst.ehu.es/>`_. The
  references of many useful papers are found at
  http://www.cryst.ehu.es/wiki/index.php/Articles.
* Ulrich Müller, "Symmetry Relationships between Crystal Structures"
* E. Parthé, K. Cenzual, and R. E. Gladyshevskii, "Standardization of
  crystal structure data as an aid to the classification of crystal
  structure types", Journal of Alloys and Compounds, **197**, 291-301
  (1993). [`doi2
  <https://dx.doi.org/10.1016/0925-8388(93)90049-S>`_]
* E. Parthé and L. M. Gelato, "The ’best’ unit cell for monoclinic
  structures consistent with b axis unique and cell choice 1
  of international tables for crystallography (1983)", Acta
  Cryst. A **41**, 142-151 (1985) [`doi3
  <https://doi.org/10.1107/S0108767385000289>`_]
* E. Parthé and L. M. Gelato, "The standardization of inorganic
  crystal-structure data", Acta Cryst. A
  **40**, 169-183 (1984) [`doi4
  <https://doi.org/10.1107/S0108767384000416>`_]
* S. Hall, "Space-group notation with an explicit origin", Acta
  Cryst. A **37**, 517-525 (1981) [`doi1
  <https://doi.org/10.1107/S0567739481001228>`_]

Space group operation and change of basis
------------------------------------------

Basis vectors :math:`(\mathbf{a}, \mathbf{b}, \mathbf{c})` or :math:`(\mathbf{a}_1, \mathbf{a}_2, \mathbf{a}_3)`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In spglib, basis vectors are represented by three column vectors:

.. math::
   :label: basis_vectors

   \mathbf{a}= \begin{pmatrix}
   a_x \\
   a_y \\
   a_z \\
   \end{pmatrix},
   \mathbf{b}= \begin{pmatrix}
   b_x \\
   b_y \\
   b_z \\
   \end{pmatrix},
   \mathbf{c}= \begin{pmatrix}
   c_x \\
   c_y \\
   c_z \\
   \end{pmatrix},

in Cartesian coordinates. Depending on the situation,
:math:`(\mathbf{a}_1, \mathbf{a}_2, \mathbf{a}_3)` is used instead of
:math:`(\mathbf{a}, \mathbf{b}, \mathbf{c})`.

Atomic point coordinates :math:`\boldsymbol{x}`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Coordinates of an atomic point :math:`\boldsymbol{x}` are represented
as three fractional values relative to basis vectors as follows,

.. math::
   :label: coordinate_triplet

   \boldsymbol{x}= \begin{pmatrix}
   x_1 \\
   x_2 \\
   x_3 \\
   \end{pmatrix},

where :math:`0 \le x_i < 1`. A position vector :math:`\mathbf{x}` in
Cartesian coordinates is obtained by

.. math::
   :label: position_vector_1

   \mathbf{x} = (\mathbf{a}, \mathbf{b}, \mathbf{c}) \boldsymbol{x}.

or

.. math::
   :label: position_vector_2

   \mathbf{x} = \sum_i x_i \mathbf{a}_i.

Symmetry operation :math:`(\boldsymbol{W}, \boldsymbol{w})`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A symmetry operation consists of a pair of the rotation part
:math:`\boldsymbol{W}` and translation part :math:`\boldsymbol{w}`,
and is represented as :math:`(\boldsymbol{W}, \boldsymbol{w})` in the
spglib document. The symmetry operation transfers :math:`\boldsymbol{x}` to
:math:`\tilde{\boldsymbol{x}}` as follows:

.. math::
   :label: space_group_operation

   \tilde{\boldsymbol{x}} = \boldsymbol{W}\boldsymbol{x} + \boldsymbol{w}.

.. _def_transformation_and_origin_shift:

Transformation matrix :math:`\boldsymbol{P}` and origin shift :math:`\boldsymbol{p}`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The transformation matrix :math:`\boldsymbol{P}` changes choice of
basis vectors as follows

.. math::
   :label: transformation_matrix

   ( \mathbf{a} \; \mathbf{b} \; \mathbf{c} )
   = ( \mathbf{a}_\mathrm{s} \; \mathbf{b}_\mathrm{s} \;
   \mathbf{c}_\mathrm{s} )  \boldsymbol{P},

where :math:`( \mathbf{a} \; \mathbf{b} \; \mathbf{c} )` and :math:`(
\mathbf{a}_\mathrm{s} \; \mathbf{b}_\mathrm{s} \;
\mathbf{c}_\mathrm{s} )` are the basis vectors of an arbitrary system
and of a starndardized system, respectively. In general, the
transformation matrix is not limited for the transformation from the
standardized system, but can be used in between any systems possibly
transformed. It has to be emphasized that the transformation matrix
**doesn't** rotate a crystal in Cartesian coordinates, but just
changes the choices of basis vectors.

The origin shift :math:`\boldsymbol{p}` gives the vector from the
origin of the standardized system :math:`\boldsymbol{O}_\mathrm{s}` to
the origin of the arbitrary system :math:`\boldsymbol{O}`,

.. math::
   :label: origin_shift

   \boldsymbol{p} = \boldsymbol{O} - \boldsymbol{O}_\mathrm{s}.

Origin shift **doesn't** move a crystal in Cartesian coordinates, but
just changes the origin to measure the coordinates of atomic points.


A change of basis is described by the combination of the
transformation matrix and the origin shift denoted by
:math:`(\boldsymbol{P}, \boldsymbol{p})` where first the
transformation matrix is applied and then origin shift. The points in
the standardized system :math:`\boldsymbol{x}_\mathrm{s}` and
arbitrary system :math:`\boldsymbol{x}` are related by

.. math::
   :label: change_of_basis_1

   \boldsymbol{x}_\mathrm{s} = \boldsymbol{P}\boldsymbol{x} +
   \boldsymbol{p},

or equivalently,

.. math::
   :label: change_of_basis_2

   \boldsymbol{x} = \boldsymbol{P}^{-1}\boldsymbol{x}_\mathrm{s} -
   \boldsymbol{P}^{-1}\boldsymbol{p}.


A graphical example is shown below.

.. |cob| image:: change-of-basis.png
         :width: 20%

|cob|

(click the figure to enlarge)

In this example,

.. math::

   \boldsymbol{P} = \begin{pmatrix}
   \frac{1}{2} & \frac{1}{2} & 0 \\
   \frac{\bar{1}}{2} & \frac{1}{2} & 0 \\
   0 & 0 & 1
   \end{pmatrix}.

Difference between rotation and transformation matrices
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A rotation matrix rotates (or mirrors, inverts) the crystal body with
respect to origin. A transformation matrix changes the choice of the
basis vectors, but does not rotate the crystal body.

A space group operation having no translation part sends an atom to
another point by

.. math::

   \tilde{\boldsymbol{x}} = \boldsymbol{W}\boldsymbol{x},

where :math:`\tilde{\boldsymbol{x}}` and :math:`\boldsymbol{x}` are
represented with respect to the same basis vectors :math:`(\mathbf{a},
\mathbf{b}, \mathbf{c})`. Equivalently the rotation is achieved by
rotating the basis vectors:

.. math::
   :label: rotation_matrix

   (\tilde{\mathbf{a}}, \tilde{\mathbf{b}}, \tilde{\mathbf{c}}) =
   (\mathbf{a}, \mathbf{b}, \mathbf{c}) \boldsymbol{W}

with keeping the representation of the atomic point coordinates
:math:`\boldsymbol{x}` because

.. math::

   \tilde{\mathbf{x}} = (\mathbf{a}, \mathbf{b}, \mathbf{c}) \tilde{\boldsymbol{x}}
   = (\mathbf{a}, \mathbf{b}, \mathbf{c}) \boldsymbol{W}
   \boldsymbol{x}
   = (\tilde{\mathbf{a}}, \tilde{\mathbf{b}}, \tilde{\mathbf{c}})
   \boldsymbol{x}.

The transformation matrix changes the choice of the basis vectors as:

.. math::

   (\mathbf{a}', \mathbf{b}', \mathbf{c}') = (\mathbf{a}, \mathbf{b},
   \mathbf{c}) \boldsymbol{P}.

The atomic position vector is not altered by this transformation, i.e.,

.. math::

   (\mathbf{a}', \mathbf{b}', \mathbf{c}') \boldsymbol{x}' =
   (\mathbf{a}, \mathbf{b}, \mathbf{c}) \boldsymbol{x}.

However the representation of the atomic point coordinates changes as follows:

.. math::

   \boldsymbol{P} \boldsymbol{x}' = \boldsymbol{x}.

because

.. math::

   (\mathbf{a}, \mathbf{b}, \mathbf{c}) \boldsymbol{P} \boldsymbol{x}'
   = (\mathbf{a}', \mathbf{b}', \mathbf{c}') \boldsymbol{x}' =
   (\mathbf{a}, \mathbf{b}, \mathbf{c}) \boldsymbol{x}.


.. _def_standardized_unit_cell:

Spglib conventions of standardized unit cell
---------------------------------------------

The standardization in spglib is achieved by :ref:`a change of basis
transformation <def_transformation_and_origin_shift>`. If
:ref:`idealization <def_idealize_cell>` as shown below is further
applied, the crystal can be rigidly rotated in Cartesian
coordinates.

Choice of basis vectors
^^^^^^^^^^^^^^^^^^^^^^^^

Using the APIs ``spg_get_dataset``,
``spg_get_dataset_with_hall_number``, or ``spg_standardize_cell``, the
starndardized unit cell is obtained. The "starndardized unit cell" in
this document means that the (conventional) unit cell structure is
standardized by the crystal symmetry and lengths of basis
vectors. This standardization in spglib is not unique, but upto space
group operations and generators of Euclidean normalizer. Crystals are
categorized by Hall symbols in 530 different types in terms of 230
space group types, unique axes, settings, and cell choices. Moreover
in spglib, lengths of basis vectors are used to choose the order of
:math:`(\mathbf{a}, \mathbf{b}, \mathbf{c})` if the order can not be
determined only by the symmetrical conventions.

.. _def_standardized_primitive_cell:

Transformation to the primitive cell
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In the standardized unit cells, there are five different centring
types available, base centrings of A and C, rhombohedral (R), body centred
(I), and face centred (F). The transformation is applied to the
standardized unit cell by

.. math::
   :label: transformation_to_primitive

   ( \mathbf{a}_\mathrm{p} \; \mathbf{b}_\mathrm{p} \; \mathbf{c}_\mathrm{p} )
   = ( \mathbf{a}_\mathrm{s} \; \mathbf{b}_\mathrm{s} \;
   \mathbf{c}_\mathrm{s} )  \boldsymbol{P}_\mathrm{c},

where :math:`\mathbf{a}_\mathrm{p}`, :math:`\mathbf{b}_\mathrm{p}`,
and :math:`\mathbf{c}_\mathrm{p}` are the basis vectors of the
primitive cell and :math:`\boldsymbol{P}_\mathrm{c}` is the
transformation matrix from the standardized unit cell to the primitive
cell. :math:`\boldsymbol{P}_\mathrm{c}` for centring types are given
as follows:

.. math::

   \boldsymbol{P}_\mathrm{A} =
   \begin{pmatrix}
   1 & 0 & 0 \\
   0 & \frac{1}{2} & \frac{\bar{1}}{2} \\
   0 & \frac{1}{2} & \frac{{1}}{2}
   \end{pmatrix},
   \boldsymbol{P}_\mathrm{C} =
   \begin{pmatrix}
   \frac{1}{2} & \frac{{1}}{2} & 0 \\
   \frac{\bar{1}}{2} & \frac{1}{2} & 0\\
   0 & 0 & 1
   \end{pmatrix},
   \boldsymbol{P}_\mathrm{R} =
   \begin{pmatrix}
   \frac{2}{3} & \frac{\bar{1}}{3} & \frac{\bar{1}}{3} \\
   \frac{1}{3} & \frac{{1}}{3} & \frac{\bar{2}}{3} \\
   \frac{1}{3} & \frac{{1}}{3} & \frac{{1}}{3}
   \end{pmatrix},
   \boldsymbol{P}_\mathrm{I} =
   \begin{pmatrix}
   \frac{\bar{1}}{2} & \frac{{1}}{2} & \frac{{1}}{2} \\
   \frac{{1}}{2} & \frac{\bar{1}}{2} & \frac{{1}}{2} \\
   \frac{{1}}{2} & \frac{{1}}{2} & \frac{\bar{1}}{2}
   \end{pmatrix},
   \boldsymbol{P}_\mathrm{F} =
   \begin{pmatrix}
   0 & \frac{{1}}{2} & \frac{{1}}{2} \\
   \frac{{1}}{2} & 0 & \frac{{1}}{2} \\
   \frac{{1}}{2} & \frac{{1}}{2} & 0
   \end{pmatrix}.

The choice of transformation matrix depends on purposes. These
transformation matrices above are just the spglib choices and may not
be the best.

For rhombohedral lattice systems with the H setting (hexagonal
lattice), :math:`\boldsymbol{P}_\mathrm{R}` is applied to obtain
primitive basis vectors, but for that with the R setting (rhombohedral
lattice), no transformation matrix is applied because it is already
the primitive cell.

.. _def_idealize_cell:

Idealization of unit cell structure
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Spglib allows tolerance parameters to match a slightly distorted unit
cell structure to a space group type with some higher symmetry. Using
obtained symmetry operations, the distortion is removed to idealize
the unit cell structure. The coordinates of atomic points are
idealized using respective site-symmetries (Grosse-Kunstleve *et
al*. (2002)). The basis vectors are idealized by forceing them into
respective lattice shapes as follows. In this treatment, except for
triclinic crystals, crystals can be rotated in Cartesian coordinates,
which is the different type of transformation from that of the
change-of-basis transformation explained above.

Triclinic lattice
""""""""""""""""""

- Niggli reduced cell is used for choosing :math:`\mathbf{a}, \mathbf{b}, \mathbf{c}`.
- :math:`\mathbf{a}` is set along :math:`+x` direction of Cartesian coordinates.
- :math:`\mathbf{b}` is set in :math:`x\text{-}y` plane of Cartesian
  coordinates so that :math:`\mathbf{a}\times\mathbf{b}` is along
  :math:`+z` direction of Cartesian coordinates.

Monoclinic lattice
"""""""""""""""""""

- :math:`b` axis is taken as the unique axis.
- :math:`\alpha = 90^\circ` and :math:`\gamma = 90^\circ`
- :math:`90^\circ < \beta < 120^\circ`.

- :math:`\mathbf{a}` is set along :math:`+x` direction of Cartesian coordinates.
- :math:`\mathbf{b}` is set along :math:`+y` direction of Cartesian coordinates.
- :math:`\mathbf{c}` is set in :math:`x\text{-}z` plane of Cartesian coordinates.

Orthorhombic lattice
"""""""""""""""""""""

- :math:`\alpha = \beta = \gamma = 90^\circ`.

- :math:`\mathbf{a}` is set along :math:`+x` direction of Cartesian coordinates.
- :math:`\mathbf{b}` is set along :math:`+y` direction of Cartesian coordinates.
- :math:`\mathbf{c}` is set along :math:`+z` direction of Cartesian coordinates.

Tetragonal lattice
"""""""""""""""""""

- :math:`\alpha = \beta = \gamma = 90^\circ`.
- :math:`a=b`.

- :math:`\mathbf{a}` is set along :math:`+x` direction of Cartesian coordinates.
- :math:`\mathbf{b}` is set along :math:`+y` direction of Cartesian coordinates.
- :math:`\mathbf{c}` is set along :math:`+z` direction of Cartesian coordinates.

Rhombohedral lattice
"""""""""""""""""""""

- :math:`\alpha = \beta = \gamma`.
- :math:`a=b=c`.

- Let :math:`\mathbf{a}`, :math:`\mathbf{b}`, and :math:`\mathbf{c}`
  projected on :math:`x\text{-}y` plane in Cartesian coordinates be
  :math:`\mathbf{a}_{xy}`, :math:`\mathbf{b}_{xy}`, and
  :math:`\mathbf{c}_{xy}`, respectively, and their angles be
  :math:`\alpha_{xy}`, :math:`\beta_{xy}`,
  :math:`\gamma_{xy}`, respectively.
- Let :math:`\mathbf{a}`, :math:`\mathbf{b}`, and :math:`\mathbf{c}`
  projected along :math:`z`-axis in Cartesian coordinates be
  :math:`\mathbf{a}_{z}`, :math:`\mathbf{b}_{z}`, and
  :math:`\mathbf{c}_{z}`, respectively.

- :math:`\mathbf{a}_{xy}` is set along the ray :math:`30^\circ`
  rotated counter-clockwise from the :math:`+x`
  direction of Cartesian coordinates, and :math:`\mathbf{b}_{xy}` and
  :math:`\mathbf{c}_{xy}` are placed by angles :math:`120^\circ` and
  :math:`240^\circ` from :math:`\mathbf{a}_{xy}` counter-clockwise,
  respectively.
- :math:`\alpha_{xy} = \beta_{xy} = \gamma_{xy} = 120^\circ`.
- :math:`a_{xy} = b_{xy} = c_{xy}`.
- :math:`a_{z} = b_{z} = c_{z}`.


Hexagonal lattice
""""""""""""""""""

- :math:`\alpha = \beta = 90^\circ`.
- :math:`\gamma = 120^\circ`.
- :math:`a=b`.

- :math:`\mathbf{a}` is set along :math:`+x` direction of Cartesian coordinates.
- :math:`\mathbf{b}` is set in :math:`x\text{-}y` plane of Cartesian coordinates.
- :math:`\mathbf{c}` is set along :math:`+z` direction of Cartesian coordinates.

Cubic lattice
""""""""""""""

- :math:`\alpha = \beta = \gamma = 90^\circ`.
- :math:`a=b=c`.

- :math:`\mathbf{a}` is set along :math:`+x` direction of Cartesian coordinates.
- :math:`\mathbf{b}` is set along :math:`+y` direction of Cartesian coordinates.
- :math:`\mathbf{c}` is set along :math:`+z` direction of Cartesian coordinates.

Rotation introduced by idealization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In the idealization step presented above, the input unit cell crystal
strcuture can be rotated in the Cartesian coordinates.  The rotation
matrix :math:`\boldsymbol{R}` of this rotation is defined by

.. math::
   :label: rigid_rotation_matrix

   ( \bar{\mathbf{a}}_\mathrm{s} \;
   \bar{\mathbf{b}}_\mathrm{s} \; \bar{\mathbf{c}}_\mathrm{s} )
   = ( \boldsymbol{R} \mathbf{a}_\mathrm{s} \;
   \boldsymbol{R} \mathbf{b}_\mathrm{s} \; \boldsymbol{R}
   \mathbf{c}_\mathrm{s} ).

This rotation matrix rotates the standardized
crystal structure before idealization :math:`( \mathbf{a}_\mathrm{s}
\; \mathbf{b}_\mathrm{s} \; \mathbf{c}_\mathrm{s} )` to that after
idealization :math:`( \bar{\mathbf{a}}_\mathrm{s} \;
\bar{\mathbf{b}}_\mathrm{s} \; \bar{\mathbf{c}}_\mathrm{s} )` in
Cartesian coordinates of the given input unit cell.

Examples
--------

Crystallographic choice and rigid rotation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The following example of a python script gives a crystal structure of
Br whose space group type is *Cmce*. The basis vectors
:math:`(\mathbf{a}, \mathbf{b}, \mathbf{c})` are fixed by the symmetry
crystal in the standardization. The C-centrng determines the c-axis,
and *m* and *c* operations in *Cmce* fix which directions a- and
b-axes should be with respect to each other axis. This is the first
one choice appearing in the list of Hall symbols among 6 different
choices for this space group type.

::

   import spglib

   # Mind that the a, b, c axes are given in row vectors here,
   # but the formulation above is given for the column vectors.
   lattice = [[7.17851431, 0, 0],  # a
              [0, 3.99943947, 0],  # b
              [0, 0, 8.57154746]]  # c
   points = [[0.0, 0.84688439, 0.1203133],
             [0.0, 0.65311561, 0.6203133],
             [0.0, 0.34688439, 0.3796867],
             [0.0, 0.15311561, 0.8796867],
             [0.5, 0.34688439, 0.1203133],
             [0.5, 0.15311561, 0.6203133],
             [0.5, 0.84688439, 0.3796867],
             [0.5, 0.65311561, 0.8796867]]
   numbers = [35,] * len(points)
   cell = (lattice, points, numbers)
   dataset = spglib.get_symmetry_dataset(cell)
   print("Space group type: %s (%d)"
         % (dataset['international'], dataset['number']))
   print("Transformation matrix:")
   for x in dataset['transformation_matrix']:
       print("  %2d %2d %2d" % tuple(x))
   print("Origin shift: %f %f %f" % tuple(dataset['origin_shift']))

This python script is saved in the file ``example.py``. Then we get

::

   % python example.py
   Space group type: Cmce (64)
   Transformation matrix:
      1  0  0
      0  1  0
      0  0  1
   Origin shift: 0.000000 0.500000 0.500000

No rotation was introduced in the idealization. Next, we swap a- and c-axes.

::

   import spglib

   # Mind that the a, b, c axes are given in row vectors here,
   # but the formulation above is given for the column vectors.
   lattice = [[8.57154746, 0, 0],  # a
              [0, 3.99943947, 0],  # b
              [0, 0, 7.17851431]]  # c
   points = [[0.1203133, 0.84688439, 0.0],
             [0.6203133, 0.65311561, 0.0],
             [0.3796867, 0.34688439, 0.0],
             [0.8796867, 0.15311561, 0.0],
             [0.1203133, 0.34688439, 0.5],
             [0.6203133, 0.15311561, 0.5],
             [0.3796867, 0.84688439, 0.5],
             [0.8796867, 0.65311561, 0.5]]
   numbers = [35,] * len(points)
   cell = (lattice, points, numbers)
   dataset = spglib.get_symmetry_dataset(cell)
   print("Space group type: %s (%d)"
         % (dataset['international'], dataset['number']))
   print("Transformation matrix:")
   for x in dataset['transformation_matrix']:
       print("  %2d %2d %2d" % tuple(x))
   print("Origin shift: %f %f %f" % tuple(dataset['origin_shift']))

By this,

::

   % python spglib-example2.py
   Space group type: Cmce (64)
   Transformation matrix:
      0  0  1
      0  1  0
     -1  0  0
   Origin shift: 0.000000 0.000000 0.000000

We get a non-identity transformation matrix, which wants to transform
back to the original (above) crystal structure by swapping a- and
c-axes. The transformation back of the basis vectors is achieved by
Eq. :eq:`transformation_matrix`. Next, we try to rotate rigidly the
crystal structure by :math:`45^\circ` around c-axis in Cartesian
coordinates from the first one::

   import spglib

   # Mind that the a, b, c axes are given in row vectors here,
   # but the formulation above is given for the column vectors.
   lattice = [[5.0759761474456697, 5.0759761474456697, 0],  # a
              [-2.8280307701821314, 2.8280307701821314, 0],  # b
              [0, 0, 8.57154746]]  # c
   points = [[0.0, 0.84688439, 0.1203133],
             [0.0, 0.65311561, 0.6203133],
             [0.0, 0.34688439, 0.3796867],
             [0.0, 0.15311561, 0.8796867],
             [0.5, 0.34688439, 0.1203133],
             [0.5, 0.15311561, 0.6203133],
             [0.5, 0.84688439, 0.3796867],
             [0.5, 0.65311561, 0.8796867]]
   numbers = [35,] * len(points)
   cell = (lattice, points, numbers)
   dataset = spglib.get_symmetry_dataset(cell)
   print("Space group type: %s (%d)"
         % (dataset['international'], dataset['number']))
   print("Transformation matrix:")
   for x in dataset['transformation_matrix']:
       print("  %2d %2d %2d" % tuple(x))
   print("Origin shift: %f %f %f" % tuple(dataset['origin_shift']))

and

::

   % python spglib-example3.py
   Space group type: Cmce (64)
   Transformation matrix:
      1  0  0
      0  1  0
      0  0  1
   Origin shift: 0.000000 0.000000 0.500000

The transformation matrix is kept unchanged even though the crystal
structure is rotated in Cartesian coordinates. The origin shift is
different but it changes only the order of atoms, so effectively it
does nothing.

Transformation to a primitive cell
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

There are infinite number of choices of primitive cell. The
transformation from a primitive cell basis vectors to the other
primitive cell basis vectors is always done by an integer matrix
because any lattice points can be generated by the linear combination
of the three primitive basis vectors.

When we have a non-primitive cell basis vectors as given in the above
example::

   lattice = [[7.17851431, 0, 0],  # a
              [0, 3.99943947, 0],  # b
              [0, 0, 8.57154746]]  # c

This has the C-centring, so it must be transformed to a primitive
cell. A possible transformation is shown at
:ref:`def_standardized_primitive_cell`, which is
:math:`\boldsymbol{P}_\mathrm{C}`. With the following script::

   import numpy as np
   lattice = [[7.17851431, 0, 0],  # a
              [0, 3.99943947, 0],  # b
              [0, 0, 8.57154746]]  # c
   Pc = [[0.5, 0.5, 0],
         [-0.5, 0.5, 0],
         [0, 0, 1]]
   print(np.dot(np.transpose(lattice), Pc).T)  # given in row vectors

we get the primitive cell basis vectors (shown in row vectors)::

   [[ 3.58925715 -1.99971973  0.        ]
    [ 3.58925715  1.99971973  0.        ]
    [ 0.          0.          8.57154746]]

``find_primitive`` gives a primitive cell that is obtained by
transforming standardized and idealized crystal structure to the
primitive cell using the transformation matrix. Therefore by this
script::

   import spglib

   lattice = [[7.17851431, 0, 0],
              [0, 3.99943947, 0],
              [0, 0, 8.57154746]]
   points = [[0.0, 0.84688439, 0.1203133],
             [0.0, 0.65311561, 0.6203133],
             [0.0, 0.34688439, 0.3796867],
             [0.0, 0.15311561, 0.8796867],
             [0.5, 0.34688439, 0.1203133],
             [0.5, 0.15311561, 0.6203133],
             [0.5, 0.84688439, 0.3796867],
             [0.5, 0.65311561, 0.8796867]]
   numbers = [8,] * len(points)
   cell = (lattice, points, numbers)

   primitive_cell = spglib.find_primitive(cell)
   print(primitive_cell[0])

we get::

   [[ 3.58925715 -1.99971973  0.        ]
    [ 3.58925715  1.99971973  0.        ]
    [ 0.          0.          8.57154746]]

This is same as what we manually obtained above.
Even when the basis vectors are rigidly rotated as::

   lattice = [[5.0759761474456697, 5.0759761474456697, 0],
              [-2.8280307701821314, 2.8280307701821314, 0],
              [0, 0, 8.57154746]]

the relationship of a, b, c axes is unchanged. Therefore the same
transformation matrix to the primitive cell can be used. Then we get::

   [[3.95200346 1.12397269 0.        ]
    [1.12397269 3.95200346 0.        ]
    [0.         0.         8.57154746]]

However applying ``find_primitive`` rigidly rotates automatically and
so the following script doesn't give this basis vectors::

   import spglib

   lattice = [[5.0759761474456697, 5.0759761474456697, 0],
              [-2.8280307701821314, 2.8280307701821314, 0],
              [0, 0, 8.57154746]]
   points = [[0.0, 0.84688439, 0.1203133],
             [0.0, 0.65311561, 0.6203133],
             [0.0, 0.34688439, 0.3796867],
             [0.0, 0.15311561, 0.8796867],
             [0.5, 0.34688439, 0.1203133],
             [0.5, 0.15311561, 0.6203133],
             [0.5, 0.84688439, 0.3796867],
             [0.5, 0.65311561, 0.8796867]]
   numbers = [8,] * len(points)
   cell = (lattice, points, numbers)

   primitive_cell = spglib.find_primitive(cell)
   print(primitive_cell[0])

but gives those with respect to the idealized ones::

   [[ 3.58925715 -1.99971973  0.        ]
    [ 3.58925715  1.99971973  0.        ]
    [ 0.          0.          8.57154746]]

To obtain the rotated primitive cell basis vectors, we can use
``standardize_cell`` as shown below::

   import spglib

   lattice = [[5.0759761474456697, 5.0759761474456697, 0],
              [-2.8280307701821314, 2.8280307701821314, 0],
              [0, 0, 8.57154746]]
   points = [[0.0, 0.84688439, 0.1203133],
             [0.0, 0.65311561, 0.6203133],
             [0.0, 0.34688439, 0.3796867],
             [0.0, 0.15311561, 0.8796867],
             [0.5, 0.34688439, 0.1203133],
             [0.5, 0.15311561, 0.6203133],
             [0.5, 0.84688439, 0.3796867],
             [0.5, 0.65311561, 0.8796867]]
   numbers = [8,] * len(points)
   cell = (lattice, points, numbers)
   primitive_cell = spglib.standardize_cell(cell, to_primitive=1, no_idealize=1)
   print(primitive_cell[0])

then we get::

   [[3.95200346 1.12397269 0.        ]
    [1.12397269 3.95200346 0.        ]
    [0.         0.         8.57154746]]

which is equivalent to that we get manually. However using
``standardize_cell``, distortion is not removed for the distorted
crystal structure.

Computing rigid rotation introduced by idealization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In spglib, rigid rotation is purposely introduced in the idealization
step though this is unlikely as a crystallographic operation.

The crystal structure in the following script is the same as shown
above, which is the one :math:`45^\circ` rotated around c-axis::

    import spglib

    # Mind that the a, b, c axes are given in row vectors here,
    # but the formulation above is given for the column vectors.
    lattice = [[5.0759761474456697, 5.0759761474456697, 0],  # a
               [-2.8280307701821314, 2.8280307701821314, 0],  # b
               [0, 0, 8.57154746]]  # c
    points = [[0.0, 0.84688439, 0.1203133],
              [0.0, 0.65311561, 0.6203133],
              [0.0, 0.34688439, 0.3796867],
              [0.0, 0.15311561, 0.8796867],
              [0.5, 0.34688439, 0.1203133],
              [0.5, 0.15311561, 0.6203133],
              [0.5, 0.84688439, 0.3796867],
              [0.5, 0.65311561, 0.8796867]]
    numbers = [35,] * len(points)
    cell = (lattice, points, numbers)
    dataset = spglib.get_symmetry_dataset(cell)
    print("Space group type: %s (%d)"
          % (dataset['international'], dataset['number']))
    print("Transformation matrix:")
    for x in dataset['transformation_matrix']:
        print("  %2d %2d %2d" % tuple(x))
    print("std_lattice_after_idealization:")
    print(dataset['std_lattice'])

we get

::

   Space group type: Cmce (64)
   Transformation matrix:
      1  0  0
      0  1  0
      0  0  1
   std_lattice_after_idealization:
   [[7.17851431 0.         0.        ]
    [0.         3.99943947 0.        ]
    [0.         0.         8.57154746]]

From Eq. :eq:`transformation_matrix`, the standardized basis vectors
**before** idealization :math:`( \mathbf{a}_\mathrm{s} \; \mathbf{b}_\mathrm{s}
\; \mathbf{c}_\mathrm{s} )` is obtained by (after ``import numpy as np``)

::

   std_lattice_before_idealization = np.dot(
       np.transpose(lattice),
       np.linalg.inv(dataset['transformation_matrix'])).T
   print(std_lattice_before_idealization)

which is (in row vectors)

::

   [[ 5.07597615  5.07597615  0.        ]
    [-2.82803077  2.82803077  0.        ]
    [ 0.          0.          8.57154746]]

This is different from the standardized basis vectors **after**
idealization :math:`( \bar{\mathbf{a}}_\mathrm{s} \;
\bar{\mathbf{b}}_\mathrm{s} \; \bar{\mathbf{c}}_\mathrm{s} )`.  Unless
this crystal strucutre is distorted from the crystal structure that
has the ideal symmetry, this means that the crystal was rotated
rigidly in the idealization step by

.. math::

   ( \bar{\mathbf{a}}_\mathrm{s} \;
   \bar{\mathbf{b}}_\mathrm{s} \; \bar{\mathbf{c}}_\mathrm{s} )
   = ( \boldsymbol{R} \mathbf{a}_\mathrm{s} \;
   \boldsymbol{R} \mathbf{b}_\mathrm{s} \; \boldsymbol{R}
   \mathbf{c}_\mathrm{s} ).

where :math:`\boldsymbol{R}` is the rotation
matrix. This is computed by

::

   R = np.dot(dataset['std_lattice'].T,
              np.linalg.inv(std_lattice_before_idealization.T))
   print(R)

and we get

::

   [[ 0.70710678  0.70710678  0.        ]
    [-0.70710678  0.70710678  0.        ]
    [ 0.          0.          1.        ]]

This equals to

.. math::

   \begin{pmatrix}
   \cos\theta & -\sin\theta & 0 \\
   \sin\theta & \cos\theta & 0 \\
   0 & 0 & 1
   \end{pmatrix},

with :math:`\theta = -\pi/4` and :math:`\det(\boldsymbol{R})=1` when
no distortion. ``dataset['std_rotation_matrix'])`` gives
approximately the same result::

   [[ 0.70710678  0.70710678  0.        ]
    [-0.70710678  0.70710678  0.        ]
    [ 0.          0.          1.        ]]

In summary,

.. math::


   ( \bar{\mathbf{a}}_\mathrm{s} \;
   \bar{\mathbf{b}}_\mathrm{s} \; \bar{\mathbf{c}}_\mathrm{s} )  \boldsymbol{P}
   = ( \boldsymbol{R} \mathbf{a} \; \boldsymbol{R} \mathbf{b} \;
   \boldsymbol{R} \mathbf{c} ).

The atomic point coordinates in :math:`( \bar{\mathbf{a}}_\mathrm{s}
\; \bar{\mathbf{b}}_\mathrm{s} \; \bar{\mathbf{c}}_\mathrm{s} )`
are simply obtained by Eq. :eq:`change_of_basis_1` since the
rotation doesn't affect them.
