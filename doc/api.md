# C-APIs

## Version

### `spg_get_major_version`, `spg_get_minor_version`, `spg_get_micro_version`

**New in version 1.8.3**

Version number of spglib is obtained. These three functions return
integers that correspond to spglib version \[major\].\[minor\].\[micro\].

## Error

### `spg_get_error_code` and `spg_get_error_message`

**New in version 1.9.5**

**Be careful. These are not thread safe, i.e., only safely usable
when calling one spglib function per process.**

These functions is used to see roughly why spglib failed.

```c
SpglibError spg_get_error_code(void);
```

```c
char * spg_get_error_message(SpglibError spglib_error);
```

The `SpglibError` type is a enum type as shown below.

```c
typedef enum {
   SPGLIB_SUCCESS = 0,
   SPGERR_SPACEGROUP_SEARCH_FAILED,
   SPGERR_CELL_STANDARDIZATION_FAILED,
   SPGERR_SYMMETRY_OPERATION_SEARCH_FAILED,
   SPGERR_ATOMS_TOO_CLOSE,
   SPGERR_POINTGROUP_NOT_FOUND,
   SPGERR_NIGGLI_FAILED,
   SPGERR_DELAUNAY_FAILED,
   SPGERR_ARRAY_SIZE_SHORTAGE,
   SPGERR_NONE,
} SpglibError;
```

The usage is as follows

```c
SpglibError error;
error = spg_get_error_code();
printf("%s\n", spg_get_error_message(error));
```

## Space-group symmetry search

(api_spg_get_symmetry)=

### `spg_get_symmetry`

This function finds a set of representative symmetry operations for
primitive cells or its extension with lattice translations for
supercells. 0 is returned if it failed.

```c
int spg_get_symmetry(int rotation[][3][3],
                     double translation[][3],
                     const int max_size,
                     const double lattice[3][3],
                     const double position[][3],
                     const int types[],
                     const int num_atom,
                     const double symprec);
```

The operations are stored in `rotation` and `translation`. The
number of operations is return as the return value. Rotations and
translations are given in fractional coordinates, and `rotation[i]`
and `translation[i]` with same index give a symmetry operations,
i.e., these have to be used together.

As an exceptional case, if a supercell (or non-primitive cell) has the
basis vectors whose lattice breaks crystallographic point group, the
crystallographic symmetry operations are searched within this broken
symmetry, i.e., at most the crystallographic point group found in this
case is the point group of the lattice. For example, this happens for
the $2\times 1\times 1$ supercell of a conventional cubic unit
cell. This may not be understandable in crystallographic sense, but is
practically useful treatment for research in computational materials
science.

(api_spg_get_dataset)=

### `spg_get_dataset` and `spg_get_dataset_with_hall_number`

**Changed in version 1.8.1**

For an input unit cell structure, symmetry operations of the crystal
are searched. Then they are compared with the crystallographic
database and the space group type is determined.  The result is
returned as the `SpglibDataset` structure as a dataset.

The detail of the dataset is given at {ref}`spglib_dataset`.

Dataset corresponding to the space group type in the standard setting
is obtained by `spg_get_dataset`. Here the standard setting means
the first top one among the Hall symbols listed for each space group
type. For example, H setting (hexagonal lattice) is chosen for
rhombohedral crystals. `spg_get_dataset_with_hall_number` explained
below is used to choose different settings such as R setting of
rhombohedral crystals. If this symmetry search fails, `NULL` is
returned in version 1.8.1 or later (spacegroup_number = 0 is returned
in the previous versions). In this function, the other
crystallographic setting is not obtained.

```c
SpglibDataset * spg_get_dataset(const double lattice[3][3],
                                const double position[][3],
                                const int types[],
                                const int num_atom,
                                const double symprec);
```

To specify the other crystallographic choice (setting, origin, axis,
or cell choice), `spg_get_dataset_with_hall_number` is used.

```c
SpglibDataset * spg_get_dataset_with_hall_number(const double lattice[3][3],
                                                 const double position[][3],
                                                 const int types[],
                                                 const int num_atom,
                                                 const int hall_number,
                                                 const double symprec)
```

