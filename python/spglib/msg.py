"""MagneticSpaceGroup operations."""
# Copyright (C) 2015 Atsushi Togo
# This file is part of spglib.
# SPDX-License-Identifier: BSD-3-Clause

from __future__ import annotations

import dataclasses
from typing import Any

import numpy as np
from numpy.typing import ArrayLike, NDArray

from . import _spglib
from ._compat.typing import TypeAlias
from .error import _set_no_error, _set_or_throw_error
from .utils import DictInterface, Lattice, Magmoms, Numbers, Positions, _expand_cell

__all__ = [
    "MsgCell",
    "SpglibMagneticDataset",
    "MagneticSpaceGroupType",
    "get_magnetic_spacegroup_type",
    "get_magnetic_spacegroup_type_from_symmetry",
    "get_magnetic_symmetry",
    "get_magnetic_symmetry_dataset",
    "get_magnetic_symmetry_from_database",
]

MsgCell: TypeAlias = tuple[Lattice, Positions, Numbers, Magmoms]
"""
Magnetic crystal structure represented by a tuple of
(:py:type:`spglib.utils.Lattice`, :py:type:`spglib.utils.Positions`,
:py:type:`spglib.utils.Numbers`, :py:type:`spglib.utils.Magmoms`).
"""


@dataclasses.dataclass(eq=False, frozen=True)
class SpglibMagneticDataset(DictInterface):
    """Spglib magnetic dataset information.

    See :ref:`magnetic_spglib_dataset` in detail.

    .. versionadded:: 2.5.0
    """

    # Magnetic space-group type
    uni_number: int
    """UNI number between 1 to 1651"""
    msg_type: int
    """Magnetic space groups (MSG) is classified by its family space group (FSG) and
    maximal space subgroup (XSG).

    FSG is a non-magnetic space group obtained by ignoring time-reversal term in MSG.
    XSG is a space group obtained by picking out non time-reversal operations in MSG.

    - msg_type==1 (type-I):
        MSG, XSG, FSG are all isomorphic.
    - msg_type==2 (type-II):
        XSG and FSG are isomorphic, and MSG is generated from XSG and pure time reversal
        operations.
    - msg_type==3 (type-III):
        XSG is a proper subgroup of MSG with isomorphic translational subgroups.
    - msg_type==4 (type-IV):
        XSG is a proper subgroup of MSG with isomorphic point group.
    """
    hall_number: int
    """For type-I, II, III, Hall number of FSG; for type-IV, that of XSG"""
    tensor_rank: int
    """Rank of magmoms."""

    # Magnetic symmetry operations
    n_operations: int
    """Number of magnetic symmetry operations"""
    rotations: NDArray[np.intc]
    """Rotation (matrix) parts of symmetry operations

    shape: (n_operations, 3, 3)
    """
    translations: NDArray[np.double]
    """Translation (vector) parts of symmetry operations

    shape: (n_operations, 3)
    """
    time_reversals: NDArray[np.intc]
    """Time reversal part of magnetic symmetry operations.

    True indicates time reversal operation, and False indicates an ordinary operation.

    shape: (n_operations, )
    """

    # Equivalent atoms
    n_atoms: int
    """Number of atoms in the input cell"""
    equivalent_atoms: NDArray[np.intc]
    """Symmetrically equivalent atoms, where 'symmetrically' means found symmetry
    operations.
    """

    # Transformation to standardized setting
    transformation_matrix: NDArray[np.intc]
    """Transformation matrix from input lattice to standardized

    shape: (3, 3)
    """
    origin_shift: NDArray[np.double]
    """Origin shift from standardized to input origin

    shape: (3, )
    """

    # Standardized crystal structure
    n_std_atoms: int
    """Number of atoms in standardized unit cell"""
    std_lattice: NDArray[np.double]
    """Row-wise lattice vectors of the standardized unit cell

    shape: (3, 3)
    """
    std_types: NDArray[np.intc]
    """Identity numbers of atoms in the standardized unit cell

    shape: (n_std_atoms, )
    """
    std_positions: NDArray[np.double]
    """Fractional coordinates of atoms in the standardized unit cell

    shape: (n_std_atoms, 3)
    """
    std_tensors: NDArray[np.double]
    """
    shape:
        (n_std_atoms, ) for collinear magnetic moments.
        (n_std_atoms, 3) for vector non-collinear magnetic moments.
    """
    std_rotation_matrix: NDArray[np.double]
    """Rigid rotation matrix to rotate from standardized basis vectors to idealized
    standardized basis vectors"""

    # Intermediate data in symmetry search
    primitive_lattice: NDArray[np.double]
    """Basis vectors of primitive lattice.

    shape: (3, 3)
    """


