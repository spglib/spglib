Definitions and conventions used in spglib
===========================================

Information in this page is valid for spglib 1.8.1 or later. The
definitions of transformation matrix and origin shift were different
in the previous versions.

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
and for a starndardized system, respectively. Transformation matrix
**doesn't** rotate a crystal in Cartesian coordinates, but leaves the
crystal fixed and just changes the choices of basis vectors.

The origin shift :math:`\boldsymbol{p}` gives the vector from the
origin of the standardized system :math:`\boldsymbol{O}_\mathrm{s}` to
the origin of the arbitrary system :math:`\boldsymbol{O}`,

.. math::

   \boldsymbol{p} = \boldsymbol{O} - \boldsymbol{O}_\mathrm{s}.

Origin shift **doesn't** move a crystal in Cartesian coordinates, but
leaves the crystal fixed and just changes the origin to measure the
coordinates of atomic points.

   
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

.. math::

   \renewcommand*{\arraystretch}{1.4}
   \boldsymbol{P}_\mathrm{s} = \begin{pmatrix}
   \frac{1}{2} & \frac{1}{2} & 0 \\
   \frac{\bar{1}}{2} & \frac{1}{2} & 0 \\
   0 & 0 & 1 
   \end{pmatrix}


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

Idealization of unit cell structure
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Spglib allows tolerance parameters to match a slightly distorted unit
cell structure to a space group type with some higher symmetry. Using
obtained symmetry operations, the distortion is removed. The
coordinates of atomic points are idealized using respective
site-symmetries (Grosse-Kunstleve *et al*. (2002)). The basis vectors
are idealized to cast into the corresponding Bravais lattice. This is
explained in more detail as follows.

Orientation in Cartesian coordinates


Transformation to the primitive cell
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

::

   static double A_mat[3][3] = {{    1,    0,    0},
   			     {    0, 1./2,-1./2},
   			     {    0, 1./2, 1./2}};
   static double C_mat[3][3] = {{ 1./2, 1./2,    0},
   			     {-1./2, 1./2,    0},
   			     {    0,    0,    1}};
   static double R_mat[3][3] = {{ 2./3,-1./3,-1./3 },
   			     { 1./3, 1./3,-2./3 },
   			     { 1./3, 1./3, 1./3 }};
   static double I_mat[3][3] = {{-1./2, 1./2, 1./2 },
   			     { 1./2,-1./2, 1./2 },
   			     { 1./2, 1./2,-1./2 }};
   static double F_mat[3][3] = {{    0, 1./2, 1./2 },
   			     { 1./2,    0, 1./2 },
   			     { 1./2, 1./2,    0 }};





.. |sflogo| image:: http://sflogo.sourceforge.net/sflogo.php?group_id=161614&type=1
            :target: http://sourceforge.net

|sflogo|
