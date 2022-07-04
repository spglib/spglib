Magnetic Space group (MSG)
==========================

.. contents::

Definition
----------

A **magnetic space group** :math:`\mathcal{M}` is a subgroup of direct product of space group :math:`\mathcal{G}` and :math:`\mathbb{Z}_{2} \simeq \{1, 1' \}`.

A **family space group** (FSG) :math:`\mathcal{F}(\mathcal{M})` is a non-magnetic space group obtained by ignoring primes in operations,

.. math::
    \mathcal{F}(\mathcal{M}) = \left\{ (\mathbf{R}, \mathbf{v}) \mid (\mathbf{R}, \mathbf{v}) \in \mathcal{M} \vee (\mathbf{R}, \mathbf{v})1' \in \mathcal{M} \right\}.

A **maximal space subgroup** (XSG) :math:`\mathcal{D}(\mathcal{M})` is a space group obtained by removing primed operations,

.. math::
    \mathcal{D}(\mathcal{M}) = \left\{ (\mathbf{R}, \mathbf{v}) \mid (\mathbf{R}, \mathbf{v}) \in \mathcal{M} \right\}.

Magnetic space groups are classified to the following four types:

- (type-I) :math:`\mathcal{M} \simeq \mathcal{F}(\mathcal{M}) \simeq \mathcal{D}(\mathcal{M})`.
- (type-II) :math:`\mathcal{M} \simeq \mathcal{F}(\mathcal{M}) \times \{1, 1'\}, \mathcal{F}(\mathcal{M}) \simeq \mathcal{D}(\mathcal{M})`.
- (type-III)
  :math:`\mathcal{D}(\mathcal{M})` is a translationengleiche subgroup of index two of :math:`\mathcal{F}(\mathcal{M})`.

  .. math::
    \mathcal{M} \simeq \mathcal{D}(\mathcal{M}) \sqcup (\mathcal{F}(\mathcal{M}) \backslash \mathcal{D}(\mathcal{M})) 1'

- (type-IV)
  :math:`\mathcal{D}(\mathcal{M})` is a klessengleichesubgroup of index two of :math:`\mathcal{F}(\mathcal{M})`.

  .. math::
    \mathcal{M} \simeq \mathcal{D}(\mathcal{M}) \sqcup (\mathcal{F}(\mathcal{M}) \backslash \mathcal{D}(\mathcal{M})) 1'

.. list-table:: Number of (3-dimensional) magnetic space groups
    :header-rows: 1

    * - type-I
      - type-II
      - type-III
      - type-IV
      - Total
    * - 230
      - 230
      - 674
      - 517
      - 1651

For type-I, II, III MSG, both OG and BNS symbols use a setting of :math:`\mathcal{F}(\mathcal{M})`.
For type-IV MSG, OG symbols use a setting of :math:`\mathcal{F}(\mathcal{M})` and BNS symbols use that of :math:`\mathcal{D}(\mathcal{M})`.

Action of MSG operations
------------------------

In general, we can arbitrarily choose how MSG operation acts on objects as long as it satisfies the definition of left actions.
Here are the examples of objects and actions on them:

* Collinear magnetic moment :math:`m_{z}` without spin-orbit coupling

.. math::
    (\mathbf{R}, \mathbf{v}) \circ m_{z} &= m_{z} \\
    1' \circ m_{z} &= -m_{z}

* Magnetic moment :math:`\mathbf{m}`

.. math::
    (\mathbf{R}, \mathbf{v}) \circ \mathbf{m} &= (\mathrm{det} \mathbf{R}) \mathbf{R} \mathbf{m} \\
    1' \circ \mathbf{m} &= -\mathbf{m}

* Grain boundary made of "black" and "white" crystals

.. math::
    (\mathbf{R}, \mathbf{v}) \circ (\mathbf{r}, \mathrm{black}) &= ( \mathbf{R}\mathbf{r} + \mathbf{v}, \mathrm{black} ) \\
    (\mathbf{R}, \mathbf{v}) \circ (\mathbf{r}, \mathrm{white}) &= ( \mathbf{R}\mathbf{r} + \mathbf{v}, \mathrm{white} ) \\
    1' \circ (\mathbf{r}, \mathrm{black}) &= (\mathbf{r}, \mathrm{white}) \\
    1' \circ (\mathbf{r}, \mathrm{white}) &= (\mathbf{r}, \mathrm{black}) \\

MSG detection for crystal structure
-----------------------------------

Formally, :code:`cell` consists of

- column-wise lattice matrix :math:`\mathbf{A}`
- fractional coordinates :math:`\mathbf{X}`
- atomic types :math:`\mathbf{T}`
- magmoms :math:`\mathbf{M}`

SG corresponds to the following equivalence relation (implemented in :code:`cel_is_overlap_with_same_type`) as :math:`\mathcal{S} := \mathrm{Stab}_{\mathrm{E}(3)} \, (\mathbf{X}, \mathbf{T}) / \sim_{\mathcal{S}}`,

.. math::
    (X_{i}, T_{i}) \sim_{\mathcal{S}} (X_{j}, T_{j})
    \overset{\mathrm{def}}{\iff}
    \exists g \in \mathrm{E}(3) \, s.t. \,  g \circ X_{i} = X_{j} \,\mathrm{and}\,  T_{i} = T_{j}.

XSG corresponds to the following equivalence relation as :math:`\mathcal{D} := \mathrm{Stab}_{ \mathcal{S} } \, (\mathbf{X}, \mathbf{T}, \mathbf{M}) / \sim_{\mathcal{D}}`,

.. math::
    (X_{i}, T_{i}, M_{i}) \sim_{\mathcal{D}} (X_{j}, T_{j}, M_{j})
    \overset{\mathrm{def}}{\iff}
    \exists g \in \mathcal{S} \, s.t. \, g \circ M_{i} = M_{j}.

MSG corresponds to the following equivalence relation as :math:`\mathcal{M} = \mathrm{Stab}_{ \mathcal{S} \times \{ 1, 1' \} } \, (\mathbf{X}, \mathbf{T}, \mathbf{M}) / \sim_{\mathcal{M}}`

.. math::
    (X_{i}, T_{i}, M_{i}) \sim_{\mathcal{M}} (X_{j}, T_{j}, M_{j})
    \overset{\mathrm{def}}{\iff}
    \exists g\theta \in \mathcal{S} \times \{ 1, 1' \} \, s.t. \, g \theta \circ M_{i} = M_{j}

:math:`\mathcal{S}` may not be index-two supergroup of :math:`\mathcal{D}` because the former ignores magmom configurations.
Thus, we compute FSG via MSG as :math:`\mathcal{F} := \mathcal{M}  / \{ 1, 1' \}`.

Procedure to detect MSG operations
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In actual implementation in :code:`spn_get_operations_with_site_tensors`, the following steps two, three, and four are performed at the same time.

#. Compute SG :math:`\mathcal{S}` by just ignoring magmoms.
#. Compute XSG :math:`\mathcal{D}` by checking :math:`g \circ M_{i} = M_{g \circ i}` one by one for :math:`g \in \mathcal{S}`.
#. Compute MSG :math:`\mathcal{M}` by checking :math:`g \theta \circ M_{i} = M_{g \circ i}` one by one for :math:`g \theta \in \mathcal{S} \times \{ 1, 1' \}`.
#. Compute FSG :math:`\mathcal{F}` by ignoring primes in :math:`\mathcal{M}`

In :code:`spgat_get_symmetry_with_site_tensors`, :code:`sym_nonspin` corresponds to :math:`\mathcal{S}`.
Then, :code:`spn_get_operations_with_site_tensors` computes :math:`\mathcal{M}` under the following actions:

- :code:`tensor_rank=0`
    - :code:`is_magnetic=true`: :math:`1' \circ m = -m, (\mathbf{R}, \mathbf{v}) \circ m = m`
    - :code:`is_magnetic=false`: :math:`1' \circ m_{z} = m_{z}, (\mathbf{R}, \mathbf{v}) \circ m = m`
- :code:`tensor_rank=1` (currently only support axial vector)
    - :code:`is_magnetic=true`: :math:`1' \circ \mathbf{m} = -\mathbf{m}, (\mathbf{R}, \mathbf{v}) \circ \mathbf{m} = (\mathrm{det} \mathbf{R}) \mathbf{R} \mathbf{m}`
    - :code:`is_magnetic=false`: :math:`1' \circ \mathbf{m} = \mathbf{m}, (\mathbf{R}, \mathbf{v}) \circ \mathbf{m} = (\mathrm{det} \mathbf{R}) \mathbf{R} \mathbf{m}`


Procedure to standardize MSG setting
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

:code:`spglib.c:get_magnetic_dataset`

#. Determine type of MSG
    - When :math:`[\mathcal{F}:\mathcal{D}] = 1`
        - :math:`[\mathcal{M}:\mathcal{F}] = 1` -> Type-I
        - :math:`[\mathcal{M}:\mathcal{F}] = 2` -> Type-II
    - When :math:`[\mathcal{F}:\mathcal{D}] = 2`
        - Find a coset decomposition :math:`\mathcal{M} = \mathcal{D} \sqcup g \mathcal{D}`
        - :math:`g` is not translation -> Type-III
        - :math:`g` is translation -> Type-IV
#. Choose reference setting
    - Type-I, II, III -> Hall symbol of :math:`\mathcal{F}`
    - Type-IV -> Hall symbol of :math:`\mathcal{D}`
#. Compare :math:`\mathcal{M}` with MSGs in database after applying a transformation to the Hall symbol's setting
#. Standardize cell

Transformation of space group :math:`\mathcal{G}`
-------------------------------------------------

We denote a primitive lattice as :math:`L_{\mathrm{prim}}` and a conventional lattice as :math:`L_{\mathrm{conv}}`.
Here the centering vectors correspond to :math:`L_{\mathrm{conv}} / L_{\mathrm{prim}}`.
For example, trigonal space groups with hexagonal cell (obverse) give

.. math::
    L_{\mathrm{conv}} / L_{\mathrm{prim}} =
    \left\{
        (0, 0, 0),
        (2/3, 1/3, 1/3),
        (1/3, 2/3, 2/3)
    \right\}.

We write translation group formed by :math:`L_{\mathrm{prim}}` as :math:`\mathcal{T}_{\mathrm{prim}}`, and translation group formed by :math:`L_{\mathrm{conv}}` as :math:`\mathcal{T}_{\mathrm{conv}}`.
Space group :math:`\mathcal{G}` can be written by finite factor group :math:`\mathcal{G} / \mathcal{T}_{\mathrm{conv}}` and the centerings:

.. math::
    \mathcal{G} / \mathcal{T}_{\mathrm{prim}}
    =
    \left\{
        (\mathbf{I}, \mathbf{c}) (\mathbf{W}, \mathbf{w})
        \mid
        (\mathbf{W}, \mathbf{w}) \in \mathcal{G} / \mathcal{T}_{\mathrm{conv}},
        \mathbf{c} \in L_{\mathrm{conv}} / L_{\mathrm{prim}}
    \right\}


Consider a transformation matrix :math:`\mathbf{P}` (corresponds to :code:`Spacegroup.bravais_lattice`) and origin shift :math:`\mathbf{p}` (corresponds to :code:`Spacegroup.origin_shift`).
This transformation convert the factor group :math:`\mathcal{G} / \mathcal{T}_{\mathrm{conv}}` to

.. math::
    \left\{
        (\mathbf{P}, \mathbf{p}) (\mathbf{W}, \mathbf{w}) (\mathbf{P}, \mathbf{p})^{-1}
        \mid
        (\mathbf{W}, \mathbf{w}) \in \mathcal{G} / \mathcal{T}_{\mathrm{conv}}
    \right\}

If the conventional lattice is properly chosen, each operation in :math:`\mathcal{G} / \mathcal{T}_{\mathrm{conv}}` has a unique linear part.
Then, the above transformed factor group has the same order as :math:`\mathcal{G} / \mathcal{T}_{\mathrm{conv}}`.

The centering vectors are transformed as

.. math::
    \left\{
        \mathbf{P} (\mathbf{c} + \mathbf{n}) \, \mathrm{mod}\, 1
        \mid
        \mathbf{c} \in L_{\mathrm{conv}} / L_{\mathrm{prim}}, \mathbf{n} \in \mathbb{Z}^{3}
    \right\}.

When :math:`|\det \mathbf{P}| = 1`, the transformed centering vectors give the same conventional lattice.
When :math:`|\det \mathbf{P}| > 1`, some centering vectors become duplicated.
When :math:`|\det \mathbf{P}| < 1`, we need to additionally find lattice points.
We can confirm the range of :math:`\mathbf{n}` to :math:`[0, d)^{3}`, where :math:`d` is maximum denominator of :math:`\mathbf{P}`.

Transformation of magnetic space group :math:`\mathcal{M}`
----------------------------------------------------------

We need care for anti-translation.

.. math::
    \mathcal{M}
        &= \mathcal{D} \sqcup g \mathcal{D} \\
    \mathcal{M} / \mathcal{T}_{\mathrm{prim}}(\mathcal{M})
        &=
        \langle g \rangle
        \cdot ( \mathcal{T}_{\mathrm{conv}}(\mathcal{M}) / \mathcal{T}_{\mathrm{prim}}(\mathcal{M}) )
        \cdot ( \mathcal{M} / \langle g \rangle / \mathcal{T}_{\mathrm{conv}}(\mathcal{M}) ) \\
        &=
        \langle g \rangle
        \cdot ( \mathcal{T}_{\mathrm{conv}}(\mathcal{M}) / \mathcal{T}_{\mathrm{prim}}(\mathcal{M}) )
        \cdot ( \mathcal{D} / \mathcal{T}_{\mathrm{conv}}(\mathcal{M}) ) \\

Each element in factor group :math:`\mathcal{D} / \mathcal{T}_{\mathrm{conv}}(\mathcal{D})` has a unique linear part.

References
----------

[Lit14] D. B. Litvin, Magnetic Group Tables 1-, 2- and 3-Dimensional Magnetic Subperiodic Groups and Magnetic Space Groups (IUCr, 2014).

[SC22] Harold T. Stokes and Branton J. Campbell, [ISO-MAG Table of Magnetic Space Groups](https://stokes.byu.edu/iso/magneticspacegroups.php).

[GPKRC21] J. González-Platas, N. A. Katcho and J. Rodríguez-Carvajal, J. Appl. Crystallogr. 54, 1, 338-342 (2021).