@dataclasses.dataclass(eq=True, frozen=True)
class MagneticSpaceGroupType(DictInterface):
    """Magnetic space group type information.

    .. versionadded:: 2.5.0
    """

    uni_number: int
    """Serial number of UNI (or BNS) symbols"""
    litvin_number: int
    """Serial number in Litvin's [Magnetic group tables](https://www.iucr.org/publ/978-0-9553602-2-0)"""
    bns_number: str
    """BNS number e.g. '151.32'"""
    og_number: str
    """OG number e.g. '153.4.1270'"""
    number: int
    """ITA's serial number of space group for reference setting"""
    type: int
    """Type of MSG from 1 to 4"""


def get_magnetic_symmetry(
    cell: MsgCell,
    symprec: float = 1e-5,
    angle_tolerance: float = -1.0,
    mag_symprec: float = -1.0,
    is_axial: bool | None = None,
    with_time_reversal: bool = True,
    _throw: bool = False,
) -> dict[str, Any] | None:
    r"""Find magnetic symmetry operations from a crystal structure and site tensors.

    Parameters
    ----------
    cell : tuple
        Crystal structure given either in tuple.
        In the case given by a tuple, it has to follow the form below,

        (basis vectors, atomic points, types in integer numbers, ...)

        - basis vectors : array_like
            shape=(3, 3), order='C', dtype='double'

            .. code-block::

                [[a_x, a_y, a_z],
                [b_x, b_y, b_z],
                [c_x, c_y, c_z]]

        - atomic points : array_like
            shape=(num_atom, 3), order='C', dtype='double'

            Atomic position vectors with respect to basis vectors, i.e.,
            given in  fractional coordinates.
        - types : array_like
            shape=(num_atom, ), dtype='intc'

            Integer numbers to distinguish species.
        - magmoms:
            case-I: Scalar
                shape=(num_atom, ), dtype='double'

                Each atomic site has a scalar value. With is_magnetic=True,
                values are included in the symmetry search in a way of
                collinear magnetic moments.
            case-II: Vectors
                shape=(num_atom, 3), order='C', dtype='double'

                Each atomic site has a vector. With is_magnetic=True,
                vectors are included in the symmetry search in a way of
                non-collinear magnetic moments.
    symprec : float
        Symmetry search tolerance in the unit of length.
    angle_tolerance : float
        Symmetry search tolerance in the unit of angle deg.
        Normally it is not recommended to use this argument.
        See a bit more detail at :ref:`variables_angle_tolerance`.
        If the value is negative, an internally optimized routine is used to judge
        symmetry.
    mag_symprec : float
        Tolerance for magnetic symmetry search in the unit of magnetic moments.
        If not specified, use the same value as symprec.
    is_axial: None or bool
        Set `is_axial=True` if `magmoms` does not change their sign by improper
        rotations. If not specified, set `is_axial=False` when
        `magmoms.shape==(num_atoms, )`, and set `is_axial=True` when
        `magmoms.shape==(num_atoms, 3)`. These default settings correspond to
        collinear and non-collinear spins.
    with_time_reversal: bool
        Set `with_time_reversal=True` if `magmoms` change their sign by time-reversal
        operations. Default is True.

    Returns
    -------
    symmetry: dict or None
        Rotation parts and translation parts of symmetry operations represented
        with respect to basis vectors and atom index mapping by symmetry
        operations.

        - 'rotations' : ndarray
            shape=(num_operations, 3, 3), order='C', dtype='intc'

            Rotation (matrix) parts of symmetry operations
        - 'translations' : ndarray
            shape=(num_operations, 3), dtype='double'

            Translation (vector) parts of symmetry operations
        - 'time_reversals': ndarray
            shape=(num_operations, ), dtype='bool\_'

            Time reversal part of magnetic symmetry operations.
            True indicates time reversal operation, and False indicates
            an ordinary operation.
        - 'equivalent_atoms' : ndarray
            shape=(num_atoms, ), dtype='intc'
        - 'primitive_lattice': ndarray
            shape=(3, 3), dtype='double'

    Notes
    -----
    .. versionadded:: 2.0

    """
    _set_no_error(_throw)

    lattice, positions, numbers, magmoms = _expand_cell(cell)
    if magmoms is None:
        raise TypeError("Specify magnetic moments in cell.")

    max_size = len(positions) * 96
    rotations = np.zeros((max_size, 3, 3), dtype="intc", order="C")
    translations = np.zeros((max_size, 3), dtype="double", order="C")
    equivalent_atoms = np.zeros(len(magmoms), dtype="intc")
    primitive_lattice = np.zeros((3, 3), dtype="double", order="C")
    # (magmoms.ndim - 1) has to be equal to the rank of physical
    # tensors, e.g., ndim=1 for collinear, ndim=2 for non-collinear.
    if magmoms.ndim == 1 or magmoms.ndim == 2:
        spin_flips = np.zeros(max_size, dtype="intc")
    else:
        spin_flips = None

    # Infer is_axial value from tensor_rank to keep backward compatibility
    if is_axial is None:
        if magmoms.ndim == 1:
            is_axial = False  # Collinear spin
        elif magmoms.ndim == 2:
            is_axial = True  # Non-collinear spin

    try:
        num_sym = _spglib.symmetry_with_site_tensors(
            rotations,
            translations,
            equivalent_atoms,
            primitive_lattice,
            spin_flips,
            lattice,
            positions,
            numbers,
            magmoms,
            int(with_time_reversal * 1),
            int(is_axial * 1),
            float(symprec),
            float(angle_tolerance),
            float(mag_symprec),
        )
    except Exception as exc:
        _set_or_throw_error(exc, _throw)
        return None
    spin_flips = np.array(spin_flips[:num_sym], dtype="intc", order="C")
    # True for time reversal operation, False for ordinary operation
    time_reversals = spin_flips == -1
    return {
        "rotations": np.array(rotations[:num_sym], dtype="intc", order="C"),
        "translations": np.array(translations[:num_sym], dtype="double", order="C"),
        "time_reversals": time_reversals,
        "equivalent_atoms": equivalent_atoms,
        "primitive_lattice": np.array(
            np.transpose(primitive_lattice),
            dtype="double",
            order="C",
        ),
    }


