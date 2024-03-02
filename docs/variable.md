# Variables

Variables frequently used in C-API are explained. In interfaces of
other languages, the definition may be different, e.g., `lattice`
for Python interface is transposed.

(variables_lattice)=

## `lattice`

**Attention**: In Python interface, `lattice` is transposed
[](#py_variables_crystal_structure).

Basis vectors (in Cartesian coordinates)

```
[ [ a_x, b_x, c_x ],
  [ a_y, b_y, c_y ],
  [ a_z, b_z, c_z ] ]
```

See [Basis vectors](#def_basis_vectors) and [Atomic point
coordinates](#def_point_coordinates) for more details.

## `position`

Atomic points (in fractional coordinates with respect to basis vectors)

```
[ [ x1_1, x1_2, x1_3 ],
  [ x2_1, x2_2, x2_3 ],
  [ x3_1, x3_2, x3_3 ],
  ...                   ]
```

See [Basis vectors](#def_basis_vectors) and [Atomic point
coordinates](#def_point_coordinates) for more details.

## `types`

Atomic species are differentiated by integers. The numbers are just
used to distinguishes atoms, and is not related to atomic numbers.

```
[ type_1, type_2, type_3, ... ]
```

The number of elements is same as the number of atoms.

## `rotation`

Rotation matrices (integer) of symmetry operations.

```
[ [ W_11, W_12, W_13 ],
  [ W_21, W_22, W_23 ],
  [ W_31, W_32, W_33 ] ]
```

See [Symmetry operation](#def_symmetry_operation) for more details.

## `translation`

Translation vectors corresponding to symmetry operations in fractional
coordinates.

```
[ w_1, w_2, w_3 ]
```

See [Symmetry operation](#def_symmetry_operation) for more details.

(variables_symprec)=

## `symprec`

Distance tolerance in Cartesian coordinates to find crystal
symmetry.

For atomic positions, roughly speaking, two position vectors `x` and `x'` in
Cartesian coordinates are considered to be the same if `|x' - x| < symprec`.
For more details, see the [spglib paper](https://arxiv.org/abs/1808.01590),
Sec. II-A.

The angle distortion between basis vectors is converted to a length
and compared with this distance tolerance. For more details, see the
[spglib paper](https://arxiv.org/abs/1808.01590), Sec. IV-A. It is
possible to specify angle tolerance explicitly, see
[](#variables_angle_tolerance).

(variables_angle_tolerance)=

## `angle_tolerance`

Tolerance of angle between basis vectors in degrees to be tolerated
in the symmetry finding. To use angle tolerance, another set of
functions are prepared for C-API as follows

```
spgat_get_dataset
spgat_get_symmetry
spgat_get_symmetry_with_collinear_spin
spgat_get_multiplicity
spgat_find_primitive
spgat_get_international
spgat_get_schoenflies
spgat_refine_cell
```

(variables_mag_symprec)=

## `mag_symprec`

Tolerance for magnetic symmetry search in the unit of spins or magmoms. By
specifying a negative value, the same value as `symprec` is used to compare
magnetic moments.

The following functions are called by the same way with an additional argument
of `const double angle_tolerance` in degrees. By specifying a negative value,
the behavior becomes the same as usual functions (e.g.
`spg_get_magnetic_dataset`). For python-API, the magnetic tolerance is given
with a keyword argument of `mag_symprec`, where the default value of
`mag_symprec` is a negative value.

```
spgms_get_magnetic_dataset
spgms_get_symmetry_with_collinear_spin
spgms_get_symmetry_with_site_tensors
```
