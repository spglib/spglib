"""Python bindings for C library for finding and handling crystal."""
# Copyright (C) 2015 Atsushi Togo
# All rights reserved.
#
# This file is part of spglib.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in
#   the documentation and/or other materials provided with the
#   distribution.
#
# * Neither the name of the spglib project nor the names of its
#   contributors may be used to endorse or promote products derived
#   from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

from __future__ import annotations

import dataclasses
import sys
import warnings
from collections.abc import Mapping, Sequence
from typing import TYPE_CHECKING, Union

import numpy as np

try:
    from . import _spglib  # type: ignore[attr-defined]
except ImportError:
    if sys.version_info < (3, 10):
        from importlib_resources import as_file, files
        from typing_extensions import TypeAlias
    else:
        from importlib.resources import as_file, files
        from typing import TypeAlias
    from ctypes import cdll

    root = files("spglib.lib")
    for file in root.iterdir():
        if "symspg." in file.name:
            with as_file(file) as bundled_lib:
                try:
                    cdll.LoadLibrary(str(bundled_lib))
                    from . import _spglib  # type: ignore[attr-defined]

                    break
                except ImportError as err:
                    raise FileNotFoundError(
                        "Could not load bundled Spglib C library"
                    ) from err
    else:
        raise FileNotFoundError(
            "Spglib C library is not installed and no bundled version was detected"
        )

if TYPE_CHECKING:
    from collections.abc import Iterator
    from typing import Any

    from numpy.typing import ArrayLike, NDArray

warnings.filterwarnings(
    "module", category=DeprecationWarning, message=r"dict interface.*"
)

Lattice: TypeAlias = Sequence[Sequence[float]]
Positions: TypeAlias = Sequence[Sequence[float]]
Numbers: TypeAlias = Sequence[int]
Magmoms: TypeAlias = Union[Sequence[float], Sequence[Sequence[float]]]
Cell: TypeAlias = Union[
    tuple[Lattice, Positions, Numbers], tuple[Lattice, Positions, Numbers, Magmoms]
]


class SpglibError:
    """Error message why spglib failed."""

    message = "no error"


spglib_error = SpglibError()


