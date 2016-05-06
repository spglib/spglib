Definitions and conventions
============================

Information in this page is valid for spglib 1.8.1 or later. The
definitions of transformation matrix and origin shift were different
in the previous versions.

.. contents::
   :depth: 2
   :local:

Basis vectors :math:`(\mathbf{a}, \mathbf{b}, \mathbf{c})` or :math:`(\mathbf{a}_1, \mathbf{a}_2, \mathbf{a}_3)`
------------------------------------------------------------------------------------------------------------------

In spglib, basis vectors are represented by three column vectors:

.. math::

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
-----------------------------------------------

Coordinates of an atomic point :math:`\boldsymbol{x}` are represented
as three fractional values relative to basis vectors as follows,

.. math::

   \boldsymbol{x}= \begin{pmatrix}
   x_1 \\
   x_2 \\
   x_3 \\
   \end{pmatrix},

where :math:`0 \le x_i < 1`. A position vector :math:`\mathbf{x}` in
Cartesian coordinates is obtained by

.. math::

   \mathbf{x} = (\mathbf{a}, \mathbf{b}, \mathbf{c}) \boldsymbol{x}.

or 

.. math::

   \mathbf{x} = \sum_i x_i \mathbf{a}_i.

Symmetry operation :math:`(\boldsymbol{W}, \boldsymbol{w})`
-----------------------------------------------------------

A symmetry operation consists of a pair of the rotation part
:math:`\boldsymbol{W}` and translation part :math:`\boldsymbol{w}`,
and is represented as :math:`(\boldsymbol{W}, \boldsymbol{w})` in the
spglib document. The symmetry operation transfers :math:`\boldsymbol{x}` to
:math:`\tilde{\boldsymbol{x}}` as follows:

.. math::

  \tilde{\boldsymbol{x}} = \boldsymbol{W}\boldsymbol{x} + \boldsymbol{w}.

Transformation matrix :math:`\boldsymbol{P}` and origin shift :math:`\boldsymbol{p}`
-------------------------------------------------------------------------------------

The transformation matrix :math:`\boldsymbol{P}` changes choice of
basis vectors as follows

.. math::

   ( \mathbf{a} \; \mathbf{b} \; \mathbf{c} )
   = ( \mathbf{a}_\mathrm{s} \; \mathbf{b}_\mathrm{s} \;
   \mathbf{c}_\mathrm{s} )  \boldsymbol{P},

where :math:`( \mathbf{a} \; \mathbf{b} \; \mathbf{c} )` and :math:`(
\mathbf{a}_\mathrm{s} \; \mathbf{b}_\mathrm{s} \;
\mathbf{c}_\mathrm{s} )` are the basis vectors of an arbitrary system
and of a starndardized system, respectively. Transformation matrix
**doesn't** rotate a crystal in Cartesian coordinates, but just
changes the choices of basis vectors.

The origin shift :math:`\boldsymbol{p}` gives the vector from the
origin of the standardized system :math:`\boldsymbol{O}_\mathrm{s}` to
the origin of the arbitrary system :math:`\boldsymbol{O}`,

.. math::

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

  \boldsymbol{x}_\mathrm{s} = \boldsymbol{P}\boldsymbol{x} +
  \boldsymbol{p},

or equivalently,

.. math::

  \boldsymbol{x} = \boldsymbol{P}^{-1}\boldsymbol{x}_\mathrm{s} -
  \boldsymbol{P}^{-1}\boldsymbol{p}.
  

A graphical example is shown below.

.. |cob| image:: change-of-basis.png
         :width: 20%

|cob|

(click the figure to enlarge)

In this example,

.. math::

   \renewcommand*{\arraystretch}{1.4}
   \boldsymbol{P} = \begin{pmatrix}
   \frac{1}{2} & \frac{1}{2} & 0 \\
   \frac{\bar{1}}{2} & \frac{1}{2} & 0 \\
   0 & 0 & 1 
   \end{pmatrix}.


.. _def_standardized_unit_cell:

Conventions of standardized unit cell
--------------------------------------

Choice of basis vectors
^^^^^^^^^^^^^^^^^^^^^^^^

Using the APIs ``spg_get_dataset``,
``spg_get_dataset_with_hall_number``, or ``spg_standardize_cell``, the
starndardized unit cell is obtained. The "starndardized unit cell" in
this document means that the (conventional) unit cell structure is
standardized by the crystal symmetry and lengths of basis vectors.
Crystals are categorized by Hall symbols in 530 different types in
terms of 230 space group types, unique axes, settings, and cell
choices. Moreover in spglib, lengths of basis vectors are used to
choose the order of :math:`(\mathbf{a}, \mathbf{b}, \mathbf{c})` if
the order can not be determined only by the symmetrical conventions. The
details of these conventions are summarized in the article found at
http://arxiv.org/abs/1506.01455.

.. _def_standardized_primitive_cell:

Transformation to the primitive cell
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In the standardized unit cells, there are five different centring
types available, base centrings of A and C, rhombohedral (R), body centred
(I), and face centred (F). The transformation is applied to the
standardized unit cell by

.. math::

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

   \renewcommand*{\arraystretch}{1.4}
   \boldsymbol{P}_\mathrm{A} = 
   \begin{pmatrix}
   1 & 0 & 0 \\
   0 & \frac{1}{2} & \frac{\bar{1}}{2} \\
   0 & \frac{1}{2} & \frac{{1}}{2}
   \end{pmatrix},
   \renewcommand*{\arraystretch}{1.4}
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

For rhombohedral lattice systems with the choice of hexagonal axes,
:math:`\boldsymbol{P}_\mathrm{R}` is applied.

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

- :math:`\mathbf{a}_{xy}` is set along :math:`+x` direction of Cartesian
  coordinates, and :math:`\mathbf{b}_{xy}` and :math:`\mathbf{c}_{xy}`
  are placed by angles :math:`120^\circ` and :math:`240^\circ` from
  :math:`\mathbf{a}_{xy}` counter-clockwise, respectively.
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
