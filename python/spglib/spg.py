"""SpaceGroup operations."""
# Copyright (C) 2015 Atsushi Togo
# This file is part of spglib.
# SPDX-License-Identifier: BSD-3-Clause

from __future__ import annotations

import dataclasses
import warnings
from typing import Any, cast

import numpy as np
from numpy.typing import ArrayLike, NDArray

import spglib

from . import _spglib
from ._compat.typing import TypeAlias
from ._compat.warnings import deprecated
from .error import _set_no_error, _set_or_throw_error
from .utils import Cell, DictInterface, Lattice, Numbers, Positions, _expand_cell

__all__ = [
    "SpgCell",
    "SpglibDataset",
    "SpaceGroupType",
    "get_symmetry",
    "get_symmetry_dataset",
    "get_spacegroup",
    "get_spacegroup_type",
    "get_spacegroup_type_from_symmetry",
    "get_symmetry_from_database",
    "get_hall_number_from_symmetry",
]


SpgCell: TypeAlias = tuple[Lattice, Positions, Numbers]
"""
Crystal structure represented by a tuple of (:py:type:`spglib.utils.Lattice`,
:py:type:`spglib.utils.Positions`, :py:type:`spglib.utils.Numbers`).
"""


@dataclasses.dataclass(eq=False, frozen=True)
class SpglibDataset(DictInterface):
    """Spglib dataset information.

    .. versionadded:: 1.9.4
        The member 'choice' is added.
    .. versionadded:: 2.5.0
    """

    number: int
    """International space group number"""
    hall_number: int
    """Hall number.

    This number is used in
        :func:`get_symmetry_from_database` and
        :func:`get_spacegroup_type`.
    """
    international: str
    """International symbol"""
    hall: str
    """Hall symbol"""
    choice: str
    """Centring, origin, basis vector setting"""
    transformation_matrix: NDArray[np.double]
    """Transformation matrix from input lattice to standardized lattice.

    shape=(3, 3), order='C', dtype='double'

    .. code-block::

        L^original = L^standardized * Tmat.

    See the detail at :ref:`dataset_origin_shift_and_transformation`.
    """
    origin_shift: NDArray[np.double]
    """Origin shift from standardized to input origin.

    shape=(3,), dtype='double'

    See the detail at :ref:`dataset_origin_shift_and_transformation`.
    """
    rotations: NDArray[np.intc]
    """Matrix (rotation) parts of space group operations.

    shape=(n_operations, 3, 3), order='C', dtype='intc'

    .. note::

        Space group operations are obtained by

        .. code-block:: python

            [(r,t) for r, t in zip(rotations, translations)]

    See also :func:`get_symmetry`.
    """
    translations: NDArray[np.double]
    """Vector (translation) parts of space group operations.

    shape=(n_operations, 3), order='C', dtype='double'

    .. note::

        Space group operations are obtained by

        .. code-block:: python

            [(r,t) for r, t in zip(rotations, translations)]

    See also :func:`get_symmetry`.
    """
    wyckoffs: list[str]
    """Wyckoff letters corresponding to the space group type."""
    site_symmetry_symbols: list[str]
    """Site symmetry symbols corresponding to the space group type."""
    crystallographic_orbits: NDArray[np.intc]
    """Symmetrically equivalent atoms, where 'symmetrically' means the space group
    operations corresponding to the space group type.

    This is very similar to ``equivalent_atoms``. See the difference explained in
    ``equivalent_atoms``

    shape=(n_atoms,), dtype='intc'
    """
    equivalent_atoms: NDArray[np.intc]
    """Symmetrically equivalent atoms, where 'symmetrically' means found symmetry
    operations.

    shape=(n_atoms,), dtype='intc'

    In spglib, symmetry operations are given for the input cell.
    When a non-primitive cell is inputted and the lattice made by the non-primitive
    basis vectors breaks its point group, the found symmetry operations may not
    correspond to the crystallographic space group type.
    """
    # Primitive cell
    primitive_lattice: NDArray[np.double]
    """Basis vectors a, b, c of a primitive cell in row vectors.

    shape=(3, 3), order='C', dtype='double'

    This is the primitive cell found inside spglib before applying standardization.
    Therefore, the values are distorted with respect to found space group type.
    """
    mapping_to_primitive: NDArray[np.intc]
    """Atom index mapping from original cell to the primitive cell of
    ``primitive_lattice``.

    shape=(n_atoms), dtype='intc'
    """
    # Idealized standardized unit cell
    std_lattice: NDArray[np.double]
    """Basis vectors a, b, c of an idealized standardized cell in row vectors.

    shape=(3, 3), order='C', dtype='double'
    """
    std_positions: NDArray[np.double]
    """Positions of atoms in the standardized cell in fractional coordinates.

    shape=(n_atoms, 3), order='C', dtype='double'
    """
    std_types: NDArray[np.intc]
    """Identity numbers of atoms in the standardized cell.

    shape=(n_atoms,), dtype='intc'
    """
    std_rotation_matrix: NDArray[np.double]
    """Rigid rotation matrix to rotate from standardized basis vectors to idealized
    standardized orthonormal basis vectors.

    shape=(3, 3), order='C', dtype='double'

    .. code-block::

        L^idealized = R * L^standardized.

    See the detail at :ref:`dataset_idealized_cell`.
    """
    std_mapping_to_primitive: NDArray[np.intc]
    """``std_positions`` index mapping to those of atoms of the primitive cell in the
    standardized cell."""
    pointgroup: str
    """Pointgroup symbol in Hermann-Mauguin notation."""


