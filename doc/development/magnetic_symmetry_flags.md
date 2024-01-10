# Flags to control action of magnetic symmetry operations

- Narrow functionality of `is_magnetic` and rename to `with_time_reversal`
- Add `is_axial`
- Retain `tensor_rank`

<!-- In this document, refer to `is_magnetic` in previous functions as `is_magnetic_old`. -->

## `spg_get_symmetry_with_site_tensors` (C API)

<!-- ### Previous behavior

- `tensor_rank=0`: $(\mathbf{R}, \mathbf{v}) \circ m = m$
    - `is_magnetic_old=true`: $1' \circ m = -m$
    - `is_magnetic_old=false`: $1' \circ m = m$
- `tensor_rank=1`: $(\mathbf{R}, \mathbf{v}) \circ \mathbf{m} = (\mathrm{det} \mathbf{R}) \mathbf{R} \mathbf{m}$
    - `is_magnetic_old=true`: $1' \circ \mathbf{m} = -\mathbf{m}$
    - `is_magnetic_old=false`: $1' \circ \mathbf{m} = \mathbf{m}$

### Proposed behavior -->

- `tensor_rank=0`
  - `with_time_reversal=true`: $1' \circ m = -m$
    - `is_axial=true`: $(\mathbf{R}, \mathbf{v}) \circ m = (\det\mathbf{R}) m$
    - `is_axial=false`: $(\mathbf{R}, \mathbf{v}) \circ m = m$
  - `with_time_reversal=false`: $1' \circ m = m$
    - `is_axial=true`: $(\mathbf{R}, \mathbf{v}) \circ m = (\det\mathbf{R}) m$
    - `is_axial=false`: $(\mathbf{R}, \mathbf{v}) \circ m = m$
- `tensor_rank=1`
  - `with_time_reversal=true`: $1' \circ \mathbf{m} = -\mathbf{m}$
    - `is_axial=true`: $(\mathbf{R}, \mathbf{v}) \circ \mathbf{m} = (\mathrm{det} \mathbf{R}) \mathbf{R} \mathbf{m}$
    - `is_axial=false`: $(\mathbf{R}, \mathbf{v}) \circ \mathbf{m} = \mathbf{R} \mathbf{m}$
  - `with_time_reversal=false`: $1' \circ \mathbf{m} = \mathbf{m}$
    - `is_axial=true`: $(\mathbf{R}, \mathbf{v}) \circ \mathbf{m} = (\mathrm{det} \mathbf{R}) \mathbf{R} \mathbf{m}$
    - `is_axial=false`: $(\mathbf{R}, \mathbf{v}) \circ \mathbf{m} = \mathbf{R} \mathbf{m}$

<!-- ### Correspondence

| `tensor_rank` | `is_magnetic_old` |   | `with_time_reversal` | `is_axial` |
| ------------- | ----------------- | - | -------------------- | ---------- |
| 0             | `true`            |   | `true`               | `false`    |
| 0             | `false`           |   | `false`              | `false`    |
| 1             | `true`            |   | `true`               | `true`     |
| 1             | `false`           |   | `false`              | `true`     | -->

<!-- ## `spg_get_magnetic_dataset` (C API)

- Previous behavior: call `spg_get_symmetry_with_site_tensors` with `is_magnetic_old=true` -->

## `get_symmetry` and `get_magnetic_symmetry` (Python API)

<!-- ### Previous behaviors

`get_symmetry((lattice, positions, numbers, magmoms), is_magnetic_old=True)`
- `magmoms = None` or not specified: other flags have no effect
- `magmoms.shape = (num_atoms, )`: call C API with `tensor_rank=0` and `is_magnetic_old`
- `magmoms.shape = (num_atoms, 3)`: call C API with `tensor_rank=1` and `is_magnetic_old`

### Proposed behavior -->

Deprecate `is_magnetic` in `get_symmetry`, and the functionality is replaced by
`get_magnetic_symmetry`.

Add new function `get_magnetic_symmetry((lattice, positions, numbers, magmoms), is_axial=None, with_time_reversal=True)`

- `magmoms = None` or not specified: other flags have no effect
- `magmoms.shape = (num_atoms, )`: call C API with `tensor_rank=0`, `is_axial`
  (default=false), and `with_time_reversal` (collinear magnetic like)
- `magmoms.shape = (num_atoms, 3)`: call C API with `tensor_rank=1`, `is_axial`
  (default=true), and `with_time_reversal` (non-collinear magnetic like)

## `get_magnetic_symmetry_dataset` (Python API)

<!-- ### Previous behaviors

`get_magnetic_symmetry_dataset((lattice, positions, numbers, magmoms), is_magnetic_old=True)`

- call `get_symmetry` with `is_magnetic_old`

### Proposed behavior  -->

`get_magnetic_symmetry_dataset((lattice, positions, numbers, magmoms), is_axial=None, with_time_reversal=True)`

- call `get_magnetic_symmetry` with `is_axial` and `with_time_reversal`