@dataclasses.dataclass(eq=True, frozen=True)
class DictInterface(Mapping[str, "Any"]):
    """Base class for dataclass with dict interface.

    .. versionadded:: 2.5.0
    .. deprecated:: 2.5.0
        Dict-like interface (``obj['field']``) are deprecated.
        Please use attribute interface instead (``obj.field``)
    """

    def __getitem__(self, key: str) -> Any:
        """Return the value of the key."""
        warnings.warn(
            f"dict interface ({self.__class__.__name__}['{key}']) is deprecated."
            "Use attribute interface ({self.__class__.__name__}.{key}) instead",
            DeprecationWarning,
        )
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
    """Basis vectors a, b, c of a standardized cell in row vectors.

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


def get_version():
    """Return version number of spglib with tuple of three numbers.

    .. versionadded:: 1.8.3
    .. deprecated:: 2.3.0
        Use :py:func:`spg_get_version` and ``spglib.__version__`` instead
    """
    warnings.warn(
        "get_version() is deprecated. Use __version__ for the python binding"
        "version and get_spg_version for the detected spglib library version.",
        DeprecationWarning,
    )
    _set_no_error()
    return _spglib.version_tuple()


def spg_get_version():
    """Get the X.Y.Z version of the detected spglib C library.

    .. versionadded:: 2.3.0
    :return: version string
    """
    _set_no_error()
    return _spglib.version_string()


def spg_get_version_full():
    """Get the full version of the detected spglib C library.

    .. versionadded:: 2.3.0
    :return: full version string
    """
    _set_no_error()
    return _spglib.version_full()


def spg_get_commit():
    """Get the commit of the detected spglib C library.

    .. versionadded:: 2.3.0
    :return: commit string
    """
    _set_no_error()
    return _spglib.commit()


def get_symmetry(
    cell: Cell,
    symprec=1e-5,
    angle_tolerance=-1.0,
    mag_symprec=-1.0,
    is_magnetic=True,
) -> dict | None:
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
        dataset = get_symmetry_dataset(
            cell,
            symprec=symprec,
            angle_tolerance=angle_tolerance,
        )
        if dataset is None:
            _set_error_message()
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
        )
        return get_magnetic_symmetry(
            cell,
            symprec=symprec,
            angle_tolerance=angle_tolerance,
            mag_symprec=mag_symprec,
            is_axial=None,
            with_time_reversal=is_magnetic,
        )


def get_magnetic_symmetry(
    cell: Cell,
    symprec=1e-5,
    angle_tolerance=-1.0,
    mag_symprec=-1.0,
    is_axial=None,
    with_time_reversal=True,
) -> dict | None:
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
    _set_no_error()

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
        with_time_reversal * 1,
        is_axial * 1,
        symprec,
        angle_tolerance,
        mag_symprec,
    )

    if num_sym == 0:
        _set_error_message()
        return None
    else:
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


def _build_dataset_dict(spg_ds: list) -> SpglibDataset:
    letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

    dataset = SpglibDataset(
        number=spg_ds[0],
        hall_number=spg_ds[1],
        international=spg_ds[2].strip(),
        hall=spg_ds[3].strip(),
        choice=spg_ds[4].strip(),
        transformation_matrix=np.array(
            spg_ds[5],
            dtype="double",
            order="C",
        ),
        origin_shift=np.array(spg_ds[6], dtype="double"),
        rotations=np.array(spg_ds[7], dtype="intc", order="C"),
        translations=np.array(spg_ds[8], dtype="double", order="C"),
        wyckoffs=[letters[x] for x in spg_ds[9]],
        site_symmetry_symbols=[s.strip() for s in spg_ds[10]],
        crystallographic_orbits=np.array(
            spg_ds[11],
            dtype="intc",
        ),
        equivalent_atoms=np.array(spg_ds[12], dtype="intc"),
        primitive_lattice=np.array(
            np.transpose(spg_ds[13]),
            dtype="double",
            order="C",
        ),
        mapping_to_primitive=np.array(
            spg_ds[14],
            dtype="intc",
        ),
        std_lattice=np.array(
            np.transpose(spg_ds[15]),
            dtype="double",
            order="C",
        ),
        std_types=np.array(spg_ds[16], dtype="intc"),
        std_positions=np.array(spg_ds[17], dtype="double", order="C"),
        std_rotation_matrix=np.array(
            spg_ds[18],
            dtype="double",
            order="C",
        ),
        std_mapping_to_primitive=np.array(
            spg_ds[19],
            dtype="intc",
        ),
        pointgroup=spg_ds[20].strip(),
    )
    return dataset


def get_symmetry_dataset(
    cell: Cell,
    symprec=1e-5,
    angle_tolerance=-1.0,
    hall_number=0,
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
    _set_no_error()

    lattice, positions, numbers, _ = _expand_cell(cell)

    spg_ds = _spglib.dataset(
        lattice,
        positions,
        numbers,
        hall_number,
        symprec,
        angle_tolerance,
    )
    if spg_ds is None:
        _set_error_message()
        return None

    dataset = _build_dataset_dict(spg_ds)
    return dataset


def get_symmetry_layerdataset(
    cell: Cell, aperiodic_dir=2, symprec=1e-5
) -> SpglibDataset | None:
    """TODO: Add comments."""
    _set_no_error()

    lattice, positions, numbers, _ = _expand_cell(cell)

    spg_ds = _spglib.layerdataset(
        lattice,
        positions,
        numbers,
        aperiodic_dir,
        symprec,
    )
    if spg_ds is None:
        _set_error_message()
        return None

    dataset = _build_dataset_dict(spg_ds)

    return dataset


def get_magnetic_symmetry_dataset(
    cell: Cell,
    is_axial=None,
    symprec=1e-5,
    angle_tolerance=-1.0,
    mag_symprec=-1.0,
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

    spg_ds = _spglib.magnetic_dataset(
        lattice,
        positions,
        numbers,
        magmoms,
        tensor_rank,
        is_axial,
        symprec,
        angle_tolerance,
        mag_symprec,
    )
    if spg_ds is None:
        _set_error_message()
        return None

    tensor_rank = spg_ds[3]
    std_tensors = np.array(spg_ds[16], dtype="double", order="C")
    if tensor_rank == 1:
        std_tensors = std_tensors.reshape(-1, 3)

    dataset = SpglibMagneticDataset(
        # Magnetic space-group type
        uni_number=spg_ds[0],
        msg_type=spg_ds[1],
        hall_number=spg_ds[2],
        tensor_rank=tensor_rank,
        # Magnetic symmetry operations
        n_operations=spg_ds[4],
        rotations=np.array(spg_ds[5], dtype="intc", order="C"),
        translations=np.array(spg_ds[6], dtype="double", order="C"),
        time_reversals=np.array(spg_ds[7], dtype="intc"),
        # Equivalent atoms
        n_atoms=spg_ds[8],
        equivalent_atoms=np.array(spg_ds[9], dtype="intc"),
        # Transformation to standardized setting
        transformation_matrix=np.array(spg_ds[10], dtype="double", order="C"),
        origin_shift=np.array(spg_ds[11], dtype="double"),
        # Standardized crystal structure
        n_std_atoms=spg_ds[12],
        std_lattice=np.array(np.transpose(spg_ds[13]), dtype="double", order="C"),
        std_types=np.array(spg_ds[14], dtype="intc"),
        std_positions=np.array(spg_ds[15], dtype="double", order="C"),
        std_tensors=std_tensors,
        std_rotation_matrix=np.array(spg_ds[17], dtype="double", order="C"),
        # Intermediate datum in symmetry search
        primitive_lattice=np.array(np.transpose(spg_ds[18]), dtype="double", order="C"),
    )

    return dataset


def get_layergroup(cell: Cell, aperiodic_dir=2, symprec=1e-5) -> SpglibDataset | None:
    """Return layer group in ....

    If it fails, None is returned.

    """
    _set_no_error()

    dataset = get_symmetry_layerdataset(
        cell,
        aperiodic_dir=aperiodic_dir,
        symprec=symprec,
    )
    return dataset


def get_spacegroup(
    cell: Cell,
    symprec=1e-5,
    angle_tolerance=-1.0,
    symbol_type=0,
) -> str | None:
    """Return space group in international table symbol and number as a string.

    With ``symbol_type=1``, Schoenflies symbol is given instead of international symbol.

    :rtype: str | None
    :return:
        If it fails, None is returned.
    """
    _set_no_error()

    dataset = get_symmetry_dataset(
        cell,
        symprec=symprec,
        angle_tolerance=angle_tolerance,
    )

    if dataset is None:
        _set_error_message()
        return None

    spg_type = get_spacegroup_type(dataset.hall_number)
    if spg_type is None:
        return None

    if symbol_type == 1:
        return "%s (%d)" % (spg_type.schoenflies, dataset.number)
    else:
        return "%s (%d)" % (spg_type.international_short, dataset.number)


def get_spacegroup_type(hall_number: int) -> SpaceGroupType | None:
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
    _set_no_error()

    spg_type_list = _spglib.spacegroup_type(hall_number)

    if spg_type_list is not None:
        spg_type = SpaceGroupType(
            number=spg_type_list[0],
            international_short=spg_type_list[1].strip(),
            international_full=spg_type_list[2].strip(),
            international=spg_type_list[3].strip(),
            schoenflies=spg_type_list[4].strip(),
            hall_number=spg_type_list[5],
            hall_symbol=spg_type_list[6].strip(),
            choice=spg_type_list[7].strip(),
            pointgroup_international=spg_type_list[8].strip(),
            pointgroup_schoenflies=spg_type_list[9].strip(),
            arithmetic_crystal_class_number=spg_type_list[10],
            arithmetic_crystal_class_symbol=spg_type_list[11].strip(),
        )
        return spg_type
    else:
        _set_error_message()
        return None


def get_spacegroup_type_from_symmetry(
    rotations,
    translations,
    lattice=None,
    symprec=1e-5,
) -> SpaceGroupType | None:
    """Return space-group type information from symmetry operations.

    This is expected to work well for the set of symmetry operations whose
    distortion is small. The aim of making this feature is to find space-group-type
    for the set of symmetry operations given by the other source than spglib. The
    parameter ``lattice`` is used as the distance measure for ``symprec``. If this
    is not given, the cubic basis vector whose lengths are one is used.

    Parameters
    ----------
    rotations : array_like
        Matrix parts of space group operations.
        shape=(n_operations, 3, 3), order='C', dtype='intc'
    translations : array_like
        Vector parts of space group operations.
        shape=(n_operations, 3), order='C', dtype='double'
    lattice : array_like, optional
        Basis vectors a, b, c given in row vectors. This is used as the measure of
        distance. Default is None, which gives unit matrix.
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
        _lattice = np.array(lattice, dtype="double", order="C")

    _set_no_error()

    spg_type_list = _spglib.spacegroup_type_from_symmetry(r, t, _lattice, symprec)
    if spg_type_list is not None:
        spg_type = SpaceGroupType(
            number=spg_type_list[0],
            international_short=spg_type_list[1].strip(),
            international_full=spg_type_list[2].strip(),
            international=spg_type_list[3].strip(),
            schoenflies=spg_type_list[4].strip(),
            hall_number=spg_type_list[5],
            hall_symbol=spg_type_list[6].strip(),
            choice=spg_type_list[7].strip(),
            pointgroup_international=spg_type_list[8].strip(),
            pointgroup_schoenflies=spg_type_list[9].strip(),
            arithmetic_crystal_class_number=spg_type_list[10],
            arithmetic_crystal_class_symbol=spg_type_list[11].strip(),
        )
        return spg_type
    else:
        _set_error_message()
        return None


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

    msg_type_list = _spglib.magnetic_spacegroup_type(uni_number)

    if msg_type_list is not None:
        msg_type = MagneticSpaceGroupType(
            uni_number=msg_type_list[0],
            litvin_number=msg_type_list[1],
            bns_number=msg_type_list[2].strip(),
            og_number=msg_type_list[3].strip(),
            number=msg_type_list[4],
            type=msg_type_list[5],
        )
        return msg_type
    else:
        _set_error_message()
        return None


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
    msg_type_list = _spglib.magnetic_spacegroup_type_from_symmetry(
        rots, trans, timerev, latt, symprec
    )

    if msg_type_list is not None:
        msg_type = MagneticSpaceGroupType(
            uni_number=msg_type_list[0],
            litvin_number=msg_type_list[1],
            bns_number=msg_type_list[2].strip(),
            og_number=msg_type_list[3].strip(),
            number=msg_type_list[4],
            type=msg_type_list[5],
        )
        return msg_type
    else:
        _set_error_message()
        return None