@dataclasses.dataclass(eq=True, frozen=True)
class SpaceGroupType(DictInterface):
    """Space group type information.

    More details are found at :ref:`spglib-dataset`.

    .. versionchanged:: 2.0
        ``hall_number`` member is added.
    .. versionadded:: 2.5.0
    """

    number: int
    """International space group number"""
    international_short: str
    """International short symbol"""
    international_full: str
    """International full symbol"""
    international: str
    """International symbol"""
    schoenflies: str
    """Schoenflies symbol"""
    hall_number: int
    """Hall symbol serial number"""
    hall_symbol: str
    """Hall symbol"""
    choice: str
    """Centring, origin, basis vector setting"""
    pointgroup_international: str
    """International symbol of crystallographic point group"""
    pointgroup_schoenflies: str
    """Schoenflies symbol of crystallographic point group"""
    arithmetic_crystal_class_number: int
    """Arithmetic crystal class number"""
    arithmetic_crystal_class_symbol: str
    """Arithmetic crystal class symbol"""


def get_symmetry(
    cell: Cell,
    symprec: float = 1e-5,
    angle_tolerance: float = -1.0,
    mag_symprec: float = -1.0,
    is_magnetic: bool = True,
) -> dict[str, Any] | None:
    r"""Find symmetry operations from a crystal structure and site tensors.

    .. warning::
        :func:`get_symmetry` with ``is_magnetic=True`` is deprecated at version 2.0.

    Use :func:`get_magnetic_symmetry` for magnetic symmetry search.

    Parameters
    ----------
    cell : tuple
        Crystal structure given in tuple.
        It has to follow the following form,
        (basis vectors, atomic points, types in integer numbers, ...)

        - basis vectors : array_like
            shape=(3, 3), order='C', dtype='double'

            .. code-block:: python

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
        - optional data :
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
        If the value is negative, an internally optimized routine is used to
        judge symmetry.
    mag_symprec : float
        Tolerance for magnetic symmetry search in the unit of magnetic moments.
        If not specified, use the same value as symprec.
    is_magnetic : bool
        When optional data (4th element of cell tuple) is given in case-II,
        the symmetry search is performed considering magnetic symmetry, which
        may be corresponding to that for non-collinear calculation. Default is
        True, but this does nothing unless optional data is supplied.

    Returns
    -------
    symmetry: dict
        Rotation parts and translation parts of symmetry operations are represented
        with respect to basis vectors.
        When the search failed, :code:`None` is returned.

        - 'rotations' : ndarray
            shape=(num_operations, 3, 3), order='C', dtype='intc'

            Rotation (matrix) parts of symmetry operations
        - 'translations' : ndarray
            shape=(num_operations, 3), dtype='double'

            Translation (vector) parts of symmetry operations
        - 'time_reversals': ndarray (exists when the optional data is given)
            shape=(num_operations, ), dtype='bool\_'

            Time reversal part of magnetic symmetry operations.
            True indicates time reversal operation, and False indicates
            an ordinary operation.
        - 'equivalent_atoms' : ndarray
            shape=(num_atoms, ), dtype='intc'

            A mapping table of atoms to symmetrically independent atoms.
            This is used to find symmetrically equivalent atoms.
            The numbers contained are the indices of atoms starting from 0,
            i.e., the first atom is numbered as 0, and
            then 1, 2, 3, ... :code:`np.unique(equivalent_atoms)` gives representative
            symmetrically independent atoms. A list of atoms that are
            symmetrically equivalent to some independent atom (here for example 1
            is in :code:`equivalent_atom`) is found by
            :code:`np.where(equivalent_atom=1)[0]`.

    Notes
    -----
    The orders of the rotation matrices and the translation
    vectors correspond with each other, e.g. , the second symmetry
    operation is organized by the set of the second rotation matrix and second
    translation vector in the respective arrays. Therefore a set of
    symmetry operations may obtained by

    .. code-block:: python

        [(r, t) for r, t in zip(dataset['rotations'], dataset['translations'])]

    The operations are given with respect to the fractional coordinates
    (not for Cartesian coordinates). The rotation matrix and translation
    vector are used as follows:

    .. code-block::

        new_vector[3x1] = rotation[3x3] * vector[3x1] + translation[3x1]

    The three values in the vector are given for the a, b, and c axes,
    respectively.

    """
    _set_no_error()

    _, _, _, magmoms = _expand_cell(cell)

    if magmoms is None:
        # Get symmetry operations without on-site tensors (i.e. normal crystal)
        try:
            dataset = get_symmetry_dataset(
                cast(SpgCell, cell),
                symprec=symprec,
                angle_tolerance=angle_tolerance,
                _throw=True,
            )
        except Exception as exc:
            _set_or_throw_error(exc)
            return None
        return {
            "rotations": dataset.rotations,
            "translations": dataset.translations,
            "equivalent_atoms": dataset.equivalent_atoms,
        }
    else:
        warnings.warn(
            "Use get_magnetic_symmetry() for cell with magnetic moments.",
            DeprecationWarning,
            stacklevel=1,
        )
        try:
            return spglib.get_magnetic_symmetry(
                cast(spglib.MsgCell, cell),
                symprec=symprec,
                angle_tolerance=angle_tolerance,
                mag_symprec=mag_symprec,
                is_axial=None,
                with_time_reversal=is_magnetic,
                _throw=True,
            )
        except Exception as exc:
            _set_or_throw_error(exc)
            return None


