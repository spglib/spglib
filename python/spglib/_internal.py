"""These APIs are for the internal usage and development of Spglib itself."""
# Copyright (C) 2015 Atsushi Togo
# This file is part of spglib.
# SPDX-License-Identifier: BSD-3-Clause

from __future__ import annotations

import numpy as np
from numpy._typing import ArrayLike

from . import _spglib
from .error import _set_no_error, _set_or_throw_error
from .spg import SpgCell, SpglibDataset
from .utils import _expand_cell, _validate_symprec

__all__ = [
    "get_pointgroup",
    "get_symmetry_layerdataset",
    "get_layergroup",
    "get_grid_point_from_address",
    "get_stabilized_reciprocal_mesh",
    "get_grid_points_by_rotations",
    "get_BZ_grid_points_by_rotations",
    "relocate_BZ_grid_address",
]


def get_pointgroup(rotations: ArrayLike[np.intc]) -> tuple[str, int, np.ndarray] | None:
    """Return point group in international table symbol and number.

    The symbols are mapped to the numbers as follows:
    1   "1    "
    2   "-1   "
    3   "2    "
    4   "m    "
    5   "2/m  "
    6   "222  "
    7   "mm2  "
    8   "mmm  "
    9   "4    "
    10  "-4   "
    11  "4/m  "
    12  "422  "
    13  "4mm  "
    14  "-42m "
    15  "4/mmm"
    16  "3    "
    17  "-3   "
    18  "32   "
    19  "3m   "
    20  "-3m  "
    21  "6    "
    22  "-6   "
    23  "6/m  "
    24  "622  "
    25  "6mm  "
    26  "-62m "
    27  "6/mmm"
    28  "23   "
    29  "m-3  "
    30  "432  "
    31  "-43m "
    32  "m-3m "

    """
    _set_no_error()

    # (symbol, pointgroup_number, transformation_matrix)
    try:
        return _spglib.pointgroup(np.array(rotations, dtype="intc", order="C"))
    except Exception as exc:
        _set_or_throw_error(exc)
        return None


def get_symmetry_layerdataset(
    cell: SpgCell,
    aperiodic_dir: int = 2,
    symprec: float = 1e-5,
    _throw: bool = False,
) -> SpglibDataset | None:
    """TODO: Add comments."""
    _set_no_error(_throw)
    _validate_symprec(symprec)

    lattice, positions, numbers, _ = _expand_cell(cell)

    try:
        spg_ds = _spglib.layer_dataset(
            lattice,
            positions,
            numbers,
            int(aperiodic_dir),
            float(symprec),
        )
    except Exception as exc:
        _set_or_throw_error(exc, _throw)
        return None
    return SpglibDataset(**spg_ds)


def get_layergroup(
    cell: SpgCell,
    aperiodic_dir: int = 2,
    symprec: float = 1e-5,
) -> SpglibDataset | None:
    """Return layer group in ....

    If it fails, None is returned.

    """
    _set_no_error()
    _validate_symprec(symprec)

    try:
        return get_symmetry_layerdataset(
            cell,
            aperiodic_dir=aperiodic_dir,
            symprec=symprec,
            _throw=True,
        )
    except Exception as exc:
        _set_or_throw_error(exc)
        return None


def get_grid_point_from_address(
    grid_address: ArrayLike[np.intc],
    mesh: ArrayLike[np.intc],
) -> int | None:
    """Return grid point index by translating grid address."""
    _set_no_error()

    try:
        return _spglib.grid_point_from_address(
            np.array(grid_address, dtype="intc"),
            np.array(mesh, dtype="intc"),
        )
    except Exception as exc:
        _set_or_throw_error(exc)
        return None