def get_magnetic_symmetry_dataset(
    cell: MsgCell,
    is_axial: bool | None = None,
    symprec: float = 1e-5,
    angle_tolerance: float = -1.0,
    mag_symprec: float = -1.0,
) -> SpglibMagneticDataset | None:
    """Search magnetic symmetry dataset from an input cell. If it fails, return None.

    Parameters
    ----------
    cell, is_axial, symprec, angle_tolerance, mag_symprec:
        See :func:`get_magnetic_symmetry`.

    Returns
    -------
    dataset : :class:`SpglibMagneticDataset` or None

    Notes
    -----
    .. versionadded:: 2.0

    """  # noqa: E501
    _set_no_error()

    lattice, positions, numbers, magmoms = _expand_cell(cell)

    tensor_rank = magmoms.ndim - 1

    # If is_axial is not specified, select collinear or non-collinear spin cases
    if is_axial is None:
        if tensor_rank == 0:
            is_axial = False  # Collinear spin
        elif tensor_rank == 1:
            is_axial = True  # Non-collinear spin

    try:
        spg_ds = _spglib.magnetic_dataset(
            lattice,
            positions,
            numbers,
            magmoms,
            int(tensor_rank),
            is_axial,
            float(symprec),
            float(angle_tolerance),
            float(mag_symprec),
        )
    except Exception as exc:
        _set_or_throw_error(exc)
        return None
    return SpglibMagneticDataset(**spg_ds)