def get_pointgroup(rotations) -> tuple[str, int, np.ndarray]:
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
    pointgroup = _spglib.pointgroup(np.array(rotations, dtype="intc", order="C"))
    _set_error_message()
    return pointgroup


def standardize_cell(
    cell: Cell,
    to_primitive=False,
    no_idealize=False,
    symprec=1e-5,
    angle_tolerance=-1.0,
) -> Cell | None:
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
    num_atom_std = _spglib.standardize_cell(
        lattice,
        positions,
        numbers,
        num_atom,
        to_primitive * 1,
        no_idealize * 1,
        symprec,
        angle_tolerance,
    )

    if num_atom_std > 0:
        return (
            np.array(lattice.T, dtype="double", order="C"),
            np.array(positions[:num_atom_std], dtype="double", order="C"),
            np.array(numbers[:num_atom_std], dtype="intc"),
        )
    else:
        _set_error_message()
        return None


def refine_cell(cell: Cell, symprec=1e-5, angle_tolerance=-1.0) -> Cell | None:
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
    num_atom_std = _spglib.refine_cell(
        lattice,
        positions,
        numbers,
        num_atom,
        symprec,
        angle_tolerance,
    )

    if num_atom_std > 0:
        return (
            np.array(lattice.T, dtype="double", order="C"),
            np.array(positions[:num_atom_std], dtype="double", order="C"),
            np.array(numbers[:num_atom_std], dtype="intc"),
        )
    else:
        _set_error_message()
        return None


