"""Python bindings for C library for finding and handling crystal."""
# Copyright (C) 2015 Atsushi Togo
# This file is part of spglib.
# SPDX-License-Identifier: BSD-3-Clause

from __future__ import annotations

import dataclasses
import os
import warnings
from collections.abc import Mapping, Sequence
from typing import TYPE_CHECKING, Union, cast

import numpy as np

try:
    from . import _spglib
except ImportError:
    import os

    # If we are only building the documentation create a stub _spglib
    if (
        os.getenv("READTHEDOCS", "False").lower() == "true"
        or os.getenv("BUILD_DOCS", "False").lower() == "true"
    ):
        _spglib = object  # type: ignore[assignment]
    else:
        # Otherwise re-raise the error
        raise
from ._compat.typing import TypeAlias
from ._compat.warnings import deprecated
from .error import SpglibError

if TYPE_CHECKING:
    from collections.abc import Iterator
    from typing import Any

    from numpy.typing import ArrayLike, NDArray

__all__ = [
    "MagneticSpaceGroupType",
    "SpaceGroupType",
    "SpglibDataset",
    "SpglibMagneticDataset",
    "delaunay_reduce",
    "find_primitive",
    "get_error_message",
    "get_hall_number_from_symmetry",
    "get_ir_reciprocal_mesh",
    "get_magnetic_spacegroup_type",
    "get_magnetic_spacegroup_type_from_symmetry",
    "get_magnetic_symmetry",
    "get_magnetic_symmetry_dataset",
    "get_magnetic_symmetry_from_database",
    "get_spacegroup",
    "get_spacegroup_type",
    "get_spacegroup_type_from_symmetry",
    "get_symmetry",
    "get_symmetry_dataset",
    "get_symmetry_from_database",
    "get_version",
    "niggli_reduce",
    "refine_cell",
    "spg_get_commit",
    "spg_get_version",
    "spg_get_version_full",
    "standardize_cell",
]

warnings.filterwarnings(
    "module", category=DeprecationWarning, message=r"dict interface.*"
)

Lattice: TypeAlias = Sequence[Sequence[float]]
Positions: TypeAlias = Sequence[Sequence[float]]
Numbers: TypeAlias = Sequence[int]
Magmoms: TypeAlias = Union[Sequence[float], Sequence[Sequence[float]]]
SpgCell: TypeAlias = tuple[Lattice, Positions, Numbers]
"""Crystal structure represented by a tuple of (lattice, positions, numbers)."""
MsgCell: TypeAlias = tuple[Lattice, Positions, Numbers, Magmoms]
"""Magnetic crystal structure represented by a tuple of
(lattice, positions, numbers, magmoms)."""
Cell: TypeAlias = Union[SpgCell, MsgCell]
"""Either SpgCell or MsgCell."""


OLD_ERROR_HANDLING: bool = True
"""
Use the old error handling.

Note that this variable may be removed in the future or change value in the future.
You can also use :envvar:`SPGLIB_OLD_ERROR_HANDLING` instead of altering this value.
"""