def get_symmetry_dataset(
    cell: SpgCell,
    symprec: float = 1e-5,
    angle_tolerance: float = -1.0,
    hall_number: int = 0,
    _throw: bool = False,
) -> SpglibDataset | None:
    """Search symmetry dataset from an input cell.

    Parameters
    ----------
    cell, symprec, angle_tolerance:
        See :func:`get_symmetry`.
    hall_number : int
        If a serial number of Hall symbol (>0) is given, the database
        corresponding to the Hall symbol is made.

        The mapping from Hall symbols to a space-group-type is the many-to-one mapping.
        Without specifying this
        option (i.e., in the case of ``hall_number=0``), always the first one
        (the smallest serial number corresponding to the space-group-type in
        [list of space groups (Seto's web site)](https://yseto.net/en/sg/sg1))
        among possible choices and settings is chosen as default. This
        argument is useful when the other choice (or setting) is expected to
        be hooked.

        This affects to the obtained values of `international`,
        `hall`, `choice`, `transformation_matrix`,
        `origin shift`, `wyckoffs`, `std_lattice`, `std_positions`,
        `std_types` and `std_rotation_matrix`, but not to `rotations`
        and `translations` since the later set is defined with respect to
        the basis vectors of user's input (the `cell` argument).

        See also :ref:`dataset_spg_get_dataset_spacegroup_type`.

    Returns
    -------
    dataset: :class:`SpglibDataset` | None
        If it fails, None is returned. Otherwise a dictionary is returned.
        More details are found at :ref:`spglib-dataset`.

    """
    _set_no_error(_throw)

    lattice, positions, numbers, _ = _expand_cell(cell)

    try:
        spg_ds = _spglib.dataset(
            lattice,
            positions,
            numbers,
            int(hall_number),
            float(symprec),
            float(angle_tolerance),
        )
    except Exception as exc:
        _set_or_throw_error(exc, _throw)
        return None

    return SpglibDataset(**spg_ds)