def get_stabilized_reciprocal_mesh(
    mesh: ArrayLike[np.intc],
    rotations: ArrayLike[np.intc],
    is_shift: ArrayLike[np.intc] | None = None,
    is_time_reversal: bool = True,
    qpoints: ArrayLike[np.double] | None = None,
    is_dense: bool = False,
) -> tuple[np.ndarray, np.ndarray] | None:
    """Return k-point map to the irreducible k-points and k-point grid points.

    The symmetry is searched from the input rotation matrices in real space.

    Parameters
    ----------
    mesh : array_like
        Uniform sampling mesh numbers.
        dtype='intc', shape=(3,)
    rotations : array_like
        Rotation matrices with respect to real space basis vectors.
        dtype='intc', shape=(rotations, 3)
    is_shift : array_like
        [0, 0, 0] gives Gamma center mesh and value 1 gives  half mesh shift.
        dtype='intc', shape=(3,)
    is_time_reversal : bool
        Time reversal symmetry is included or not.
    qpoints : array_like
        q-points used as stabilizer(s) given in reciprocal space with respect
        to reciprocal basis vectors.
        dtype='double', shape=(qpoints ,3) or (3,)
    is_dense : bool, optional
        grid_mapping_table is returned with dtype='uintp' if True. Otherwise
        its dtype='intc'. Default is False.

    Returns
    -------
    grid_mapping_table : ndarray
        Grid point mapping table to ir-gird-points.
        dtype='intc', shape=(prod(mesh),)
    grid_address : ndarray
        Address of all grid points. Each address is given by three unsigned
        integers.
        dtype='intc', shape=(prod(mesh), 3)

    """
    _set_no_error()

    if is_dense:
        dtype = "uintp"
    else:
        dtype = "intc"
    mapping_table = np.zeros(np.prod(mesh), dtype=dtype)
    grid_address = np.zeros((np.prod(mesh), 3), dtype="intc")
    if is_shift is None:
        is_shift = [0, 0, 0]
    if qpoints is None:
        qpoints = np.array([[0, 0, 0]], dtype="double", order="C")
    else:
        qpoints = np.array(qpoints, dtype="double", order="C")
        if qpoints.shape == (3,):
            qpoints = np.array([qpoints], dtype="double", order="C")

    try:
        _spglib.stabilized_reciprocal_mesh(
            grid_address,
            mapping_table,
            np.array(mesh, dtype="intc"),
            np.array(is_shift, dtype="intc"),
            int(is_time_reversal * 1),
            np.array(rotations, dtype="intc", order="C"),
            qpoints,
        )
    except Exception as exc:
        _set_or_throw_error(exc)
        return None
    return mapping_table, grid_address


def get_grid_points_by_rotations(
    address_orig: ArrayLike[np.intc],
    reciprocal_rotations: ArrayLike[np.intc],
    mesh: ArrayLike[np.intc],
    is_shift: ArrayLike[np.intc] | None = None,
    is_dense: bool = False,
) -> np.ndarray | None:
    """Return grid points obtained after rotating input grid address.

    Parameters
    ----------
    address_orig : array_like
        Grid point address to be rotated.
        dtype='intc', shape=(3,)
    reciprocal_rotations : array_like
        Rotation matrices {R} with respect to reciprocal basis vectors.
        Defined by q'=Rq.
        dtype='intc', shape=(rotations, 3, 3)
    mesh : array_like
        dtype='intc', shape=(3,)
    is_shift : array_like, optional
        With (1) or without (0) half grid shifts with respect to grid intervals
        sampled along reciprocal basis vectors. Default is None, which
        gives [0, 0, 0].
    is_dense : bool, optional
        rot_grid_points is returned with dtype='uintp' if True. Otherwise
        its dtype='intc'. Default is False.

    Returns
    -------
    rot_grid_points : ndarray
        Grid points obtained after rotating input grid address
        dtype='intc' or 'uintp', shape=(rotations,)

    """
    _set_no_error()

    if is_shift is None:
        _is_shift = np.zeros(3, dtype="intc")
    else:
        _is_shift = np.array(is_shift, dtype="intc")

    rot_grid_points = np.zeros(len(reciprocal_rotations), dtype="uintp")
    try:
        _spglib.grid_points_by_rotations(
            rot_grid_points,
            np.array(address_orig, dtype="intc"),
            np.array(reciprocal_rotations, dtype="intc", order="C"),
            np.array(mesh, dtype="intc"),
            _is_shift,
        )
    except Exception as exc:
        _set_or_throw_error(exc)
        return None

    if is_dense:
        return rot_grid_points
    else:
        return np.array(rot_grid_points, dtype="intc")