def get_magnetic_spacegroup_type(uni_number: int) -> MagneticSpaceGroupType | None:
    """Translate UNI number to magnetic space group type information.

    If fails, return None.

    Parameters
    ----------
    uni_number : int
        UNI number between 1 to 1651

    Returns
    -------
    magnetic_spacegroup_type: :class:`MagneticSpaceGroupType` | None

    Notes
    -----
    .. versionadded:: 2.0

    """
    _set_no_error()

    try:
        msg_type = _spglib.magnetic_spacegroup_type(int(uni_number))
    except Exception as exc:
        _set_or_throw_error(exc)
        return None
    return MagneticSpaceGroupType(**msg_type)


def get_magnetic_spacegroup_type_from_symmetry(
    rotations: ArrayLike[np.intc],
    translations: ArrayLike[np.double],
    time_reversals: ArrayLike[np.intc],
    lattice: ArrayLike[np.double] | None = None,
    symprec: float = 1e-5,
) -> MagneticSpaceGroupType | None:
    """Return magnetic space-group type information from symmetry operations.

    Parameters
    ----------
    rotations, translations, time_reversals:
        See returns of :func:`get_magnetic_symmetry`.
    lattice : (Optional) array_like (3, 3)
        Basis vectors a, b, c given in row vectors. This is used as the measure of
        distance. Default is None, which gives unit matrix.
    symprec: float
        See :func:`get_symmetry`.

    Returns
    -------
    magnetic_spacegroup_type: :class:`MagneticSpaceGroupType` | None

    """
    rots = np.array(rotations, dtype="intc", order="C")
    trans = np.array(translations, dtype="double", order="C")
    timerev = np.array(time_reversals, dtype="intc", order="C")
    if lattice is None:
        latt = np.eye(3, dtype="double", order="C")
    else:
        latt = np.array(lattice, dtype="double", order="C")

    _set_no_error()
    try:
        msg_type = _spglib.magnetic_spacegroup_type_from_symmetry(
            rots, trans, timerev, latt, float(symprec)
        )
    except Exception as exc:
        _set_or_throw_error(exc)
        return None
    return MagneticSpaceGroupType(**msg_type)


def get_magnetic_symmetry_from_database(
    uni_number: int, hall_number: int = 0
) -> dict[str, Any] | None:
    """Return magnetic symmetry operations from UNI number between 1 and 1651.

    If fails, return None.

    Optionally alternative settings can be specified with Hall number.

    Parameters
    ----------
    uni_number : int
        UNI number between 1 and 1651.
    hall_number : int, optional
        The Hall symbol is given by the serial number in between 1 and 530.

    Returns
    -------
    symmetry : dict
        - 'rotations'
        - 'translations'
        - 'time_reversals'
            0 and 1 indicate ordinary and anti-time-reversal operations, respectively.

    Notes
    -----
    .. versionadded:: 2.0

    """
    _set_no_error()

    rotations = np.zeros((384, 3, 3), dtype="intc")
    translations = np.zeros((384, 3), dtype="double")
    time_reversals = np.zeros(384, dtype="intc")
    try:
        num_sym = _spglib.magnetic_symmetry_from_database(
            rotations,
            translations,
            time_reversals,
            int(uni_number),
            int(hall_number),
        )
    except Exception as exc:
        _set_or_throw_error(exc)
        return None
    return {
        "rotations": np.array(rotations[:num_sym], dtype="intc", order="C"),
        "translations": np.array(translations[:num_sym], dtype="double", order="C"),
        "time_reversals": np.array(
            time_reversals[:num_sym],
            dtype="intc",
            order="C",
        ),
    }
