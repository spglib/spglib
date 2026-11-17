"""KPoints related operations."""
# Copyright (C) 2015 Atsushi Togo
# This file is part of spglib.
# SPDX-License-Identifier: BSD-3-Clause

from __future__ import annotations

import numpy as np
from numpy.typing import ArrayLike

from . import _spglib
from .error import _set_no_error, _set_or_throw_error
from .utils import Cell, _expand_cell, _validate_symprec

__all__ = [
    "get_ir_reciprocal_mesh",
]


def get_ir_reciprocal_mesh(
    mesh: ArrayLike[np.intc],
    cell: Cell,
    is_shift: ArrayLike[np.intc] | None = None,
    is_time_reversal: bool = True,
    symprec: float = 1e-5,
    is_dense: bool = False,
) -> tuple[np.ndarray, np.ndarray]:
    """Return k-points mesh and k-point map to the irreducible k-points.

    The symmetry is searched from the input cell.

    Parameters
    ----------
    mesh : array_like
        Uniform sampling mesh numbers.
        dtype='intc', shape=(3,)
    cell : spglib cell tuple
        Crystal structure.
    is_shift : array_like, optional
        [0, 0, 0] gives Gamma center mesh and value 1 gives half mesh shift.
        Default is None which equals to [0, 0, 0].
        dtype='intc', shape=(3,)
    is_time_reversal : bool, optional
        Whether time reversal symmetry is included or not. Default is True.
    symprec : float, optional
        Symmetry tolerance in distance. Default is 1e-5.
    is_dense : bool, optional
        grid_mapping_table is returned with dtype='uintp' if True. Otherwise
        its dtype='intc'. Default is False.

    Returns
    -------
    grid_mapping_table : ndarray
        Grid point mapping table to ir-gird-points.
        dtype='intc' or 'uintp', shape=(prod(mesh),)
    grid_address : ndarray
        Address of all grid points.
        dtype='intc', shape=(prod(mesh), 3)

    """
    _set_no_error()
    _validate_symprec(symprec)

    lattice, positions, numbers, _ = _expand_cell(cell)
    if lattice is None:
        return None

    if is_dense:
        dtype = "uintp"
    else:
        dtype = "intc"
    grid_mapping_table = np.zeros(np.prod(mesh), dtype=dtype)
    grid_address = np.zeros((np.prod(mesh), 3), dtype="intc")
    if is_shift is None:
        is_shift = [0, 0, 0]
    try:
        _spglib.ir_reciprocal_mesh(
            grid_address,
            grid_mapping_table,
            np.array(mesh, dtype="intc"),
            np.array(is_shift, dtype="intc"),
            int(is_time_reversal * 1),
            lattice,
            positions,
            numbers,
            float(symprec),
        )
    except Exception as exc:
        _set_or_throw_error(exc)
        return None
    return grid_mapping_table, grid_address