def get_BZ_grid_points_by_rotations(
    address_orig: ArrayLike[np.intc],
    reciprocal_rotations: ArrayLike[np.intc],
    mesh: ArrayLike[np.intc],
    bz_map: ArrayLike[np.uintp],
    is_shift: ArrayLike[np.intc] | None = None,
    is_dense: bool = False,
) -> np.ndarray | None:
    """Return grid points obtained after rotating input grid address.

    Parameters
    ----------
    address_orig : array_like
        Grid point address to be rotated.
        dtype='intc', shape=(3,)
    reciprocal_rotations : array_like
        Rotation matrices {R} with respect to reciprocal basis vectors.
        Defined by q'=Rq.
        dtype='intc', shape=(rotations, 3, 3)
    mesh : array_like
        dtype='intc', shape=(3,)
    bz_map : array_like
        TODO
    is_shift : array_like, optional
        With (1) or without (0) half grid shifts with respect to grid intervals
        sampled along reciprocal basis vectors. Default is None, which
        gives [0, 0, 0].
    is_dense : bool, optional
        rot_grid_points is returned with dtype='uintp' if True. Otherwise
        its dtype='intc'. Default is False.

    Returns
    -------
    rot_grid_points : ndarray
        Grid points obtained after rotating input grid address
        dtype='intc' or 'uintp', shape=(rotations,)

    """
    _set_no_error()

    if is_shift is None:
        _is_shift = np.zeros(3, dtype="intc")
    else:
        _is_shift = np.array(is_shift, dtype="intc")

    if bz_map.dtype == "uintp" and bz_map.flags.c_contiguous:
        _bz_map = bz_map
    else:
        _bz_map = np.array(bz_map, dtype="uintp")

    rot_grid_points = np.zeros(len(reciprocal_rotations), dtype="uintp")
    try:
        _spglib.BZ_grid_points_by_rotations(
            rot_grid_points,
            np.array(address_orig, dtype="intc"),
            np.array(reciprocal_rotations, dtype="intc", order="C"),
            np.array(mesh, dtype="intc"),
            _is_shift,
            _bz_map,
        )
    except Exception as exc:
        _set_or_throw_error(exc)
        return None

    if is_dense:
        return rot_grid_points
    else:
        return np.array(rot_grid_points, dtype="intc")


def relocate_BZ_grid_address(
    grid_address: ArrayLike[np.intc],
    mesh: ArrayLike[np.intc],
    reciprocal_lattice: ArrayLike[np.double],  # column vectors
    is_shift: ArrayLike[np.intc] | None = None,
    is_dense: bool = False,
) -> tuple[np.ndarray, np.ndarray] | None:
    """Grid addresses are relocated to be inside first Brillouin zone.

    Number of ir-grid-points inside Brillouin zone is returned.
    It is assumed that the following arrays have the shapes of

    Note that the shape of grid_address is (prod(mesh), 3) and the
    addresses in grid_address are arranged to be in parallelepiped
    made of reciprocal basis vectors. The addresses in bz_grid_address
    are inside the first Brillouin zone or on its surface. Each
    address in grid_address is mapped to one of those in
    bz_grid_address by a reciprocal lattice vector (including zero
    vector) with keeping element order. For those inside first
    Brillouin zone, the mapping is one-to-one. For those on the first
    Brillouin zone surface, more than one addresses in bz_grid_address
    that are equivalent by the reciprocal lattice translations are
    mapped to one address in grid_address. In this case, those grid
    points except for one of them are appended to the tail of this array,
    for which bz_grid_address has the following data storing:

    .. code-block::

      |------------------array size of bz_grid_address-------------------------|
      |--those equivalent to grid_address--|--those on surface except for one--|
      |-----array size of grid_address-----|

    Number of grid points stored in bz_grid_address is returned.
    bz_map is used to recover grid point index expanded to include BZ
    surface from grid address. The grid point indices are mapped to
    (mesh[0] * 2) x (mesh[1] * 2) x (mesh[2] * 2) space (bz_map).

    """
    _set_no_error()

    if is_shift is None:
        _is_shift = np.zeros(3, dtype="intc")
    else:
        _is_shift = np.array(is_shift, dtype="intc")
    bz_grid_address = np.zeros((np.prod(np.add(mesh, 1)), 3), dtype="intc")
    bz_map = np.zeros(np.prod(np.multiply(mesh, 2)), dtype="uintp")
    try:
        num_bz_ir = _spglib.BZ_grid_address(
            bz_grid_address,
            bz_map,
            grid_address,
            np.array(mesh, dtype="intc"),
            np.array(reciprocal_lattice, dtype="double", order="C"),
            _is_shift,
        )
    except Exception as exc:
        _set_or_throw_error(exc)
        return None

    if is_dense:
        return bz_grid_address[:num_bz_ir], bz_map
    else:
        return bz_grid_address[:num_bz_ir], np.array(bz_map, dtype="intc")
