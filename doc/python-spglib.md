(python_spglib)=

# Spglib for Python

## Usage note

The python binding will first try to use your system installed version of spglib. This can
be controlled by setting an appropriate `LD_LIBRARY_PATH` (linux) or `DYLD_LIBRAY_PATH`
(macos) to the appropriate location of the `libsymspg.so` library file. If no suitable
version is found, it will default to using the bundled version.

## Installation

The main installation method we support are using [pip](#using-pip) and [conda](#using-conda).
Additionally, you can build and install [from source](#building-from-source).

The following pip and conda packages are made and maintained by
[Paweł T. Jochym](https://github.com/jochym), which is of great help
to keeping spglib handy and useful.

### Using pip

You can find all the spglib release versions on our [PyPI project page](https://pypi.org/project/spglib/).
To install it use the standard pip install commands:

```console
$ pip install spglib
```

Release candidates can be found at the equivalent [PyPI testing project page](https://pypi.org/project/spglib/).
You can test a specific release candidate locally using the following command:

```console
$ pip install -i https://test.pypi.org/simple/ spglib==2.1.0rc1
```

### Using conda

We also maintain a [conda package](https://anaconda.org/conda-forge/spglib) which you
can install:

```console
$ conda install -c conda-forge spglib
```

### Building from source

For this section we are assuming you have cloned or downloaded the spglib source
code, and we are working from the top-level directory.

```console
$ git clone https://github.com/spglib/spglib
$ cd spglib
```

To manually install spglib you will need `cmake` with any make tool like
`make`, a C compiler (e.g. gcc, clang), and python development files (python-dev),
including numpy ones.

There are two ways of building spglib with the python bindings using either a
python-centric or cmake-centric approach

#### Python controlled

This is the primary installation method suitable for production that we use to
distribute the spglib package.

```console
$ pip install .
```

All build configurations are defined in the top-level `pyproject.toml` file.
We are using [scikit-build-core](https://github.com/scikit-build/scikit-build-core)
to build both the main spglib C library and the python apis. You can thus
further configure your local installation, e.g. to use OpenMP:

```console
$ pip install . --config-settings=cmake.define.SPGLIB_USE_OMP=ON
```

Currently `scikit-build-core` does not support editable installs for development.

#### Cmake controlled

This method is not suitable for production, and it is only meant for quick development

```console
$ cmake -B ./build -DSPGLIB_WITH_Fortran=ON
$ cmake --build ./build
$ cmake --install ./build
```

This will detect and install the python manually in the currently activated virtual
environment's site-package folder (e.g. `venv/lib64/python3.11/site-packages`),
otherwise defaulting to the system (`/usr/lib64/python3.11/site-packages`). You may
edit this location by passing a `Python_INSTALL_DIR` option.

## Test

You may execute the python tests locally from the git repository root:

```console
$ pytest
```

## How to import spglib module

**Change in version 1.9.0!**

For versions 1.9.x or later:

```python
import spglib
```

For versions 1.8.x or before:

```python
from pyspglib import spglib
```

If the version is not sure:

```python
try:
    import spglib as spg
except ImportError:
    from pyspglib import spglib as spg
```

## Version number

In version 1.8.3 or later, the version number is obtained by
`spglib.__version__` or {ref}`method_get_version`.

## Example

Examples are found in [examples](https://github.com/spglib/spglib/tree/master/python/examples) directory.

(py_variables)=

## Variables

(py_variables_crystal_structure)=

### Crystal structure (`cell`)

A crystal structure is given by a **tuple**. This tuple format is
supported at version 1.9.1 or later.

The tuple format is shown as follows. There are three or four elements
in the tuple: `cell = (lattice, positions, numbers)` or `cell = (lattice, positions, numbers, magmoms)` where `magmoms` represents
magnetic moments on atoms and is optional.

Lattice parameters `lattice` are given by a 3x3 matrix with floating
point values, where $\mathbf{a}, \mathbf{b}, \mathbf{c}$ are
given as rows, which results in the transpose of the definition for
C-API ({ref}`variables_lattice`). Fractional atomic positions
`positions` are given by a Nx3 matrix with floating point values,
where N is the number of atoms. Numbers to distinguish atomic species
`numbers` are given by a list of N integers.
The magnetic moments `magmoms` can be specified with `get_magnetic_symmetry`.
`magmoms` is a list of N floating-point values for collinear cases and a list of Nx3 in **cartesian coordinates** for non-collinear cases.

```python
lattice = [[a_x, a_y, a_z],
            [b_x, b_y, b_z],
            [c_x, c_y, c_z]]
positions = [[a_1, b_1, c_1],
               [a_2, b_2, c_2],
               [a_3, b_3, c_3],
               ...]
numbers = [n_1, n_2, n_3, ...]
magmoms = [m_1, m_2, m_3, ...]  # Works with get_magnetic_symmetry for a collinear case
# magmoms = [[m_1x, m_1y, m_1z], ...]  # For a non-collinear case
```

For example, the crystal structure (`cell`) of L1$_{2}$-type AlNi$_{3}$ is:

```python
lattice = [[1.0, 0.0, 0.0],
            [0.0, 1.0, 0.0],
            [0.0, 0.0, 1.0]]
positions = [[0.0, 0.0, 0.0], # Al
            [0.5, 0.5, 0.0], # Ni
            [0.0, 0.5, 0.5], # Ni
            [0.5, 0.0, 0.5]] # Ni
numbers = [1, 2, 2, 2]        # Al, Ni, Ni, Ni
```

Version 1.9.5 or later: The methods that use the crystal structure
will return `None` when a crystal structure is not properly given.

#### ASE Atoms-like input is deprecated.

In the previous versions, ASE Atoms-like input was supported, but it
is deprecated. It is recommended to use the above tuple-style input
for the future support. `DeprecationWarning` is issued at version
1.10.0 or later.

The reason to make this feature deprecated is that ASE Atoms class is
too huge and users may expect spglib can understand its full
feature. However spglib actually collects only the following values
from the ASE Atoms-class instance::

```python
   lattice = cell.get_cell()
   positions = cell.get_scaled_positions()
   numbers = cell.get_atomic_numbers()
   magmoms = None
```

for which the corresponding code is written out of API and it is found at
[here](https://github.com/spglib/spglib/blob/e0851894ccdad1abb87d519b228d056128b56806/python/spglib/spglib.py#L737-L741).
Nevertheless the ASE Atoms-like input will be accepted for a while.  An
alternative Atoms class
([atoms.py](https://github.com/spglib/spglib/blob/master/python/examples/atoms.py))
that contains minimum set of methods is prepared in the
[examples](https://github.com/spglib/spglib/tree/master/python/examples)
directory. `get_symmetry` with collinear polarizations is not supported when
ASE Atoms-class instance.

### Symmetry tolerance (`symprec`, `angle_tolerance`, `mag_symprec`)

Distance tolerance in Cartesian coordinates to find crystal symmetry. For more
details, see {ref}`variables_symprec`, {ref}`variables_angle_tolerance`, and
{ref}`variables_mag_symprec`.

## Methods (version)

(method_get_version)=

### `get_version`

**New in version 1.8.3**

```python
version = get_version()
```

This returns version number of spglib by tuple with three numbers.

## Methods (error)

### `get_error_message`

**New in version 1.9.5**

**Be careful. This method is not thread safe, i.e., only safely usable
when calling one spglib method per process.**

This method is used to see roughly why spglib failed.

```python
error_message = get_error_message()
```

## Method (space-group symmetry search)

(py_method_get_symmetry)=

### `get_symmetry`

**`get_symmetry` with `is_magnetic=True` is deprecated at version 2.0. Use {ref}`py_get_magnetic_symmetry` for magnetic symmetry search.**

```python
symmetry = get_symmetry(cell, symprec=1e-5, angle_tolerance=-1.0, mag_symprec=-1.0)
```

Symmetry operations are obtained as a dictionary. The key `rotation`
contains a numpy array of integer, which is "number of symmetry
operations" x "3x3 matrices". The key `translation` contains a numpy
array of float, which is "number of symmetry operations" x
"vectors". The orders of the rotation matrices and the translation
vectors correspond with each other, e.g. , the second symmetry
operation is organized by the set of the second rotation matrix and second
translation vector in the respective arrays. Therefore a set of
symmetry operations may obtained by

```python
[(r, t) for r, t in zip(dataset['rotations'], dataset['translations'])]
```

The operations are given with respect to the fractional coordinates
(not for Cartesian coordinates). The rotation matrix and translation
vector are used as follows:

```
new_vector[3x1] = rotation[3x3] * vector[3x1] + translation[3x1]
```

The three values in the vector are given for the a, b, and c axes,
respectively. The key `equivalent_atoms` gives a mapping table of
atoms to symmetrically independent atoms. This is used to find
symmetrically equivalent atoms. The numbers contained are the indices
of atoms starting from 0, i.e., the first atom is numbered as 0, and
then 1, 2, 3, ... `np.unique(equivalent_atoms)` gives representative
symmetrically independent atoms. A list of atoms that are
symmetrically equivalent to some independent atom (here for example 1
is in `equivalent_atom`) is found by
`np.where(equivalent_atom=1)[0]`. When the search failed, `None`
is returned.

If `cell` is given as a tuple and magnetic moments are given
as the fourth element of this tuple, symmetry operations are searched
considering this freedom. In ASE Atoms-class object, this is not supported.

(py_method_get_symmetry_dataset)=

### `get_symmetry_dataset`

**At version 1.9.4, the member 'choice' is added.**

```python
dataset = get_symmetry_dataset(cell, symprec=1e-5, angle_tolerance=-1.0, hall_number=0)
```

The arguments are:

- `cell` and `symprec`: See {ref}`py_variables`.
- `angle_tolerance`: An experimental argument that controls angle
  tolerance between basis vectors. Normally it is not recommended to use
  this argument. See a bit more detail at
  {ref}`variables_angle_tolerance`.
- `hall_number` (see the definition of this number at
  {ref}`dataset_spg_get_dataset_spacegroup_type`): The argument to
  constrain the space-group-type search only for the Hall symbol
  corresponding to it. The mapping from Hall symbols to a
  space-group-type is the many-to-one mapping. Without specifying this
  option (i.e., in the case of `hall_number=0`), always the first one
  (the smallest serial number corresponding to the space-group-type in
  [list of space groups (Seto's web site)](https://yseto.net/?page_id=29%3E))
  among possible choices and settings is chosen as default. This
  argument is useful when the other choice (or setting) is expected to
  be hooked. This affects to the obtained values of `international`,
  `hall`, `hall_number`, `choice`, `transformation_matrix`,
  `origin shift`, `wyckoffs`, `std_lattice`, `std_positions`,
  `std_types` and `std_rotation_matrix`, but not to `rotations`
  and `translations` since the later set is defined with respect to
  the basis vectors of user's input (the `cell` argument).

`dataset` is a dictionary. Short explanations of the values of the
keys are shown below. More details are found at {ref}`spglib_dataset`.

- `number`: International space group number
- `international`: International short symbol
- `hall`: Hall symbol
- `hall_number`: Hall number. This number is used in
  {ref}`py_method_get_symmetry_from_database` and
  {ref}`py_method_get_spacegroup_type`.
- `choice`: Centring, origin, basis vector setting
- `transformation_matrix`: See the detail at
  {ref}`dataset_origin_shift_and_transformation`.
- `origin shift`: See the detail at
  {ref}`dataset_origin_shift_and_transformation`.
- `wyckoffs`: Wyckoff letters
- `site_symmetry_symbols`: Site-symmetry symbols (**experimental**)
- `equivalent_atoms`: Mapping table to equivalent atoms
- `crystallographic_orbits` : Mapping table to equivalent atoms (see
  {ref}`this <dataset_spg_get_dataset_site_symmetry>` for the difference
  between `equivalent_atoms` and `crystallographic_orbits`)
- `primitive_lattice` : Basis vectors of a primitive cell
- `mapping_to_primitive`: Mapping table to atoms in the primitive cell
- `rotations` and `translations`: Rotation matrices and
  translation vectors. See {ref}`py_method_get_symmetry` for more
  details.
- `pointgroup`: Symbol of the crystallographic point group in
  the Hermann–Mauguin notation.
- `std_lattice`, `std_positions`, `std_types`: Standardized
  crystal structure corresponding to a Hall symbol found. These are
  equivalently given in the array formats of `lattice`,
  `positions`, and `numbers` presented at
  {ref}`py_variables_crystal_structure`,
  respectively.
- `std_rotation_matrix`: See the detail at {ref}`dataset_idealized_cell`.
- `std_mapping_to_primitive`: Mapping table from atoms in the
  standardized crystal structure to the atoms in the primitive cell.

%    * `pointgrouop_number`: Serial number of the crystallographic point
%      group, which refers list of space groups (Seto’s web site)

When the search failed, `None` is returned.

## Method (space-group type search)

### `get_spacegroup`

```python
spacegroup = get_spacegroup(cell, symprec=1e-5, angle_tolerance=-1.0, symbol_type=0)
```

International space group short symbol and number are obtained as a
string. With `symbol_type=1`, Schoenflies symbol is given instead of
international symbol.

## Method (standardization and finding primitive)

### `standardize_cell`

**New in version 1.8.x**

```python
lattice, scaled_positions, numbers = standardize_cell(bulk, to_primitive=False, no_idealize=False, symprec=1e-5, angle_tolerance=-1.0)
```

`to_primitive=True` is used to create the standardized primitive
cell, and `no_idealize=True` disables to idealize lengths and angles
of basis vectors and positions of atoms according to crystal
symmetry. Now `refine_cell` and `find_primitive` are shorthands of
this method with combinations of these options. When the search
failed, `None` is returned.  About the default choice
of the setting, see the documentation of `hall_number` argument of
{ref}`py_method_get_symmetry_dataset`.  More detailed explanation is
shown in the spglib (C-API) document.

### `find_primitive`

**Behaviour changed in version 1.8.x**

```python
lattice, scaled_positions, numbers = find_primitive(cell, symprec=1e-5, angle_tolerance=-1.0)
```

When a primitive cell is found, lattice parameters (a 3x3 numpy array),
scaled positions (a numpy array of \[number_of_atoms,3\]), and atomic
numbers (a 1D numpy array) is returned. When the search failed,
`None` is returned.

The detailed control of standardization of unit cell can be done using
`standardize_cell`.

### `refine_cell`

**Behaviour changed in version 1.8.x**

```python
lattice, scaled_positions, numbers = refine_cell(cell, symprec=1e-5, angle_tolerance=-1.0)
```

Standardized crystal structure is obtained as a tuple of lattice (a
3x3 numpy array), atomic scaled positions (a numpy array of
\[number_of_atoms,3\]), and atomic numbers (a 1D numpy array) that are
symmetrized following space group type. When the search failed,
`None` is returned. About the default choice of the setting, see the
documentation of `hall_number` argument of
{ref}`py_method_get_symmetry_dataset`.

The detailed control of standardization of unit cell is achieved using
`standardize_cell`.

## Method (space-group dataset access)

(py_method_get_symmetry_from_database)=

### `get_symmetry_from_database`

```python
symmetry = get_symmetry_from_database(hall_number)
```

A set of crystallographic symmetry operations corresponding to
`hall_number` is returned by a dictionary where rotation parts and
translation parts are accessed by the keys `rotations` and
`translations`, respectively. The definition of `hall_number` is
found at {ref}`dataset_spg_get_dataset_spacegroup_type`.

When something wrong happened, `None` is returned.

(py_method_get_spacegroup_type)=

### `get_spacegroup_type`

- **New at version 1.9.4**
- **`hall_number` member is added at version 2.0.**

```python
spacegroup_type = get_spacegroup_type(hall_number)
```

This function allows to directly access to the space-group-type
database in spglib (spg_database.c). A dictionary is returned. To
specify the space group type with a specific choice, `hall_number`
is used. The definition of `hall_number` is found at
{ref}`dataset_spg_get_dataset_spacegroup_type`. The keys of the returned
dictionary is as follows:

```
number
international_short
international_full
international
schoenflies
hall_symbol
hall_number
choice
pointgroup_schoenflies
pointgroup_international
arithmetic_crystal_class_number
arithmetic_crystal_class_symbol
```

Here `spacegroup_type['international_short']` is equivalent to
`dataset['international']` of `get_symmetry_dataset`,
`spacegroup_type['hall_symbol']` is equivalent to
`dataset['hall']` of `get_symmetry_dataset`, and
`spacegroup_type['pointgroup_international']` is equivalent to
`dataset['pointgroup_symbol']` of `get_symmetry_dataset`.

When something wrong happened, `None` is returned.

(py_method_get_spacegroup_type_from_symmetry)=

### `get_spacegroup_type_from_symmetry`

**New at version 2.0**

```python
spacegroup_type = get_spacegroup_type_from_symmetry(rotations, translations, lattice=None, symprec=1e-5)
```

Return space-group type information (see {ref}`py_method_get_spacegroup_type`)
from symmetry operations. This is the replacement of
`get_hall_number_from_symmetry` (deprecated at v2.0).

This is expected to work well for the set of symmetry operations whose
distortion is small. The aim of making this feature is to find space-group-type
for the set of symmetry operations given by the other source than spglib. The
parameter `lattice` is used as the distance measure for `symprec`. If this
is not given, the cubic basis vector whose lengths are one is used.

## Methods (magnetic symmetry)

(py_get_magnetic_symmetry)=

### `get_magnetic_symmetry`

**Experimental: new at version 2.0**

Find magnetic symmetry operations from a crystal structure and site tensors

```python
symmetry = get_magnetic_symmetry(
    cell,
    symprec=1e-5,
    angle_tolerance=-1.0,
    mag_symprec=-1.0,
    is_axial=None,
    with_time_reversal=True,
)
```

Arguments

- `cell`: See {ref}`py_variables_crystal_structure`
- `symprec`, `angle_tolerance`: See {ref}`py_method_get_symmetry`
- `mag_symprec`:
  Tolerance for magnetic symmetry search in the unit of `magmoms`.
  If not specified, use the same value as `symprec`.
- `is_axial`
  Set `is_axial=True` if `magmoms` does not change their sign by improper rotations.
  If not specified, set `is_axial=False` when `magmoms.shape==(num_atoms, )`, and
  set `is_axial=True` when `magmoms.shape==(num_atoms, 3)`. These default settings
  correspond to collinear and non-collinear spins.
- `with_time_reversal`
  Set `with_time_reversal=True` if `magmoms` change their sign by time-reversal
  operations.

Returned `symmetry` is a dictionary with the following keys:

- `rotations` : Rotation (matrix) parts of symmetry operations
- `translations`: Translation (vector) parts of symmetry operations
- `time_reversals`:
  Time reversal part of magnetic symmetry operations.
  `True` indicates time reversal operation, and `False` indicates
  an ordinary operation.
- `equivalent_atoms`

### `get_magnetic_symmetry_dataset`

**Experimental: new at version 2.0**

Search magnetic symmetry dataset from an input cell.

```python
dataset = get_magnetic_symmetry_dataset(
    cell,
    is_axial=None,
    symprec=1e-5,
    angle_tolerance=-1.0,
    mag_symprec=-1.0,
)
```

Arguments

- `cell`: See {ref}`py_variables_crystal_structure`
- `symprec`, `angle_tolerance`: See {ref}`py_method_get_symmetry`
- `mag_symprec`: See {ref}`py_get_magnetic_symmetry`
- `is_axial`: See {ref}`py_get_magnetic_symmetry`

If successful, returned `dataset` is a dictionary.
The description of its keys is given at {ref}`magnetic_spglib_dataset`.
If the magnetic symmetry search failed, `dataset` is `None`.

### `get_magnetic_spacegroup_type`

**Experimental: new at version 2.0**

Translate UNI number to magnetic space group type information.

```python
magnetic_spacegroup_type = get_magnetic_spacegroup_type(uni_number):
```

The returned `magnetic_spacegroup_type` is a dictionary with

```
uni_number
litvin_number
bns_number
og_number
number
type
```

See {ref}`api_get_magnetic_spacegroup_type` for these descriptions.

### `get_magnetic_symmetry_from_database`

**Experimental: new at version 2.0**

```python
symmetry = get_magnetic_symmetry_from_database(uni_number, hall_number=0)
```

Return magnetic symmetry operations corresponding to `uni_number` between 1 and 1651.
Optionally alternative settings can be specified with `hall_number`.

## Methods (lattice reduction)

### `niggli_reduce`

**New at version 1.9.4**

```python
niggli_lattice = niggli_reduce(lattice, eps=1e-5)
```

Niggli reduction is achieved using this method. The algorithm detail
is found at https://atztogo.github.io/niggli/ and the references are
there in. Original basis vectors are stored in `lattice` and the
Niggli reduced basis vectors are given in `niggli_lattice`. The
format of basis vectors are found at
{ref}`py_variables_crystal_structure`. `esp` is the tolerance
parameter, but unlike `symprec` the unit is not a length. This is
used to check if difference of norms of two basis vectors is close to
zero or not and if two basis vectors are orthogonal by the value of
dot product being close to zero or not.  The detail is shown at
<https://atztogo.github.io/niggli/>.

When the search failed, `None` is returned.

The transformation from original basis vectors $( \mathbf{a} \; \mathbf{b} \; \mathbf{c} )$ to final basis vectors $( \mathbf{a}' \; \mathbf{b}' \; \mathbf{c}' )$ is achieved by linear
combination of basis vectors with integer coefficients without
rotating coordinates. Therefore the transformation matrix is obtained
by $\boldsymbol{P} = ( \mathbf{a} \; \mathbf{b} \; \mathbf{c} ) ( \mathbf{a}' \; \mathbf{b}' \; \mathbf{c}' )^{-1}$ and the matrix
elements have to be almost integers.

### `delaunay_reduce`

**New at version 1.9.4**

```python
delaunay_lattice = delaunay_reduce(lattice, eps=1e-5)
```

Delaunay reduction is achieved using this method. The algorithm is
found in the international tables for crystallography
volume A. Original basis vectors are stored in `lattice` and the
Delaunay reduced basis vectors are given in `delaunay_lattice`,
where the format of basis vectors are shown in
{ref}`py_variables_crystal_structure`. `esp` is the tolerance
parameter, but unlike `symprec` the unit is not a length. This is
used as the criterion if volume is close to zero or not and if two
basis vectors are orthogonal by the value of dot product being close
to zero or not.

When the search failed, `None` is returned.

The transformation from original basis vectors $( \mathbf{a} \; \mathbf{b} \; \mathbf{c} )$ to final basis vectors $( \mathbf{a}' \; \mathbf{b}' \; \mathbf{c}' )$ is achieved by linear
combination of basis vectors with integer coefficients without
rotating coordinates. Therefore the transformation matrix is obtained
by $\boldsymbol{P} = ( \mathbf{a} \; \mathbf{b} \; \mathbf{c} ) ( \mathbf{a}' \; \mathbf{b}' \; \mathbf{c}' )^{-1}$ and the matrix
elements have to be almost integers.

## Methods (kpoints)

### `get_ir_reciprocal_mesh`

```python
mapping, grid = get_ir_reciprocal_mesh(mesh, cell, is_shift=[0, 0, 0])
```

Irreducible k-points are obtained from a sampling mesh of k-points.
`mesh` is given by three integers by array and specifies mesh
numbers along reciprocal primitive axis. `is_shift` is given by the
three integers by array. When `is_shift` is set for each reciprocal
primitive axis, the mesh is shifted along the axis in half of adjacent
mesh points irrespective of the mesh numbers. When the value is not 0,
`is_shift` is set.

`mapping` and `grid` are returned. `grid` gives the mesh points in
fractional coordinates in reciprocal space. `mapping` gives mapping to
the irreducible k-point indices that are obtained by

```python
np.unique(mapping)
```

Here `np` means the numpy module. The grid point is accessed by
`grid[index]`.

When the search failed, `None` is returned.

An example is shown below:

```python
import numpy as np
import spglib

lattice = np.array([[0.0, 0.5, 0.5],
                    [0.5, 0.0, 0.5],
                    [0.5, 0.5, 0.0]]) * 5.4
positions = [[0.875, 0.875, 0.875],
            [0.125, 0.125, 0.125]]
numbers= [1,] * 2
cell = (lattice, positions, numbers)
print(spglib.get_spacegroup(cell, symprec=1e-5))
mesh = [8, 8, 8]

#
# Gamma centre mesh
#
mapping, grid = spglib.get_ir_reciprocal_mesh(mesh, cell, is_shift=[0, 0, 0])

# All k-points and mapping to ir-grid points
for i, (ir_gp_id, gp) in enumerate(zip(mapping, grid)):
    print("%3d ->%3d %s" % (i, ir_gp_id, gp.astype(float) / mesh))

# Irreducible k-points
print("Number of ir-kpoints: %d" % len(np.unique(mapping)))
print(grid[np.unique(mapping)] / np.array(mesh, dtype=float))

#
# With shift
#
mapping, grid = spglib.get_ir_reciprocal_mesh(mesh, cell, is_shift=[1, 1, 1])

# All k-points and mapping to ir-grid points
for i, (ir_gp_id, gp) in enumerate(zip(mapping, grid)):
    print("%3d ->%3d %s" % (i, ir_gp_id, (gp + [0.5, 0.5, 0.5]) / mesh))

# Irreducible k-points
print("Number of ir-kpoints: %d" % len(np.unique(mapping)))
print((grid[np.unique(mapping)] + [0.5, 0.5, 0.5]) / mesh)
```

## Method (deprecated)

### `get_hall_number_from_symmetry`

**Deprecated at version 2.0. This function is replaced by
{ref}`py_method_get_spacegroup_type_from_symmetry`.**

Return one of `hall_number` corresponding to a space-group type of the given
set of symmetry operations. When multiple `hall_number` exist for the
space-group type, the smallest one (the first description of the space-group
type in International Tables for Crystallography) is chosen. The definition of
`hall_number` is found at {ref}`dataset_spg_get_dataset_spacegroup_type` and
the corresponding space-group-type information is obtained through
{ref}`py_method_get_spacegroup_type`.

This is expected to work well for the set of symmetry operations whose
distortion is small. The aim of making this feature is to find
space-group-type for the set of symmetry operations given by the other
source than spglib.

Note that the definition of `symprec` is
different from usual one, but is given in the fractional
coordinates and so it should be small like `1e-5`.

```python
get_hall_number_from_symmetry(rotations, translations, symprec=1e-5)
```