def get_spacegroup(
    cell: SpgCell,
    symprec: float = 1e-5,
    angle_tolerance: float = -1.0,
    symbol_type: int = 0,
) -> str | None:
    """Return space group in international table symbol and number as a string.

    With ``symbol_type=1``, Schoenflies symbol is given instead of international symbol.

    :rtype: str | None
    :return:
        If it fails, None is returned.
    """
    _set_no_error()

    try:
        dataset = get_symmetry_dataset(
            cell,
            symprec=symprec,
            angle_tolerance=angle_tolerance,
            _throw=True,
        )
    except Exception as exc:
        _set_or_throw_error(exc)
        return None

    try:
        spg_type = get_spacegroup_type(dataset.hall_number, _throw=True)
    except Exception as exc:
        _set_or_throw_error(exc)
        return None

    if symbol_type == 1:
        return f"{spg_type.schoenflies} ({dataset.number})"
    else:
        return f"{spg_type.international_short} ({dataset.number})"


def get_spacegroup_type(
    hall_number: int,
    _throw: bool = False,
) -> SpaceGroupType | None:
    """Translate Hall number to space group type information. If it fails, return None.

    This function allows to directly access to the space-group-type database
    in spglib (spg_database.c).
    To specify the space group type with a specific choice, ``hall_number`` is used.
    The definition of ``hall_number`` is found at
    :ref:`dataset_spg_get_dataset_spacegroup_type`.

    :param hall_number: Serial number for Hall symbol
    :return: :class:`SpaceGroupType` or None

    .. versionadded:: 1.9.4
    """
    _set_no_error(_throw)

    try:
        spg_type = _spglib.spacegroup_type(int(hall_number))
    except Exception as exc:
        _set_or_throw_error(exc, _throw)
        return None
    return SpaceGroupType(**spg_type)