where `hall_number` is used to specify the choice. The possible
choices and those serial numbers are found at [list of space groups
(Seto's web site)](https://yseto.net/?page_id=29&lang=en).
The crystal structure has to possess the space-group type of the Hall
symbol. If the symmetry search fails or the specified `hall_number`
is not in the list of Hall symbols for the space group type of the
crystal structure, `spacegroup_number` in the `SpglibDataset`
structure is set 0.

Finally, its allocated memory space must be freed by calling
`spg_free_dataset`.

### `spg_free_dataset`

Allocated memory space of the C-structure of `SpglibDataset` is
freed by calling `spg_free_dataset`.

```c
void spg_free_dataset(SpglibDataset *dataset);
```

### `spg_get_multiplicity`

This function returns exact number of symmetry operations. 0 is
returned when it failed.

```c
int spg_get_multiplicity(const double lattice[3][3],
                         const double position[][3],
                         const int types[],
                         const int num_atom,
                         const double symprec);
```

This function may be used in advance to allocate memory space for
symmetry operations.

## Space-group type search

### `spg_get_international`

Space group type is found and returned in international table symbol
to `symbol` and also as a number (return value). 0 is returned if
it failed.

```c
int spg_get_international(char symbol[11],
                          const double lattice[3][3],
                          const double position[][3],
                          const int types[],
                          const int num_atom,
                          const double symprec);
```

### `spg_get_schoenflies`

Space group type is found and returned in schoenflies to `symbol`
and also as a number (return value). 0 is returned if it failed.

```c
int spg_get_schoenflies(char symbol[7],
                        const double lattice[3][3],
                        const double position[][3],
                        const int types[],
                        const int num_atom,
                        const double symprec);
```

## Standardization and finding primitive cell

### `spg_standardize_cell`

The standardized unit cell (see {ref}`def_standardized_unit_cell`) is
generated from an input unit cell structure and its symmetry found by
the symmetry search. The choice of the setting for each space group
type is as explained for {ref}`spg_get_dataset <api_spg_get_dataset>`.
Usually `to_primitive=0` and `no_idealize=0` are recommended to
set and this setting results in the same behavior as
`spg_refine_cell`. 0 is returned if it failed.

```c
int spg_standardize_cell(double lattice[3][3],
                         double position[][3],
                         int types[],
                         const int num_atom,
                         const int to_primitive,
                         const int no_idealize,
                         const double symprec);
```

Number of atoms in the found standardized unit (primitive) cell is
returned.

`to_primitive=1` is used to create the standardized primitive cell
with the transformation matrices shown at
{ref}`def_standardized_primitive_cell`, otherwise `to_primitive=0`
must be specified. The found basis vectors and
atomic point coordinates and types are overwritten in `lattice`,
`position`, and `types`, respectively. Therefore with
`to_primitive=0`, at a maximum four times larger array size for
`position` and `types` than the those size of the input unit cell
is required to store a standardized unit cell with face centring found
in the case that the input unit cell is a primitive cell.

`no_idealize=0` is used to idealize the lengths and angles of basis
vectors with adjusting the positions of atoms to nearest exact
positions according to crystal symmetry. However the crystal can be
rotated in Cartesian coordinates by the idealization of the basis
vectors.  `no_idealize=1` disables this. The detail of the
idealization (`no_idealize=0`) is written at
{ref}`def_idealize_cell`. `no_idealize=1` may be useful when we want
to leave basis vectors and atomic positions in Cartesian coordinates
fixed.

### `spg_find_primitive`

**Behavior is changed. This function is now a shortcut of**
`spg_standardize_cell` **with**
`to_primitive=1` **and** `no_idealize=0`.

A primitive cell is found from an input unit cell. 0 is returned if it
failed.

```c
int spg_find_primitive(double lattice[3][3],
                       double position[][3],
                       int types[],
                       const int num_atom,
                       const double symprec);
```

`lattice`, `position`, and `types` are overwritten. Number of
atoms in the found primitive cell is returned. The crystal can be
rotated by this function. To avoid this, please use
`spg_standardize_cell` with `to_primitive=1` and `no_idealize=1`
although the crystal structure is not idealized.

### `spg_refine_cell`

**This function exists for backward compatibility since it is same
as** `spg_standardize_cell` **with** `to_primitive=0` **and**
`no_idealize=0`.

The standardized crystal structure is obtained from a non-standard
crystal structure which may be slightly distorted within a symmetry
recognition tolerance, or whose primitive vectors are differently
chosen, etc. 0 is returned if it failed.

```c
int spg_refine_cell(double lattice[3][3],
                    double position[][3],
                    int types[],
                    const int num_atom,
                    const double symprec);
```

The calculated standardized lattice and atomic positions overwrites
`lattice`, `position`, and `types`. The number of atoms in the
standardized unit cell is returned as the return value. When the input
unit cell is a primitive cell and is the face centring symmetry, the
number of the atoms returned becomes four times large. Since this
function does not have any means of checking the array size (memory
space) of these variables, the array size (memory space) for
`position` and `types` should be prepared **four times more** than
those required for the input unit cell in general.

## Space-group dataset access

### `spg_get_symmetry_from_database`

This function allows to directly access to the space group operations
in the spglib database (spg_database.c). To specify the space group
type with a specific choice, `hall_number` is used. The definition
of `hall_number` is found at
{ref}`dataset_spg_get_dataset_spacegroup_type`. 0 is returned when it
failed.

```c
int spg_get_symmetry_from_database(int rotations[192][3][3],
                                   double translations[192][3],
                                   const int hall_number);
```

The returned value is the number of space group operations. The space
group operations are stored in `rotations` and `translations`.

(api_spg_spacegroup_type)=

### `spg_get_spacegroup_type`

**Changed at version 1.9.4: Some members are added and the member name 'setting' is changed to 'choice'.**
**Changed at version 2.0: Add 'hall_number' member.**

This function allows to directly access to the space-group-type
database in spglib (spg_database.c). To specify the space group type
with a specific choice, `hall_number` is used. The definition of
`hall_number` is found at
{ref}`dataset_spg_get_dataset_spacegroup_type`.
`number = 0` is returned when it failed.

```c
SpglibSpacegroupType spg_get_spacegroup_type(const int hall_number)
```

`SpglibSpacegroupType` structure is as follows:

```c
typedef struct {
    int number;
    char international_short[11];
    char international_full[20];
    char international[32];
    char schoenflies[7];
    char hall_symbol[17];
    int hall_number;
    char choice[6];
    char pointgroup_schoenflies[4];
    char pointgroup_international[6];
    int arithmetic_crystal_class_number;
    char arithmetic_crystal_class_symbol[7];
} SpglibSpacegroupType;
```

(api_spg_get_spacegroup_type_from_symmetry)=

### `spg_get_spacegroup_type_from_symmetry`

**New at version 2.0**

Return space-group type information from symmetry operations.
This is the replacement of `spg_get_hall_number_from_symmetry` (deprecated at v2.0).

This is expected to work well for the set of symmetry operations whose
distortion is small. The aim of making this feature is to find
space-group-type for the set of symmetry operations given by the other
source than spglib.

```c
SpglibSpacegroupType spg_get_spacegroup_type_from_symmetry(
    const int rotations[][3][3], const double translations[][3],
    const int num_operations, const double lattice[3][3], const double symprec
);
```

The `SpglibSpacegroupType` structure is explained at {ref}`api_spg_spacegroup_type`.
The parameter `lattice` is used as the distance measure for `symprec`. If it
is unknown, the following may be a reasonable choice.

```
lattice[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
```

## Magnetic symmetry

### `spg_get_symmetry_with_collinear_spin`

This function finds symmetry operations with collinear polarizations
(spins) on atoms. Except for the argument of `const double spins[]`,
the usage is basically the same as `spg_get_symmetry`, but as an
output, `equivalent_atoms` are obtained. The size of this array is
the same of `num_atom`. See {ref}`dataset_spg_get_dataset_site_symmetry`
for the definition `equivalent_atoms`. 0 is returned when it failed.

```c
int spg_get_symmetry_with_collinear_spin(int rotation[][3][3],
                                         double translation[][3],
                                         int equivalent_atoms[],
                                         const int max_size,
                                         const double lattice[3][3],
                                         const double position[][3],
                                         const int types[],
                                         const double spins[],
                                         const int num_atom,
                                         const double symprec);
```

### `spg_get_symmetry_with_site_tensors`

**Experimental: new at version 2.0**

Returns magnetic symmetry operations represented by `rotation`, `translation`, and `spin_flips`
from crystal structure with `lattice`, `position`, `types`, and `tensors`.
When you need magnetic symmetry operations of non-collinear spins, set `tensor_rank=1`, `is_axial=1`,
and `tensors` with length `num_atom * 3` (ordered by $[ m_{1x}, m_{1y}, m_{1z}, \dots ]$)
in cartesian coordinates.
For other combinations of `tensor_rank` and `is_axial`, see {ref}`magnetic_action_flags`.
Returned `spin_flips` represents sign of site tensors after applying time-reversal operations:
1 for non time reversal, and -1 for time reversal.

```c
int spg_get_symmetry_with_site_tensors(
    int rotation[][3][3], double translation[][3], int equivalent_atoms[],
    double primitive_lattice[3][3], int *spin_flips, const int max_size,
    const double lattice[3][3], const double position[][3],
    const int types[], const double *tensors, const int tensor_rank,
    const int num_atom, const int with_time_reversal, const int is_axial,
    const double symprec);
```

### `spg_get_magnetic_dataset`

**Experimental: new at version 2.0**

Return magnetic symmetry operations and standardized structure of given structure with site tensors.

- To search magnetic symmetry operations of a structure with collinear spins, set `tensor_rank=0`,
  `is_axial=0`, and `tensors` with length `num_atom`
- To search magnetic symmetry operations of a structure with non-collinear spins, set `tensor_rank=1`,
  `is_axial=1`, and `tensors` with length `num_atom * 3` in cartesian coordinates.

The description of returned dataset is given at {ref}`magnetic_spglib_dataset`.

```c
SpglibMagneticDataset *spg_get_magnetic_dataset(
    const double lattice[3][3], const double position[][3],
    const int types[], const double *tensors, const int tensor_rank,
    const int num_atom, const int is_axial, const double symprec);
```

### `spg_get_magnetic_symmetry_from_database`

**Experimental: new at version 2.0**

Magnetic space-group operations in built-in database are accessed by UNI
number, which is defined as number from 1 to 1651.
Optionally alternative settings can be specified with hall_number.
For type-I, type-II, and type-III magnetic space groups, `hall_number`
changes settings in family space group.
For type-IV, `hall_number` changes settings in maximal space group.
When `hall_number = 0`, the smallest hall number corresponding to uni_number is
used.

```c
int spg_get_magnetic_symmetry_from_database(int rotations[384][3][3],
                                            double translations[384][3],
                                            int time_reversals[384],
                                            const int uni_number,
                                            const int hall_number);
```

### `spg_free_magnetic_dataset`

**Experimental: new at version 2.0**

```c
void spg_free_magnetic_dataset(SpglibMagneticDataset *dataset);
```

(api_get_magnetic_spacegroup_type)=

### `spg_get_magnetic_spacegroup_type`

**Experimental: new at version 2.0**

Magnetic space-group type information is accessed by serial number of UNI (or BNS) symbols.
The serial number is between 1 and 1651.

```c
SpglibMagneticSpacegroupType spg_get_magnetic_spacegroup_type(
    const int uni_number);
```

Returned `SpglibMagneticSpacegroupType` is the following C-structure:

```c
typedef struct {
    int uni_number;
    int litvin_number;
    char bns_number[8];
    char og_number[12];
    int number;
    int type;
} SpglibMagneticSpacegroupType;
```

- `uni_number` serial number of UNI (or BNS) symbols
- `litvin_number` serial number in Litvin's [Magnetic group tables](https://www.iucr.org/publ/978-0-9553602-2-0)
- `bns_number` BNS number e.g. "151.32"
- `og_number` OG number e.g. "153.4.1270"
- `number` ITA's serial number of space group for reference setting
- `type` Type of MSG from 1 to 4

### `spg_get_magnetic_spacegroup_type_from_symmetry`

**Experimental: new at version 2.0**

Return magnetic space-group type information from magnetic symmetry operations.
`time_reversals` takes 0 for ordinary operations and 1 for time-reversal operations.

```c
SpglibMagneticSpacegroupType spg_get_magnetic_spacegroup_type_from_symmetry(
    const int rotations[][3][3], const double translations[][3],
    const int *time_reversals, const int num_operations,
    const double lattice[3][3], const double symprec
);
```

See {ref}`api_get_magnetic_spacegroup_type` for returned `SpglibMagneticSpacegroupType`.

## Lattice reduction

### `spg_niggli_reduce`

Niggli reduction is applied to input basis vectors `lattice` and the
reduced basis vectors are overwritten to `lattice`. 0 is returned if
it failed.

```c
int spg_niggli_reduce(double lattice[3][3], const double symprec);
```

The transformation from original basis vectors $( \mathbf{a}
\; \mathbf{b} \; \mathbf{c} )$ to final basis vectors $(
\mathbf{a}' \; \mathbf{b}' \; \mathbf{c}' )$ is achieved by linear
combination of basis vectors with integer coefficients without
rotating coordinates. Therefore the transformation matrix is obtained
by $\boldsymbol{P} = ( \mathbf{a} \; \mathbf{b} \; \mathbf{c} )
( \mathbf{a}' \; \mathbf{b}' \; \mathbf{c}' )^{-1}$ and the matrix
elements have to be almost integers.

### `spg_delaunay_reduce`

Delaunay reduction is applied to input basis vectors `lattice` and
the reduced basis vectors are overwritten to `lattice`. 0 is
returned if it failed.

```c
int spg_delaunay_reduce(double lattice[3][3], const double symprec);
```

The transformation from original basis vectors $( \mathbf{a}
\; \mathbf{b} \; \mathbf{c} )$ to final basis vectors $(
\mathbf{a}' \; \mathbf{b}' \; \mathbf{c}' )$ is achieved by linear
combination of basis vectors with integer coefficients without
rotating coordinates. Therefore the transformation matrix is obtained
by $\boldsymbol{P} = ( \mathbf{a} \; \mathbf{b} \; \mathbf{c} )
( \mathbf{a}' \; \mathbf{b}' \; \mathbf{c}' )^{-1}$ and the matrix
elements have to be almost integers.

## Kpoints

### `spg_get_ir_reciprocal_mesh`

Irreducible reciprocal grid points are searched from uniform mesh grid
points specified by `mesh` and `is_shift`.

```c
int spg_get_ir_reciprocal_mesh(int grid_address[][3],
                               int map[],
                               const int mesh[3],
                               const int is_shift[3],
                               const int is_time_reversal,
                               const double lattice[3][3],
                               const double position[][3],
                               const int types[],
                               const int num_atom,
                               const double symprec)
```

`mesh` stores three integers. Reciprocal primitive vectors are
divided by the number stored in `mesh` with (0,0,0) point
centering. The center of grid mesh is shifted +1/2 of a grid spacing
along corresponding reciprocal axis by setting 1 to a `is_shift`
element. No grid mesh shift is made if 0 is set for `is_shift`.

The reducible uniform grid points are returned in fractional coordinates
as `grid_address`. A map between reducible and irreducible points are
returned as `map` as in the indices of `grid_address`. The number of
the irreducible k-points are returned as the return value.  The time
reversal symmetry is imposed by setting `is_time_reversal` 1.

Grid points are stored in the order that runs left most element
first, e.g. (4x4x4 mesh).

```
[[ 0  0  0]
 [ 1  0  0]
 [ 2  0  0]
 [-1  0  0]
 [ 0  1  0]
 [ 1  1  0]
 [ 2  1  0]
 [-1  1  0]
 ....      ]
```

where the first index runs first.  k-qpoints are calculated by
`(grid_address + is_shift / 2) / mesh`. A grid point index is
recovered from `grid_address` by `numpy.dot(grid_address % mesh, [1, mesh[0], mesh[0] * mesh[1]])` in Python-numpy notation, where
`%` always returns non-negative integers. The order of
`grid_address` can be changed so that the last index runs first by
setting the macro `GRID_ORDER_XYZ` in `kpoint.c`. In this case the
grid point index is recovered by `numpy.dot(grid_address % mesh, [mesh[2] * mesh[1], mesh[2], 1])`.

### `spg_get_stabilized_reciprocal_mesh`

The irreducible k-points are searched from unique k-point mesh grids
from direct (real space) basis vectors and a set of rotation parts of
symmetry operations in direct space with one or multiple
stabilizers.

```c
int spg_get_stabilized_reciprocal_mesh(int grid_address[][3],
                                       int map[],
                                       const int mesh[3],
                                       const int is_shift[3],
                                       const int is_time_reversal,
                                       const int num_rot,
                                       const int rotations[][3][3],
                                       const int num_q,
                                       const double qpoints[][3])
```

The stabilizers are written in fractional coordinates. Number of the
stabilizers are given by `num_q`. Symmetrically equivalent k-points
(stars) in fractional coordinates are stored in `map` as indices of
`grid_address`. The number of reduced k-points with the stabilizers
are returned as the return value.

This function can be used to obtain all mesh grid points by setting
`num_rot = 1`, `rotations = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}`,
`num_q = 1`, and `qpoints = {0, 0, 0}`.

## Deprecated

### `spg_get_hall_number_from_symmetry`

**Deprecated at version 2.0. This function is replaced by
{ref}`api_spg_get_spacegroup_type_from_symmetry`.**

Return one of `hall_number` corresponding to a space-group type of the given
set of symmetry operations. When multiple `hall_number` exist for the
space-group type, the smallest one (the first description of the space-group
type in International Tables for Crystallography) is chosen. The definition of
`hall_number` is found at {ref}`dataset_spg_get_dataset_spacegroup_type` and
the corresponding space-group-type information is obtained through
{ref}`api_spg_spacegroup_type`.

This is expected to work well for the set of symmetry operations whose
distortion is small. The aim of making this feature is to find
space-group-type for the set of symmetry operations given by the other
source than spglib.

Note that the definition of `symprec` is
different from usual one, but is given in the fractional
coordinates and so it should be small like `1e-5`.

```c
int spg_get_hall_number_from_symmetry(const int rotation[][3][3],
                                       const double translation[][3],
                                       const int num_operations,
                                       const double symprec)
```
