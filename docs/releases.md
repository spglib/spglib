# Summary of releases

```{include} ../ChangeLog.md
---
start-after: <!-- SPHINX-START -->
end-before: <!-- SPHINX-END -->
---
```

## Release v2.0 and future plan

This release contains functions to search magnetic space group types which are
provided as experimental features. The behaviours of these magnetic related
functions (`*_magnetic_*`) with respect to the tolerance parameter (`symprec` or
{ref}`variables_mag_symprec`) may be changed in the future.

We are planning to separate irreducible k-points search from spglib. At spglib
version 4.0, those functions will be removed if we succeed to provide an
alternative package (hopefully with better functionalities) until then.

### C functions and structures

#### `SpglibSpacegroupType` structure

- Add `hall_number` member at version 2.0
- Used as return value of `spg_get_spacegroup_type` and `spg_get_spacegroup_type_from_symmetry`

#### `spg_get_spacegroup_type_from_symmetry`

- New at version 2.0
- Replacement of `spg_get_hall_number_from_symmetry`

#### `spg_get_symmetry_with_site_tensors`

- Experimental: new at version 2.0

#### `spg_get_magnetic_dataset`

- Experimental: new at version 2.0

#### `spg_get_magnetic_symmetry_from_database`

- Experimental: new at version 2.0

#### `spg_free_magnetic_dataset`

- Experimental: new at version 2.0

#### `spg_get_magnetic_spacegroup_type_from_symmetry`

- Experimental: new at version 2.0

#### `spg_get_hall_number_from_symmetry`

- Deprecated at version 2.0
- Will be removed at version 3.0

#### `spgat_get_symmetry_with_collinear_spin`

- Deprecated at version 2.0
- Will be removed at version 3.0
- Will be replaced by `spgms_get_symmetry_with_collinear_spin`

#### `spg_get_ir_reciprocal_mesh`

- Plan to make it deprecated at version 3.0

#### `spg_get_stabilized_reciprocal_mesh`

- Plan to make it deprecated at version 3.0

### Python interface

#### `get_symmetry`

- get_symmetry with `is_magnetic=True` is deprecated at version 2.0. Use
  `get_magnetic_symmetry` for magnetic symmetry search.
- As of version 2.0, the behavior of `get_symmetry` with zero magmoms (corresponding to type-II MSG) is changed.
  When all magmoms are zero, the newer `get_symmetry` returns the same spatial symmetry with `time_reversal=True` and `time-reversal=False`.
  This doubles the size of symmetry operations compared to the previous version.

#### `get_spacegroup_type`

- `hall_number` member is added at version 2.0.

#### `get_spacegroup_type_from_symmetry`

- New at version 2.0
- Replacement of get_hall_number_from_symmetry

#### `get_magnetic_symmetry`

- Experimental: new at version 2.0

#### `get_magnetic_symmetry_dataset`

- Experimental: new at version 2.0

#### `get_magnetic_spacegroup_type`

- Experimental: new at version 2.0

#### `get_magnetic_symmetry_from_database`

- Experimental: new at version 2.0

#### `get_hall_number_from_symmetry`

- Deprecated at version 2.0
- Will be removed at version 3.0
- Will be replaced by `get_spacegroup_type_from_symmetry`

#### `get_ir_reciprocal_mesh`

- Plan to make it deprecated at version 3.0
