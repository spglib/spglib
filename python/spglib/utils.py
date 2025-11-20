"""Various base and utility definitions."""
# Copyright (C) 2015 Atsushi Togo
# This file is part of spglib.
# SPDX-License-Identifier: BSD-3-Clause

from __future__ import annotations

import dataclasses
import typing
import warnings
from collections.abc import Mapping, Sequence
from typing import Union

import numpy as np

import spglib

from . import _spglib
from ._compat.typing import TypeAlias
from ._compat.warnings import deprecated
from .error import SpglibError, _set_no_error

if typing.TYPE_CHECKING:
    from collections.abc import Iterator
    from typing import Any

__all__ = [
    "Lattice",
    "Positions",
    "Numbers",
    "Magmoms",
    "Cell",
    "get_version",
    "spg_get_version",
    "spg_get_version_full",
    "spg_get_commit",
]

warnings.filterwarnings(
    "module", category=DeprecationWarning, message=r"dict interface.*"
)


Lattice: TypeAlias = Sequence[Sequence[float]]
"""Basis vectors in ``(3, 3)`` shape."""
Positions: TypeAlias = Sequence[Sequence[float]]
"""Point coordinates of atoms in ``(N, 3)`` shape."""
Numbers: TypeAlias = Sequence[int]
"""Integer numbers for identifying atoms in ``(N,)`` shape."""
Magmoms: TypeAlias = Union[Sequence[float], Sequence[Sequence[float]]]
"""Magnetic moments of atoms in either ``(N,)`` or ``(N, 3)`` shape."""

# Compatibility alias for some deprecated interfaces
Cell: TypeAlias = Union["spglib.spg.SpgCell", "spglib.msg.MsgCell"]
"""Either :py:type:`spglib.spg.SpgCell` or :py:type:`spglib.msg.MsgCell`."""


@dataclasses.dataclass(eq=True, frozen=True)
class DictInterface(Mapping[str, "Any"]):
    """Base class for dataclass with dict interface.

    .. versionadded:: 2.5.0
    .. deprecated:: 2.5.0
        Dict-like interface (``obj['field']``) are deprecated.
        Please use attribute interface instead (``obj.field``)
    """

    @deprecated("dict interface is deprecated. Use attribute interface instead")
    def __getitem__(self, key: str) -> Any:
        """Return the value of the key."""
        return dataclasses.asdict(self)[key]

    def __len__(self) -> int:
        """Return the number of fields."""
        return len(dataclasses.fields(self))

    def __iter__(self) -> Iterator[str]:
        """Return an iterator over the keys."""
        return iter(dataclasses.asdict(self))


@deprecated("Use __version__ or spg_get_version instead")
def get_version() -> tuple[int, int, int]:
    """Return version number of spglib with tuple of three numbers.

    .. versionadded:: 1.8.3
    .. deprecated:: 2.3.0
        Use :py:func:`spg_get_version` and ``spglib.__version__`` instead
    """
    _set_no_error()
    return _spglib.version_tuple()


def spg_get_version() -> str:
    """Get the X.Y.Z version of the detected spglib C library.

    .. versionadded:: 2.3.0

    :return: version string
    """
    _set_no_error()
    return _spglib.version_string()


def spg_get_version_full() -> str:
    """Get the full version of the detected spglib C library.

    .. versionadded:: 2.3.0

    :return: full version string
    """
    _set_no_error()
    return _spglib.version_full()


def spg_get_commit() -> str:
    """Get the commit of the detected spglib C library.

    .. versionadded:: 2.3.0

    :return: commit string
    """
    _set_no_error()
    return _spglib.commit()


def _expand_cell(
    cell: Cell,
) -> tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray | None]:
    try:
        lattice = np.array(np.transpose(cell[0]), dtype="double", order="C")
        positions = np.array(cell[1], dtype="double", order="C")
        numbers = np.array(cell[2], dtype="intc")
        if len(cell) == 4:
            magmoms = np.array(cell[3], order="C", dtype="double")
        elif len(cell) == 3:
            magmoms = None
        else:
            raise TypeError("cell has to be a tuple of 3 or 4 elements.")

        # Sanity check
        if lattice.shape != (3, 3):
            raise TypeError("lattice has to be a (3, 3) array.")
        if not (positions.ndim == 2 and positions.shape[1] == 3):
            raise TypeError("positions has to be a (num_atoms, 3) array.")
        num_atoms = positions.shape[0]
        if numbers.ndim != 1:
            raise TypeError("numbers has to be a (num_atoms,) array.")
        if len(numbers) != num_atoms:
            raise TypeError(
                "numbers has to have the same number of atoms as positions."
            )
        if magmoms is not None:
            if len(magmoms) != num_atoms:
                raise TypeError(
                    "magmoms has to have the same number of atoms as positions."
                )
            if magmoms.ndim == 1:
                # collinear
                pass
            elif magmoms.ndim == 2:
                # non-collinear
                if magmoms.shape[1] != 3:
                    raise TypeError(
                        "non-collinear magmoms has to be a (num_atoms, 3) array."
                    )
            else:
                raise TypeError("magmoms has to be a 1D or 2D array.")
    except Exception as exc:
        # Note: these will eventually be moved to the C++ side
        # For now we just recast them to SpglibError
        raise SpglibError(f"Generic Spglib error:\n{exc}") from exc

    return (lattice, positions, numbers, magmoms)
