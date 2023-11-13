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

```{testsetup}
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
`spglib.__version__` or {func}`spglib.get_version`.

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

### Symmetry tolerance (`symprec`, `angle_tolerance`, `mag_symprec`)

Distance tolerance in Cartesian coordinates to find crystal symmetry. For more
details, see {ref}`variables_symprec`, {ref}`variables_angle_tolerance`, and
{ref}`variables_mag_symprec`.

## API summary

### Version

```{autodoc2-summary}
  spglib.get_version
```

### Error

```{autodoc2-summary}
  spglib.get_error_message
```

### Space-group symmetry search

```{autodoc2-summary}
  spglib.get_symmetry
  spglib.get_symmetry_dataset
```

### Space-group type search

```{autodoc2-summary}
  spglib.get_spacegroup
```

### Standardization and finding primitive cell

```{autodoc2-summary}
  spglib.standardize_cell
  spglib.find_primitive
  spglib.refine_cell
```

### Space-group dataset access

```{autodoc2-summary}
  spglib.get_symmetry_from_database
  spglib.get_spacegroup_type
  spglib.get_spacegroup_type_from_symmetry
```

### Magnetic symmetry

```{autodoc2-summary}
  spglib.get_magnetic_symmetry
  spglib.get_magnetic_symmetry_dataset
  spglib.get_magnetic_spacegroup_type
  spglib.get_magnetic_symmetry_from_database
```

### Lattice reduction

```{autodoc2-summary}
  spglib.niggli_reduce
  spglib.delaunay_reduce
```

### Kpoints

```{autodoc2-summary}
  spglib.get_ir_reciprocal_mesh
```

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

### Deprecated

```{caution} Following functions are deprecated!
```

```{autodoc2-summary}
  spglib.get_hall_number_from_symmetry
```