_spglib_error = ""


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
            return get_magnetic_symmetry(
                cast(MsgCell, cell),
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
    hall_number: int, _throw: bool = False
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
    cell: SpgCell, symprec: float = 1e-5, angle_tolerance: float = -1.0
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
    cell: SpgCell, symprec: float = 1e-5, angle_tolerance: float = -1.0
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


############
# k-points #
############


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


def delaunay_reduce(
    lattice: ArrayLike[np.double], eps: float = 1e-5
) -> np.ndarray | None:
    r"""Run Delaunay reduction. When the search failed, `None` is returned.

    The transformation from original basis vectors
    :math:`( \mathbf{a} \; \mathbf{b} \; \mathbf{c} )`
    to final basis vectors :math:`( \mathbf{a}' \; \mathbf{b}' \; \mathbf{c}' )` is achieved by linear
    combination of basis vectors with integer coefficients without
    rotating coordinates. Therefore the transformation matrix is obtained
    by :math:`\mathbf{P} = ( \mathbf{a} \; \mathbf{b} \; \mathbf{c} ) ( \mathbf{a}' \; \mathbf{b}' \; \mathbf{c}' )^{-1}` and the matrix
    elements have to be almost integers.

    The algorithm is found in the international tables for crystallography volume A.

    Parameters
    ----------
    lattice: ndarray, (3, 3)
        Lattice parameters in the form of

        .. code-block::

            [[a_x, a_y, a_z],
                [b_x, b_y, b_z],
                [c_x, c_y, c_z]]

    eps: float
        Tolerance parameter, but unlike `symprec` the unit is not a length.
        Tolerance to check if volume is close to zero or not and
        if two basis vectors are orthogonal by the value of dot
        product being close to zero or not.

    Returns
    -------
    delaunay_lattice: ndarray, (3, 3)
        Reduced lattice parameters are given as a numpy 'double' array:

        .. code-block::

            [[a_x, a_y, a_z],
             [b_x, b_y, b_z],
             [c_x, c_y, c_z]]

    Notes
    -----
    .. versionadded:: 1.9.4

    """  # noqa: E501
    _set_no_error()

    delaunay_lattice = np.array(np.transpose(lattice), dtype="double", order="C")
    try:
        _spglib.delaunay_reduce(delaunay_lattice, float(eps))
    except Exception as exc:
        _set_or_throw_error(exc)
        return None
    return np.array(np.transpose(delaunay_lattice), dtype="double", order="C")


def niggli_reduce(
    lattice: ArrayLike[np.double], eps: float = 1e-5
) -> np.ndarray | None:
    r"""Run Niggli reduction. When the search failed, ``None`` is returned.

    The transformation from original basis vectors :math:`( \mathbf{a} \; \mathbf{b} \; \mathbf{c} )` to final basis vectors :math:`( \mathbf{a}' \; \mathbf{b}' \; \mathbf{c}' )` is achieved by linear
    combination of basis vectors with integer coefficients without
    rotating coordinates. Therefore the transformation matrix is obtained
    by :math:`\mathbf{P} = ( \mathbf{a} \; \mathbf{b} \; \mathbf{c} ) ( \mathbf{a}' \; \mathbf{b}' \; \mathbf{c}' )^{-1}` and the matrix
    elements have to be almost integers.

    The algorithm detail is found at https://atztogo.github.io/niggli/ and
    the references are there in.

    Parameters
    ----------
    lattice: ndarray
        Lattice parameters in the form of

        .. code-block::

            [[a_x, a_y, a_z],
            [b_x, b_y, b_z],
            [c_x, c_y, c_z]]

    eps: float
        Tolerance parameter, but unlike `symprec` the unit is not a length.
        This is used to check if difference of norms of two basis
        vectors is close to zero or not and if two basis vectors are
        orthogonal by the value of dot product being close to zero or
        not.
        The detail is shown at https://atztogo.github.io/niggli/.

    Returns
    -------
    niggli_lattice: ndarray, (3, 3)
        if the Niggli reduction succeeded:
            Reduced lattice parameters are given as a numpy 'double' array:

            .. code-block::

                [[a_x, a_y, a_z],
                [b_x, b_y, b_z],
                [c_x, c_y, c_z]]

        otherwise None is returned.

    Notes
    -----
    .. versionadded:: 1.9.4

    """  # noqa: E501
    _set_no_error()

    niggli_lattice = np.array(np.transpose(lattice), dtype="double", order="C")
    try:
        _spglib.niggli_reduce(niggli_lattice, float(eps))
    except Exception as exc:
        _set_or_throw_error(exc)
        return None
    return np.array(np.transpose(niggli_lattice), dtype="double", order="C")


@deprecated("Set OLD_ERROR_HANDLING to false and catch the errors directly")
def get_error_message() -> str:
    """Return error message why spglib failed.

    .. warning::
        This method is not thread safe, i.e., only safely usable
        when calling one spglib method per process.

    Notes
    -----
    .. versionadded:: 1.9.5
    .. deprecated:: 2.7.0

    """
    return _spglib_error


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


def _check_OLD_ERROR_HANDLING() -> bool:
    env_var = os.environ.get("SPGLIB_OLD_ERROR_HANLDING")
    if env_var is not None:
        if env_var.lower() in ("false", "0"):
            return False
        return True
    return OLD_ERROR_HANDLING


def _set_or_throw_error(exc: Exception, _throw: bool = False) -> None:
    if _throw or not _check_OLD_ERROR_HANDLING():
        if isinstance(exc, SpglibError):
            # Our native errors we pass transparently
            raise exc
        # Otherwise we try to recast them to SplibError
        raise SpglibError(f"Generic Spglib error:\n{exc}") from exc
    warnings.warn(
        "Set OLD_ERROR_HANDLING to false and catch the errors directly.",
        DeprecationWarning,
        stacklevel=2,
    )
    _spglib_error = str(exc)


def _set_no_error(_throw: bool = False) -> None:
    if _throw or not _check_OLD_ERROR_HANDLING():
        return
    warnings.warn(
        "Set OLD_ERROR_HANDLING to false and catch the errors directly.",
        DeprecationWarning,
        stacklevel=2,
    )
    _spglib_error = "no error"


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