def find_primitive(cell: Cell, symprec=1e-5, angle_tolerance=-1.0) -> Cell | None:
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

    num_atom_prim = _spglib.primitive(
        lattice, positions, numbers, symprec, angle_tolerance
    )

    if num_atom_prim > 0:
        return (
            np.array(lattice.T, dtype="double", order="C"),
            np.array(positions[:num_atom_prim], dtype="double", order="C"),
            np.array(numbers[:num_atom_prim], dtype="intc"),
        )
    else:
        _set_error_message()
        return None


def get_symmetry_from_database(hall_number) -> dict | None:
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
    num_sym = _spglib.symmetry_from_database(rotations, translations, hall_number)

    if num_sym is None:
        _set_error_message()
        return None
    else:
        return {
            "rotations": np.array(rotations[:num_sym], dtype="intc", order="C"),
            "translations": np.array(translations[:num_sym], dtype="double", order="C"),
        }


def get_magnetic_symmetry_from_database(uni_number, hall_number=0) -> dict | None:
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
    num_sym = _spglib.magnetic_symmetry_from_database(
        rotations,
        translations,
        time_reversals,
        uni_number,
        hall_number,
    )

    if num_sym is None:
        _set_error_message()
        return None
    else:
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
def get_grid_point_from_address(grid_address, mesh):
    """Return grid point index by translating grid address."""
    _set_no_error()

    return _spglib.grid_point_from_address(
        np.array(grid_address, dtype="intc"),
        np.array(mesh, dtype="intc"),
    )