def get_spacegroup_type_from_symmetry(
    rotations: ArrayLike[np.intc],
    translations: ArrayLike[np.double],
    lattice: ArrayLike[np.double] | None = None,
    symprec: float = 1e-5,
) -> SpaceGroupType | None:
    """Return space-group type information from symmetry operations.

    This is expected to work well for the set of symmetry operations whose
    distortion is small. The aim of making this feature is to find
    space-group-type for the set of symmetry operations given by the other
    source than spglib.

    Parameters
    ----------
    rotations : array_like
        Matrix parts of space group operations.
        shape=(n_operations, 3, 3), order='C', dtype='intc'
    translations : array_like
        Vector parts of space group operations.
        shape=(n_operations, 3), order='C', dtype='double'
    lattice : array_like, optional
        Basis vectors a, b, c given in row vectors. Default is None, which gives
        unit matrix. This should be the same as that used to find ``rotations``
        and ``translations``. If it is unknown, the cubic basis vector may be a
        possible choice unless the ``rotations`` and ``translations`` were
        obtained for an unusual (very oblique) choice of basis vectors.
        shape=(3, 3), order='C', dtype='double'
    symprec: float
        See :func:`get_symmetry`.

    Returns
    -------
    spacegroup_type : :class:`SpaceGroupType` or None

    Notes
    -----
    .. versionadded:: 2.0

    This is the replacement of :func:`get_hall_number_from_symmetry`.

    """
    r = np.array(rotations, dtype="intc", order="C")
    t = np.array(translations, dtype="double", order="C")
    if lattice is None:
        _lattice = np.eye(3, dtype="double", order="C")
    else:
        _lattice = np.array(np.transpose(lattice), dtype="double", order="C")

    _set_no_error()

    try:
        spg_type = _spglib.spacegroup_type_from_symmetry(r, t, _lattice, float(symprec))
    except Exception as exc:
        _set_or_throw_error(exc)
        return None
    return SpaceGroupType(**spg_type)


def get_symmetry_from_database(hall_number: int) -> dict[str, Any] | None:
    """Return symmetry operations corresponding to a Hall symbol. If fails, return None.

    Parameters
    ----------
    hall_number : int
        The Hall symbol is given by the serial number in between 1 and 530.
        The definition of ``hall_number`` is found at
        :ref:`dataset_spg_get_dataset_spacegroup_type`.

    Returns
    -------
    symmetry : dict
        - rotations
            Rotation parts of symmetry operations corresponding to ``hall_number``.
        - translations
            Translation parts of symmetry operations corresponding to ``hall_number``.

    """
    _set_no_error()

    rotations = np.zeros((192, 3, 3), dtype="intc")
    translations = np.zeros((192, 3), dtype="double")
    try:
        num_sym = _spglib.symmetry_from_database(
            rotations,
            translations,
            int(hall_number),
        )
    except Exception as exc:
        _set_or_throw_error(exc)
        return None
    return {
        "rotations": np.array(rotations[:num_sym], dtype="intc", order="C"),
        "translations": np.array(translations[:num_sym], dtype="double", order="C"),
    }


@deprecated("Use get_spacegroup_type_from_symmetry instead")
def get_hall_number_from_symmetry(
    rotations: ArrayLike[np.intc],
    translations: ArrayLike[np.double],
    symprec: float = 1e-5,
) -> int | None:
    """Hall number is obtained from a set of symmetry operations. If fails, return None.

    .. deprecated:: 2.0
        Replaced by {func}`get_spacegroup_type_from_symmetry`.

    Return one of ``hall_number`` corresponding to a space-group type of the given
    set of symmetry operations. When multiple ``hall_number`` exist for the
    space-group type, the smallest one (the first description of the space-group
    type in International Tables for Crystallography) is chosen. The definition of
    ``hall_number`` is found at :ref:`dataset_spg_get_dataset_spacegroup_type` and
    the corresponding space-group-type information is obtained through
    {func}`get_spacegroup_type`.

    This is expected to work well for the set of symmetry operations whose
    distortion is small. The aim of making this feature is to find
    space-group-type for the set of symmetry operations given by the other
    source than spglib.

    Note that the definition of ``symprec`` is
    different from usual one, but is given in the fractional
    coordinates and so it should be small like ``1e-5``.
    """
    _set_no_error()

    r = np.array(rotations, dtype="intc", order="C")
    t = np.array(translations, dtype="double", order="C")
    try:
        hall_number = _spglib.hall_number_from_symmetry(r, t, float(symprec))
    except Exception as exc:
        _set_or_throw_error(exc)
        return None
    return hall_number
