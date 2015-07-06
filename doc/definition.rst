Definitions and conventions used in spglib
===========================================

Definition of symmetry operation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Basis vectors :math:`(\mathbf{a}, \mathbf{b}, \mathbf{c})`
------------------------------------------------------------

Basis vectors are represented by three column vectors in Cartesian
coordinates in spglib.

Atomic point coordinate :math:`\boldsymbol{x}`
-----------------------------------------------

Coordinates of an atomic point are given as fractions relative to
basis vectors and represented by a column vector with three elements
corresponding to :math:`\mathbf{a}`, :math:`\mathbf{b}`, and
:math:`\mathbf{c}`, respectively. A position vector
:math:`\mathbf{x}` in Cartesian coodinates is obtained by

.. math::

   \mathbf{x} = (\mathbf{a}, \mathbf{b}, \mathbf{c}) \boldsymbol{x}.

Symmetry operation :math:`(\boldsymbol{W}, \boldsymbol{w})`
-----------------------------------------------------------

A symmetry operation consists of a pair of the rotation part
:math:`\boldsymbol{W}` and translation part :math:`\boldsymbol{w}`,
and is represented as :math:`(\boldsymbol{W}, \boldsymbol{w})` in the
spglib document. The symmetry operation transfers :math:`\boldsymbol{x}` to
:math:`\tilde{\boldsymbol{x}}` as follows:

.. math::

  \tilde{\boldsymbol{x}} = \boldsymbol{W}\cdot\boldsymbol{x} + \boldsymbol{w}.

Transformation matrix :math:`\boldsymbol{P}` and origin shift :math:`\boldsymbol{p}`
-------------------------------------------------------------------------------------

.. |cob| image:: change-of-basis.png
         :width: 25%

|cob|


The origin shift is a vector of rigid shift, :math:`\boldsymbol{p}`,
between the origin :math:`\boldsymbol{o}` of a reference point in the
standard basis system and an origin :math:`\boldsymbol{o}'` of
another reference point in arbitrary basis system. When the two points
:math:`\boldsymbol{x}` with respect to :math:`\boldsymbol{o}` and
:math:`\boldsymbol{x}'` with respect to
:math:`\boldsymbol{o}'` give the same position in Cartesian
coordinates, the relation between :math:`\boldsymbol{x}` and
:math:`\boldsymbol{x}'` are defined using
:math:`\boldsymbol{p}` by

.. math::

  \boldsymbol{x}' = \boldsymbol{x} - \boldsymbol{p}.
   


.. |sflogo| image:: http://sflogo.sourceforge.net/sflogo.php?group_id=161614&type=1
            :target: http://sourceforge.net

|sflogo|
