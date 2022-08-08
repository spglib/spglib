# Summary of changes at release v2.0
## C functions and structures
### `spg_get_spacegroup_type`
- Add `hall_number` member at version 2.0
### `spg_get_spacegroup_type_from_symmetry`
- New at version 2.0
- Replacement of `spg_get_hall_number_from_symmetry`
### `spg_get_symmetry_with_site_tensors`
- Experimental: new at version 2.0
### `spg_get_magnetic_dataset`
- Experimental: new at version 2.0
### `spg_get_magnetic_symmetry_from_database`
- Experimental: new at version 2.0
### `spg_free_magnetic_dataset`
- Experimental: new at version 2.0
### `spg_get_magnetic_spacegroup_type_from_symmetry`
- Experimental: new at version 2.0
### `spg_get_hall_number_from_symmetry`
- Deprecated at version 2.0
- Will be removed at version 3.0
### `spgat_get_symmetry_with_collinear_spin`
- Deprecated at version 2.0
- Will be removed at version 3.0
- Will be replaced by `spgms_get_symmetry_with_collinear_spin`
### `spg_get_ir_reciprocal_mesh`
- Plan to make it deprecated at version 3.0
### `spg_get_stabilized_reciprocal_mesh`
- Plan to make it deprecated at version 3.0


## Python interface
### `get_symmetry`
- get_symmetry with `is_magnetic=True` is deprecated at version 2.0. Use
`get_magnetic_symmetry` for magnetic symmetry search.
### `get_spacegroup_type`
- `hall_number` member is added at version 2.0.
### `get_spacegroup_type_from_symmetry`
- New at version 2.0
- Replacement of get_hall_number_from_symmetry
### `get_magnetic_symmetry`
- Experimental: new at version 2.0
### `get_magnetic_symmetry_dataset`
- Experimental: new at version 2.0
### `get_magnetic_spacegroup_type`
- Experimental: new at version 2.0
### `get_magnetic_symmetry_from_database`
- Experimental: new at version 2.0
### `get_hall_number_from_symmetry`
- Deprecated at version 2.0
- Will be removed at version 3.0
- Will be replaced by `get_spacegroup_type_from_symmetry`
### `get_ir_reciprocal_mesh`
- Plan to make it deprecated at version 3.0
