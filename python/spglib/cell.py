"""Cell operations and standardizations."""

# Copyright (C) 2015 Atsushi Togo
# This file is part of spglib.
# SPDX-License-Identifier: BSD-3-Clause
from __future__ import annotations

import numpy as np

from . import _spglib
from .error import _set_no_error, _set_or_throw_error
from .spg import SpgCell
from .utils import _expand_cell, _validate_symprec

__all__ = [
    "standardize_cell",
    "find_primitive",
    "refine_cell",
]


def standardize_cell(
    cell: SpgCell,
    to_primitive: bool = False,
    no_idealize: bool = False,
    symprec: float = 1e-5,
    angle_tolerance: float = -1.0,
) -> SpgCell | None:
    """Return standardized cell. When the search failed, ``None`` is returned.

    Parameters
    ----------
    cell, symprec, angle_tolerance:
        See the docstring of get_symmetry.
    to_primitive : bool
        If True, the standardized primitive cell is created.
    no_idealize : bool
        If True, it is disabled to idealize lengths and angles of basis vectors
        and positions of atoms according to crystal symmetry.

    Returns
    -------
    The standardized unit cell or primitive cell is returned by a tuple of
    (lattice, positions, numbers). If it fails, None is returned.

    Notes
    -----
    .. versionadded:: 1.8

    Now :func:`refine_cell` and :func:`find_primitive` are shorthands of
    this method with combinations of these options.
    About the default choice of the setting, see the documentation of ``hall_number``
    argument of :func:`get_symmetry_dataset`. More detailed explanation is
    shown in the spglib (C-API) document.

    """
    _set_no_error()
    _validate_symprec(symprec)

    lattice, _positions, _numbers, _ = _expand_cell(cell)

    # Atomic positions have to be specified by scaled positions for spglib.
    num_atom = len(_positions)
    positions = np.zeros((num_atom * 4, 3), dtype="double", order="C")
    positions[:num_atom] = _positions
    numbers = np.zeros(num_atom * 4, dtype="intc")
    numbers[:num_atom] = _numbers
    try:
        num_atom_std = _spglib.standardize_cell(
            lattice,
            positions,
            numbers,
            num_atom,
            int(to_primitive * 1),
            int(no_idealize * 1),
            float(symprec),
            float(angle_tolerance),
        )
    except Exception as exc:
        _set_or_throw_error(exc)
        return None
    return (
        np.array(lattice.T, dtype="double", order="C"),
        np.array(positions[:num_atom_std], dtype="double", order="C"),
        np.array(numbers[:num_atom_std], dtype="intc"),
    )


def refine_cell(
    cell: SpgCell,
    symprec: float = 1e-5,
    angle_tolerance: float = -1.0,
) -> SpgCell | None:
    """Return refined cell. When the search failed, ``None`` is returned.

    The standardized unit cell is returned by a tuple of
    (lattice, positions, numbers).

    Notes
    -----
    .. versionchanged:: 1.8

    The detailed control of standardization of unit cell can be done using
    :func:`standardize_cell`.

    """
    _set_no_error()
    _validate_symprec(symprec)

    lattice, _positions, _numbers, _ = _expand_cell(cell)

    # Atomic positions have to be specified by scaled positions for spglib.
    num_atom = len(_positions)
    positions = np.zeros((num_atom * 4, 3), dtype="double", order="C")
    positions[:num_atom] = _positions
    numbers = np.zeros(num_atom * 4, dtype="intc")
    numbers[:num_atom] = _numbers
    try:
        num_atom_std = _spglib.refine_cell(
            lattice,
            positions,
            numbers,
            num_atom,
            float(symprec),
            float(angle_tolerance),
        )
    except Exception as exc:
        _set_or_throw_error(exc)
        return None
    return (
        np.array(lattice.T, dtype="double", order="C"),
        np.array(positions[:num_atom_std], dtype="double", order="C"),
        np.array(numbers[:num_atom_std], dtype="intc"),
    )


def find_primitive(
    cell: SpgCell,
    symprec: float = 1e-5,
    angle_tolerance: float = -1.0,
) -> SpgCell | None:
    """Primitive cell is searched in the input cell. If it fails, ``None`` is returned.

    The primitive cell is returned by a tuple of (lattice, positions, numbers).

    Notes
    -----
    .. versionchanged:: 1.8

    The detailed control of standardization of unit cell can be done using
    :func:`standardize_cell`.

    """
    _set_no_error()
    _validate_symprec(symprec)

    lattice, positions, numbers, _ = _expand_cell(cell)

    try:
        num_atom_prim = _spglib.primitive(
            lattice, positions, numbers, float(symprec), float(angle_tolerance)
        )
    except Exception as exc:
        _set_or_throw_error(exc)
        return None
    return (
        np.array(lattice.T, dtype="double", order="C"),
        np.array(positions[:num_atom_prim], dtype="double", order="C"),
        np.array(numbers[:num_atom_prim], dtype="intc"),
    )
