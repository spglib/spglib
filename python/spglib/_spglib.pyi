import typing

import numpy as np

from ._compat.typing import TypeAlias
from ._compat.warnings import deprecated

_data_list: TypeAlias = list[typing.Any]

# Note: Many of these interfaces do not match the _spglib.c quite correctly.
#   This will resolve itself when using pybind and C++ interface

@deprecated("Not used")
def error_out() -> None: ...
@deprecated("Use __version__ or spg_get_version instead")
def version_tuple() -> tuple[int, int, int]: ...
def version_string() -> str: ...
def version_full() -> str: ...
def commit() -> str: ...
def dataset(
    lattice: np.ndarray,
    positions: np.ndarray,
    atom_types: np.ndarray,
    hall_number: int,
    symprec: float,
    angle_tolerance: float,
) -> _data_list | None: ...
def layer_dataset(
    lattice: np.ndarray,
    positions: np.ndarray,
    atom_types: np.ndarray,
    aperiodic_dir: int,
    symprec: float,
) -> _data_list | None: ...
def magnetic_dataset(
    lattice: np.ndarray,
    positions: np.ndarray,
    atom_types: np.ndarray,
    magmoms: np.ndarray,
    tensor_rank: int,
    is_axial: bool,
    symprec: float,
    angle_tolerance: float,
    mag_symprec: float,
) -> _data_list | None: ...
def spacegroup_type(hall_number: int) -> _data_list | None: ...
def spacegroup_type_from_symmetry(
    rotations: np.ndarray, translations: np.ndarray, lattice: np.ndarray, symprec: float
) -> _data_list | None: ...
def magnetic_spacegroup_type(uni_number: int) -> _data_list | None: ...
def magnetic_spacegroup_type_from_symmetry(
    rotations: np.ndarray,
    translations: np.ndarray,
    time_reversals: np.ndarray,
    lattice: np.ndarray,
    symprec: float,
) -> _data_list | None: ...
def symmetry_from_database(
    rotations: np.ndarray,
    translations: np.ndarray,
    hall_number: int,
) -> int | None: ...
def magnetic_symmetry_from_database(
    rotations: np.ndarray,
    translations: np.ndarray,
    time_reversals: np.ndarray,
    uni_number: int,
    hall_number: int,
) -> int | None: ...
def pointgroup(rotations: np.ndarray) -> tuple[str, int, np.ndarray] | None: ...
def standardize_cell(
    lattice: np.ndarray,
    positions: np.ndarray,
    atom_types: np.ndarray,
    num_atom: int,
    to_primitive: int,
    no_idealize: int,
    symprec: float,
    angle_tolerance: float,
) -> int | None: ...
def refine_cell(
    lattice: np.ndarray,
    positions: np.ndarray,
    atom_types: np.ndarray,
    num_atom: int,
    symprec: float,
    angle_tolerance: float,
) -> int | None: ...
def symmetry(
    rotations: np.ndarray,
    translations: np.ndarray,
    lattice: np.ndarray,
    positions: np.ndarray,
    atom_types: np.ndarray,
    symprec: float,
    angle_tolerance: float,
) -> int | None: ...
@deprecated("Not used")
def symmetry_with_collinear_spin(
    rotations: np.ndarray,
    translations: np.ndarray,
    equiv_atoms: np.ndarray,
    lattice: np.ndarray,
    positions: np.ndarray,
    atom_types: np.ndarray,
    magmoms: np.ndarray,
    symprec: float,
    angle_tolerance: float,
) -> int | None: ...
def symmetry_with_site_tensors(
    rotations: np.ndarray,
    translations: np.ndarray,
    equiv_atoms: np.ndarray,
    primitive_lattice: np.ndarray,
    spin_flips: np.ndarray,
    lattice: np.ndarray,
    positions: np.ndarray,
    atom_types: np.ndarray,
    tensors: np.ndarray,
    with_time_reversal: int,
    is_axial: int,
    symprec: float,
    angle_tolerance: float,
    mag_symprec: float,
) -> int | None: ...
def primitive(
    lattice: np.ndarray,
    positions: np.ndarray,
    atom_types: np.ndarray,
    symprec: float,
    angle_tolerance: float,
) -> int | None: ...
def grid_point_from_address(
    grid_address: np.ndarray,
    mesh: np.ndarray,
) -> int | None: ...
def ir_reciprocal_mesh(
    grid_address: np.ndarray,
    grid_mapping_table: np.ndarray,
    mesh: np.ndarray,
    is_shift: np.ndarray,
    is_time_reversal: int,
    lattice: np.ndarray,
    positions: np.ndarray,
    atom_types: np.ndarray,
    symprec: float,
) -> int | None: ...
def stabilized_reciprocal_mesh(
    grid_address: np.ndarray,
    grid_mapping_table: np.ndarray,
    mesh: np.ndarray,
    is_shift: np.ndarray,
    is_time_reversal: int,
    rotations: np.ndarray,
    qpoints: np.ndarray,
) -> int | None: ...
def grid_points_by_rotations(
    rot_grid_points: np.ndarray,
    address_orig: np.ndarray,
    rot_reciprocal: np.ndarray,
    mesh: np.ndarray,
    is_shift: np.ndarray,
) -> None: ...
def BZ_grid_points_by_rotations(
    rot_grid_points: np.ndarray,
    address_orig: np.ndarray,
    rot_reciprocal: np.ndarray,
    mesh: np.ndarray,
    is_shift: np.ndarray,
    bz_map: np.ndarray,
) -> None: ...
def BZ_grid_address(
    bz_grid_address: np.ndarray,
    bz_map: np.ndarray,
    grid_address: np.ndarray,
    mesh: np.ndarray,
    reciprocal_lattice: np.ndarray,
    is_shift: np.ndarray,
) -> int | None: ...
def delaunay_reduce(
    lattice: np.ndarray,
    symprec: float,
) -> int | None: ...
def niggli_reduce(
    lattice: np.ndarray,
    eps: float,
) -> int | None: ...
def hall_number_from_symmetry(
    rotations: np.ndarray,
    translations: np.ndarray,
    symprec: float,
) -> int | None: ...
def error_message() -> str: ...