def get_ir_reciprocal_mesh(
    mesh,
    cell,
    is_shift=None,
    is_time_reversal=True,
    symprec=1e-5,
    is_dense=False,
):
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
    if (
        _spglib.ir_reciprocal_mesh(
            grid_address,
            grid_mapping_table,
            np.array(mesh, dtype="intc"),
            np.array(is_shift, dtype="intc"),
            is_time_reversal * 1,
            lattice,
            positions,
            numbers,
            symprec,
        )
        > 0
    ):
        return grid_mapping_table, grid_address
    else:
        _set_error_message()
        return None


def get_stabilized_reciprocal_mesh(
    mesh,
    rotations,
    is_shift=None,
    is_time_reversal=True,
    qpoints=None,
    is_dense=False,
):
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

    if (
        _spglib.stabilized_reciprocal_mesh(
            grid_address,
            mapping_table,
            np.array(mesh, dtype="intc"),
            np.array(is_shift, dtype="intc"),
            is_time_reversal * 1,
            np.array(rotations, dtype="intc", order="C"),
            qpoints,
        )
        > 0
    ):
        return mapping_table, grid_address
    else:
        _set_error_message()
        return None


def get_grid_points_by_rotations(
    address_orig,
    reciprocal_rotations,
    mesh,
    is_shift=None,
    is_dense=False,
):
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
    _spglib.grid_points_by_rotations(
        rot_grid_points,
        np.array(address_orig, dtype="intc"),
        np.array(reciprocal_rotations, dtype="intc", order="C"),
        np.array(mesh, dtype="intc"),
        _is_shift,
    )

    if is_dense:
        return rot_grid_points
    else:
        return np.array(rot_grid_points, dtype="intc")


def get_BZ_grid_points_by_rotations(
    address_orig,
    reciprocal_rotations,
    mesh,
    bz_map,
    is_shift=None,
    is_dense=False,
):
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
    _spglib.BZ_grid_points_by_rotations(
        rot_grid_points,
        np.array(address_orig, dtype="intc"),
        np.array(reciprocal_rotations, dtype="intc", order="C"),
        np.array(mesh, dtype="intc"),
        _is_shift,
        _bz_map,
    )

    if is_dense:
        return rot_grid_points
    else:
        return np.array(rot_grid_points, dtype="intc")


def relocate_BZ_grid_address(
    grid_address,
    mesh,
    reciprocal_lattice,  # column vectors
    is_shift=None,
    is_dense=False,
):
    """Grid addresses are relocated to be inside first Brillouin zone.

    Number of ir-grid-points inside Brillouin zone is returned.
    It is assumed that the following arrays have the shapes of
        bz_grid_address : (num_grid_points_in_FBZ, 3)
        bz_map (prod(mesh * 2), )

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
    num_bz_ir = _spglib.BZ_grid_address(
        bz_grid_address,
        bz_map,
        grid_address,
        np.array(mesh, dtype="intc"),
        np.array(reciprocal_lattice, dtype="double", order="C"),
        _is_shift,
    )

    if is_dense:
        return bz_grid_address[:num_bz_ir], bz_map
    else:
        return bz_grid_address[:num_bz_ir], np.array(bz_map, dtype="intc")


def delaunay_reduce(lattice, eps=1e-5):
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
    result = _spglib.delaunay_reduce(delaunay_lattice, float(eps))

    if result == 0:
        _set_error_message()
        return None
    else:
        return np.array(np.transpose(delaunay_lattice), dtype="double", order="C")


def niggli_reduce(lattice, eps=1e-5):
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
    result = _spglib.niggli_reduce(niggli_lattice, float(eps))

    if result == 0:
        _set_error_message()
        return None
    else:
        return np.array(np.transpose(niggli_lattice), dtype="double", order="C")


def get_error_message():
    """Return error message why spglib failed.

    .. warning::
        This method is not thread safe, i.e., only safely usable
        when calling one spglib method per process.

    Notes
    -----
    .. versionadded:: 1.9.5

    """
    return spglib_error.message


def _expand_cell(
    cell: Cell,
) -> tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray | None]:
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
        raise TypeError("numbers has to have the same number of atoms as positions.")
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

    return (lattice, positions, numbers, magmoms)


def _set_error_message():
    spglib_error.message = _spglib.error_message()


def _set_no_error():
    spglib_error.message = "no error"


def get_hall_number_from_symmetry(rotations, translations, symprec=1e-5) -> int | None:
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
    warnings.warn(
        "get_hall_number_from_symmetry() is deprecated. "
        "Use get_spacegroup_type_from_symmetry() instead.",
        DeprecationWarning,
    )

    r = np.array(rotations, dtype="intc", order="C")
    t = np.array(translations, dtype="double", order="C")
    hall_number = _spglib.hall_number_from_symmetry(r, t, symprec)
    return hall_number
