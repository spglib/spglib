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

import warnings

import numpy as np

try:
    from spglib import _spglib as spg
except ImportError:
    import os.path
    import re
    from ctypes import cdll

    bundled_lib = next(
        filter(
            lambda fl: re.match(".*symspg\\..*", fl),
            sorted(os.listdir(os.path.dirname(__file__))),
        ),
        None,
    )
    if not bundled_lib:
        raise FileNotFoundError(
            "Spglib C++ library is not installed and no bundled version was detected"
        )
    cdll.LoadLibrary(os.path.join(os.path.dirname(__file__), bundled_lib))
    from spglib import _spglib as spg


class SpglibError(object):
    """Error message why spglib failed."""

    message = "no error"


spglib_error = SpglibError()


def get_version():
    """Version number of spglib."""
    _set_no_error()
    return tuple(spg.version())


def get_symmetry(
    cell, symprec=1e-5, angle_tolerance=-1.0, mag_symprec=-1.0, is_magnetic=True
):
    """Find symmetry operations from a crystal structure and site tensors.

    Parameters
    ----------
    cell : tuple
        Crystal structure given either in tuple or Atoms object (deprecated).
        In the case given by a tuple, it has to follow the form below,

        (basis vectors, atomic points, types in integer numbers, ...)

        basis vectors : array_like
            [[a_x, a_y, a_z],
             [b_x, b_y, b_z],
             [c_x, c_y, c_z]]
            shape=(3, 3), order='C', dtype='double'
        atomic points : array_like
            Atomic position vectors with respect to basis vectors, i.e.,
            given in  fractional coordinates.
            shape=(num_atom, 3), order='C', dtype='double'
        types : array_like
            Integer numbers to distinguish species.
            shape=(num_atom, ), dtype='intc'
        optional data :
            case-I: Scalar
                Each atomic site has a scalar value. With is_magnetic=True,
                values are included in the symmetry search in a way of
                collinear magnetic moments.
                shape=(num_atom, ), dtype='double'
            case-II: Vectors
                Each atomic site has a vector. With is_magnetic=True,
                vectors are included in the symmetry search in a way of
                non-collinear magnetic moments.
                shape=(num_atom, 3), order='C', dtype='double'
    symprec : float
        Symmetry search tolerance in the unit of length.
    angle_tolerance : float
        Symmetry search tolerance in the unit of angle deg. If the value is
        negative, an internally optimized routine is used to judge symmetry.
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
    dictionary
        Rotation parts and translation parts of symmetry operations represented
        with respect to basis vectors and atom index mapping by symmetry
        operations.
        'rotations' : ndarray
            Rotation (matrix) parts of symmetry operations
            shape=(num_operations, 3, 3), order='C', dtype='intc'
        'translations' : ndarray
            Translation (vector) parts of symmetry operations
            shape=(num_operations, 3), dtype='double'
        'time_reversals': ndarray (exists when the optional data is given)
            Time reversal part of magnetic symmetry operations.
            True indicates time reversal operation, and False indicates
            an ordinary operation.
            shape=(num_operations, ), dtype='bool_'
        'equivalent_atoms' : ndarray
            shape=(num_atoms, ), dtype='intc'

    """
    _set_no_error()

    lattice, _, _, magmoms = _expand_cell(cell)
    if lattice is None:
        return None

    if magmoms is None:
        # Get symmetry operations without on-site tensors (i.e. normal crystal)
        dataset = get_symmetry_dataset(
            cell, symprec=symprec, angle_tolerance=angle_tolerance
        )
        if dataset is None:
            return None

        return {
            "rotations": dataset["rotations"],
            "translations": dataset["translations"],
            "equivalent_atoms": dataset["equivalent_atoms"],
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
    cell,
    symprec=1e-5,
    angle_tolerance=-1.0,
    mag_symprec=-1.0,
    is_axial=None,
    with_time_reversal=True,
):
    """Find magnetic symmetry operations from a crystal structure and site tensors.

    Parameters
    ----------
    cell : tuple
        Crystal structure given either in tuple or Atoms object (deprecated).
        In the case given by a tuple, it has to follow the form below,

        (basis vectors, atomic points, types in integer numbers, ...)

        basis vectors : array_like
            [[a_x, a_y, a_z],
             [b_x, b_y, b_z],
             [c_x, c_y, c_z]]
            shape=(3, 3), order='C', dtype='double'
        atomic points : array_like
            Atomic position vectors with respect to basis vectors, i.e.,
            given in  fractional coordinates.
            shape=(num_atom, 3), order='C', dtype='double'
        types : array_like
            Integer numbers to distinguish species.
            shape=(num_atom, ), dtype='intc'
        magmoms:
            case-I: Scalar
                Each atomic site has a scalar value. With is_magnetic=True,
                values are included in the symmetry search in a way of
                collinear magnetic moments.
                shape=(num_atom, ), dtype='double'
            case-II: Vectors
                Each atomic site has a vector. With is_magnetic=True,
                vectors are included in the symmetry search in a way of
                non-collinear magnetic moments.
                shape=(num_atom, 3), order='C', dtype='double'
    symprec : float
        Symmetry search tolerance in the unit of length.
    angle_tolerance : float
        Symmetry search tolerance in the unit of angle deg. If the value is
        negative, an internally optimized routine is used to judge symmetry.
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
    dictionary
        Rotation parts and translation parts of symmetry operations represented
        with respect to basis vectors and atom index mapping by symmetry
        operations.
        'rotations' : ndarray
            Rotation (matrix) parts of symmetry operations
            shape=(num_operations, 3, 3), order='C', dtype='intc'
        'translations' : ndarray
            Translation (vector) parts of symmetry operations
            shape=(num_operations, 3), dtype='double'
        'time_reversals': ndarray (exists when the optional data is given)
            Time reversal part of magnetic symmetry operations.
            True indicates time reversal operation, and False indicates
            an ordinary operation.
            shape=(num_operations, ), dtype='bool_'
        'equivalent_atoms' : ndarray
            shape=(num_atoms, ), dtype='intc'
    """
    _set_no_error()

    lattice, positions, numbers, magmoms = _expand_cell(cell)
    if lattice is None:
        return None
    if magmoms is None:
        warnings.warn("Specify magnetic moments in cell.")
        return None

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

    num_sym = spg.symmetry_with_site_tensors(
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

    _set_error_message()
    if num_sym == 0:
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
            "primitive_lattice": primitive_lattice,
        }


def _build_dataset_dict(spg_ds):
    keys = (
        "number",
        "hall_number",
        "international",
        "hall",
        "choice",
        "transformation_matrix",
        "origin_shift",
        "rotations",
        "translations",
        "wyckoffs",
        "site_symmetry_symbols",
        "crystallographic_orbits",
        "equivalent_atoms",
        "primitive_lattice",
        "mapping_to_primitive",
        "std_lattice",
        "std_types",
        "std_positions",
        "std_rotation_matrix",
        "std_mapping_to_primitive",
        # 'pointgroup_number',
        "pointgroup",
    )
    dataset = {}
    for key, data in zip(keys, spg_ds):
        dataset[key] = data

    dataset["international"] = dataset["international"].strip()
    dataset["hall"] = dataset["hall"].strip()
    dataset["choice"] = dataset["choice"].strip()
    dataset["transformation_matrix"] = np.array(
        dataset["transformation_matrix"], dtype="double", order="C"
    )
    dataset["origin_shift"] = np.array(dataset["origin_shift"], dtype="double")
    dataset["rotations"] = np.array(dataset["rotations"], dtype="intc", order="C")
    dataset["translations"] = np.array(
        dataset["translations"], dtype="double", order="C"
    )
    letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    dataset["wyckoffs"] = [letters[x] for x in dataset["wyckoffs"]]
    dataset["site_symmetry_symbols"] = [
        s.strip() for s in dataset["site_symmetry_symbols"]
    ]
    dataset["crystallographic_orbits"] = np.array(
        dataset["crystallographic_orbits"], dtype="intc"
    )
    dataset["equivalent_atoms"] = np.array(dataset["equivalent_atoms"], dtype="intc")
    dataset["primitive_lattice"] = np.array(
        np.transpose(dataset["primitive_lattice"]), dtype="double", order="C"
    )
    dataset["mapping_to_primitive"] = np.array(
        dataset["mapping_to_primitive"], dtype="intc"
    )
    dataset["std_lattice"] = np.array(
        np.transpose(dataset["std_lattice"]), dtype="double", order="C"
    )
    dataset["std_types"] = np.array(dataset["std_types"], dtype="intc")
    dataset["std_positions"] = np.array(
        dataset["std_positions"], dtype="double", order="C"
    )
    dataset["std_rotation_matrix"] = np.array(
        dataset["std_rotation_matrix"], dtype="double", order="C"
    )
    dataset["std_mapping_to_primitive"] = np.array(
        dataset["std_mapping_to_primitive"], dtype="intc"
    )
    dataset["pointgroup"] = dataset["pointgroup"].strip()
    return dataset


def get_symmetry_dataset(cell, symprec=1e-5, angle_tolerance=-1.0, hall_number=0):
    """Search symmetry dataset from an input cell.

    Parameters
    ----------
    cell, symprec, angle_tolerance:
        See the docstring of get_symmetry.
    hall_number : int
        If a serial number of Hall symbol (>0) is given, the database
        corresponding to the Hall symbol is made.

    Returns
    -------
    dict or None
        If it fails, None is returned. Otherwise a dictionary is returned.
        Dictionary keys are as follows:

        number : int
            International space group number.
        international : str
            International symbol.
        hall : str
            Hall symbol.
        choice : str
            Centring, origin, basis vector setting.
        transformation_matrix : ndarray
            Transformation matrix from input lattice to standardized lattice:
                L^original = L^standardized * Tmat.
            shape=(3, 3), order='C', dtype='double'
        origin shift : ndarray
            Origin shift from standardized to input origin.
            shape=(3,), dtype='double'
        rotations : ndarray
            Matrix (rotation) parts of space group operations. Space group
            operations are obtained by [(r,t) for r, t in zip(rotations,
            translations)].
            shape=(n_operations, 3, 3), order='C', dtype='intc'
        translations : ndarray
            Vector (translation) parts of space group operations. Space group
            operations are obtained by [(r,t) for r, t in zip(rotations,
            translations)].
            shape=(n_operations, 3), order='C', dtype='double'
        wyckoffs : list of str
            Wyckoff letters corresponding to the space group type.
        site_symmetry_symbols : list of str
            Site symmetry symbols corresponding to the space group type.
        equivalent_atoms : ndarray
            Symmetrically equivalent atoms, where 'symmetrically' means found
            symmetry operations. In spglib, symmetry operations are given for
            the input cell. When a non-primitive cell is inputted and the
            lattice made by the non-primitive basis vectors breaks its point
            group, the found symmetry operations may not correspond to the
            crystallographic space group type.
            shape=(n_atoms,), dtype='intc'
        crystallographic_orbits : ndarray
            Symmetrically equivalent atoms, where 'symmetrically' means the
            space group operations corresponding to the space group type. This
            is very similar to ``equivalent_atoms``. See the difference
            explained in ``equivalent_atoms``
            shape=(n_atoms,), dtype='intc'
        Primitive cell :
            primitive_lattice : ndarray
                Basis vectors a, b, c of a primitive cell in row vectors. This
                is the primitive cell found inside spglib before applying
                standardization. Therefore, the values are distorted with
                respect to found space group type.
                shape=(3, 3), order='C', dtype='double'
            mapping_to_primitive : ndarray
                Atom index mapping from original cell to the primitive cell of
                ``primitive_lattice``.
                shape=(n_atoms), dtype='intc'
        Idealized standardized unit cell :
            std_lattice : ndarray
                Basis vectors a, b, c of a standardized cell in row vectors.
                shape=(3, 3), order='C', dtype='double'
            std_positions : ndarray
                Positions of atoms in the standardized cell in fractional
                coordinates.
                shape=(n_atoms, 3), order='C', dtype='double'
            std_types : ndarray
                Identity numbers of atoms in the standarzied cell.
                shape=(n_atoms,), dtype='intc'
        std_rotation_matrix : ndarray
            Rigid rotation matrix to rotate from standardized basis vectors to
            idealized standardized basis vectors. Orthonormalized.
                L^idealized = R * L^standardized.
            shape=(3, 3), order='C', dtype='double'
        std_mapping_to_primitive :
            ``std_positions`` index mapping to those of atoms of the primitive
            cell in the standardized cell.
            dtype='intc'
        pointgroup : str
            Pointgroup symbol.

    """
    _set_no_error()

    lattice, positions, numbers, _ = _expand_cell(cell)
    if lattice is None:
        return None

    spg_ds = spg.dataset(
        lattice, positions, numbers, hall_number, symprec, angle_tolerance
    )
    if spg_ds is None:
        _set_error_message()
        return None

    dataset = _build_dataset_dict(spg_ds)
    _set_error_message()
    return dataset


def get_symmetry_layerdataset(cell, aperiodic_dir=2, symprec=1e-5):
    """TODO: Add comments."""
    _set_no_error()

    lattice, positions, numbers, _ = _expand_cell(cell)
    if lattice is None:
        return None

    spg_ds = spg.layerdataset(
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

    _set_error_message()
    return dataset


def get_magnetic_symmetry_dataset(
    cell,
    is_axial=None,
    symprec=1e-5,
    angle_tolerance=-1.0,
    mag_symprec=-1.0,
):
    """Search magnetic symmetry dataset from an input cell.

    Parameters
    ----------
    cell, is_axial, symprec, angle_tolerance, mag_symprec:
        See the docstring of get_magnetic_symmetry.

    Returns
    -------
    dict or None
        If it fails, None is returned. Otherwise a dictionary is returned.
        Dictionary keys are as follows:

        Magnetic space-group type
            uni_number: int
                UNI number between 1 to 1651
            msg_type: int
                Magnetic space groups (MSG) is classified by its family space
                group (FSG) and maximal space subgroup (XSG). FSG is a non-magnetic
                space group obtained by ignoring time-reversal term in MSG. XSG is
                a space group obtained by picking out non time-reversal operations
                in MSG.

                msg_type==1 (type-I): MSG, XSG, FSG are all isomorphic.
                msg_type==2 (type-II): XSG and FSG are isomorphic, and MSG is
                                        generated from XSG and pure time reversal
                                        operations.
                msg_type==3 (type-III): XSG is a proper subgroup of MSG with
                                        isomorphic translational subgroups.
                msg_type==4 (type-IV): XSG is a proper subgroup of MSG with
                                        isomorphic point group.
            hall_number: int
                For type-I, II, III, Hall number of FSG; for type-IV, that of XSG
            tensor_rank: int
        Magnetic symmetry operations
            n_operations: int
            rotations: array, (n_operations, 3, 3)
                Rotation (matrix) parts of symmetry operations
            translations: array, (n_operations, 3)
                Translation (vector) parts of symmetry operations
            time_reversals: array, (n_operations, )
                Time reversal part of magnetic symmetry operations.
                True indicates time reversal operation, and False indicates
                an ordinary operation.
        Equivalent atoms
            n_atoms: int
            equivalent_atoms: array
                See the docstring of get_symmetry_dataset
        Transformation to standardized setting
            transformation_matrix: array, (3, 3)
                Transformation matrix from input lattice to standardized
            origin_shift: array, (3, )
                Origin shift from standardized to input origin
        Standardized crystal structure
            n_std_atoms: int
                Number of atoms in standardized unit cell
            std_lattice: array, (3, 3)
                Row-wise lattice vectors
            std_types: array, (n_std_atoms, )
            std_positions: array, (n_std_atoms, 3)
            std_tensors: array
                (n_std_atoms, ) for collinear magnetic moments.
                (n_std_atoms, 3) for vector non-collinear magnetic moments.
            std_rotation_matrix
                Rigid rotation matrix to rotate from standardized basis
                vectors to idealized standardized basis vectors
        Intermediate data in symmetry search
            primitive_lattice: array, (3, 3)

    """
    _set_no_error()

    lattice, positions, numbers, magmoms = _expand_cell(cell)
    if lattice is None:
        return None

    tensor_rank = magmoms.ndim - 1

    # If is_axial is not specified, select collinear or non-collinear spin cases
    if is_axial is None:
        if tensor_rank == 0:
            is_axial = False  # Collinear spin
        elif tensor_rank == 1:
            is_axial = True  # Non-collinear spin

    spg_ds = spg.magnetic_dataset(
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

    keys = (
        # Magnetic space-group type
        "uni_number",
        "msg_type",
        "hall_number",
        "tensor_rank",
        # Magnetic symmetry operations
        "n_operations",
        "rotations",
        "translations",
        "time_reversals",
        # Equivalent atoms
        "n_atoms",
        "equivalent_atoms",
        # Transformation to standardized setting
        "transformation_matrix",
        "origin_shift",
        # Standardized crystal structure
        "n_std_atoms",
        "std_lattice",
        "std_types",
        "std_positions",
        "std_tensors",
        "std_rotation_matrix",
        # Intermidiate datum in symmetry search
        "primitive_lattice",
    )
    dataset = {}
    for key, data in zip(keys, spg_ds):
        dataset[key] = data
    dataset["rotations"] = np.array(dataset["rotations"], dtype="intc", order="C")
    dataset["translations"] = np.array(
        dataset["translations"], dtype="double", order="C"
    )
    dataset["time_reversals"] = (
        np.array(dataset["time_reversals"], dtype="intc", order="C") == 1
    )
    dataset["equivalent_atoms"] = np.array(dataset["equivalent_atoms"], dtype="intc")
    dataset["transformation_matrix"] = np.array(
        dataset["transformation_matrix"], dtype="double", order="C"
    )
    dataset["origin_shift"] = np.array(dataset["origin_shift"], dtype="double")
    dataset["std_lattice"] = np.array(
        np.transpose(dataset["std_lattice"]), dtype="double", order="C"
    )
    dataset["std_types"] = np.array(dataset["std_types"], dtype="intc")
    dataset["std_positions"] = np.array(
        dataset["std_positions"], dtype="double", order="C"
    )
    dataset["std_rotation_matrix"] = np.array(
        dataset["std_rotation_matrix"], dtype="double", order="C"
    )
    dataset["primitive_lattice"] = np.array(
        np.transpose(dataset["primitive_lattice"]), dtype="double", order="C"
    )

    dataset["std_tensors"] = np.array(dataset["std_tensors"], dtype="double", order="C")
    if tensor_rank == 1:
        dataset["std_tensors"] = dataset["std_tensors"].reshape(-1, 3)

    _set_error_message()
    return dataset


def get_layergroup(cell, aperiodic_dir=2, symprec=1e-5):
    """Return layer group in ....

    If it fails, None is returned.

    """
    _set_no_error()

    dataset = get_symmetry_layerdataset(
        cell, aperiodic_dir=aperiodic_dir, symprec=symprec
    )
    return dataset


def get_spacegroup(cell, symprec=1e-5, angle_tolerance=-1.0, symbol_type=0):
    """Return space group in international table symbol and number as a string.

    If it fails, None is returned.

    """
    _set_no_error()

    dataset = get_symmetry_dataset(
        cell, symprec=symprec, angle_tolerance=angle_tolerance
    )

    if dataset is None:
        return None

    spg_type = get_spacegroup_type(dataset["hall_number"])
    if symbol_type == 1:
        return "%s (%d)" % (spg_type["schoenflies"], dataset["number"])
    else:
        return "%s (%d)" % (spg_type["international_short"], dataset["number"])


def get_spacegroup_type(hall_number):
    """Translate Hall number to space group type information.

    Parameters
    ----------
    hall_number : int
        Hall symbol ID.

    Returns
    -------
    dict or None
        If it fails, None is returned. Otherwise a dictionary is returned.
        Dictionary keys are as follows:

        number : int
            International space group number
        international_short : str
            International short symbol.
        international_full : str
            International full symbol.
        international : str
            International symbol.
        schoenflies : str
            Schoenflies symbol.
        hall_number : int
            Hall symbol ID number.
        hall_symbol : str
            Hall symbol.
        choice : str
            Centring, origin, basis vector setting.
        pointgroup_international :
            International symbol of crystallographic point group.
        pointgroup_schoenflies :
            Schoenflies symbol of crystallographic point group.
        arithmetic_crystal_class_number : int
            Arithmetic crystal class number
        arithmetic_crystal_class_symbol : str
            Arithmetic crystal class symbol.

    """
    _set_no_error()

    keys = (
        "number",
        "international_short",
        "international_full",
        "international",
        "schoenflies",
        "hall_number",
        "hall_symbol",
        "choice",
        "pointgroup_international",
        "pointgroup_schoenflies",
        "arithmetic_crystal_class_number",
        "arithmetic_crystal_class_symbol",
    )
    spg_type_list = spg.spacegroup_type(hall_number)
    _set_error_message()

    if spg_type_list is not None:
        spg_type = dict(zip(keys, spg_type_list))
        for key in spg_type:
            if key not in ("number", "hall_number", "arithmetic_crystal_class_number"):
                spg_type[key] = spg_type[key].strip()
        return spg_type
    else:
        return None


def get_spacegroup_type_from_symmetry(
    rotations, translations, lattice=None, symprec=1e-5
):
    """Space group type information is obtained from a set of symmetry operations.

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

    Returns
    -------
    dict or None
        If it fails, None is returned. Otherwise a dictionary is returned.
        Dictionary keys are as follows:

        number : int
            International space group number
        international_short : str
            International short symbol.
        international_full : str
            International full symbol.
        international : str
            International symbol.
        schoenflies : str
            Schoenflies symbol.
        hall_number : int
            Hall symbol ID number.
        hall_symbol : str
            Hall symbol.
        choice : str
            Centring, origin, basis vector setting.
        pointgroup_international :
            International symbol of crystallographic point group.
        pointgroup_schoenflies :
            Schoenflies symbol of crystallographic point group.
        arithmetic_crystal_class_number : int
            Arithmetic crystal class number
        arithmetic_crystal_class_symbol : str
            Arithmetic crystal class symbol.

    """
    r = np.array(rotations, dtype="intc", order="C")
    t = np.array(translations, dtype="double", order="C")
    if lattice is None:
        _lattice = np.eye(3, dtype="double", order="C")
    else:
        _lattice = np.array(lattice, dtype="double", order="C")

    _set_no_error()

    keys = (
        "number",
        "international_short",
        "international_full",
        "international",
        "schoenflies",
        "hall_number",
        "hall_symbol",
        "choice",
        "pointgroup_international",
        "pointgroup_schoenflies",
        "arithmetic_crystal_class_number",
        "arithmetic_crystal_class_symbol",
    )

    spg_type_list = spg.spacegroup_type_from_symmetry(r, t, _lattice, symprec)
    _set_error_message()

    if spg_type_list is not None:
        spg_type = dict(zip(keys, spg_type_list))
        for key in spg_type:
            if key not in ("number", "hall_number", "arithmetic_crystal_class_number"):
                spg_type[key] = spg_type[key].strip()
        return spg_type
    else:
        return None


def get_magnetic_spacegroup_type(uni_number):
    """Translate UNI number to magnetic space group type information.

    If it fails, return None

    """
    _set_no_error()

    keys = (
        "uni_number",
        "litvin_number",
        "bns_number",
        "og_number",
        "number",
        "type",
    )
    msg_type_list = spg.magnetic_spacegroup_type(uni_number)
    _set_error_message()

    if msg_type_list is not None:
        msg_type = dict(zip(keys, msg_type_list))
        for key in msg_type:
            if key not in ["uni_number", "litvin_number", "number", "type"]:
                msg_type[key] = msg_type[key].strip()
        return msg_type
    else:
        return None


def get_pointgroup(rotations):
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
    pointgroup = spg.pointgroup(np.array(rotations, dtype="intc", order="C"))
    _set_error_message()
    return pointgroup


def standardize_cell(
    cell, to_primitive=False, no_idealize=False, symprec=1e-5, angle_tolerance=-1.0
):
    """Return standardized cell.

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

    """
    _set_no_error()

    lattice, _positions, _numbers, _ = _expand_cell(cell)
    if lattice is None:
        return None

    # Atomic positions have to be specified by scaled positions for spglib.
    num_atom = len(_positions)
    positions = np.zeros((num_atom * 4, 3), dtype="double", order="C")
    positions[:num_atom] = _positions
    numbers = np.zeros(num_atom * 4, dtype="intc")
    numbers[:num_atom] = _numbers
    num_atom_std = spg.standardize_cell(
        lattice,
        positions,
        numbers,
        num_atom,
        to_primitive * 1,
        no_idealize * 1,
        symprec,
        angle_tolerance,
    )
    _set_error_message()

    if num_atom_std > 0:
        return (
            np.array(lattice.T, dtype="double", order="C"),
            np.array(positions[:num_atom_std], dtype="double", order="C"),
            np.array(numbers[:num_atom_std], dtype="intc"),
        )
    else:
        return None


def refine_cell(cell, symprec=1e-5, angle_tolerance=-1.0):
    """Return refined cell.

    The standardized unit cell is returned by a tuple of
    (lattice, positions, numbers).
    If it fails, None is returned.
    """
    _set_no_error()

    lattice, _positions, _numbers, _ = _expand_cell(cell)
    if lattice is None:
        return None

    # Atomic positions have to be specified by scaled positions for spglib.
    num_atom = len(_positions)
    positions = np.zeros((num_atom * 4, 3), dtype="double", order="C")
    positions[:num_atom] = _positions
    numbers = np.zeros(num_atom * 4, dtype="intc")
    numbers[:num_atom] = _numbers
    num_atom_std = spg.refine_cell(
        lattice, positions, numbers, num_atom, symprec, angle_tolerance
    )
    _set_error_message()

    if num_atom_std > 0:
        return (
            np.array(lattice.T, dtype="double", order="C"),
            np.array(positions[:num_atom_std], dtype="double", order="C"),
            np.array(numbers[:num_atom_std], dtype="intc"),
        )
    else:
        return None


def find_primitive(cell, symprec=1e-5, angle_tolerance=-1.0):
    """Primitive cell is searched in the input cell.

    The primitive cell is returned by a tuple of (lattice, positions, numbers).
    If it fails, None is returned.
    """
    _set_no_error()

    lattice, positions, numbers, _ = _expand_cell(cell)
    if lattice is None:
        return None

    num_atom_prim = spg.primitive(lattice, positions, numbers, symprec, angle_tolerance)
    _set_error_message()

    if num_atom_prim > 0:
        return (
            np.array(lattice.T, dtype="double", order="C"),
            np.array(positions[:num_atom_prim], dtype="double", order="C"),
            np.array(numbers[:num_atom_prim], dtype="intc"),
        )
    else:
        return None


def get_symmetry_from_database(hall_number):
    """Return symmetry operations corresponding to a Hall symbol.

    The Hall symbol is given by the serial number in between 1 and 530.
    The symmetry operations are given by a dictionary whose keys are
    'rotations' and 'translations'.
    If it fails, None is returned.
    """
    _set_no_error()

    rotations = np.zeros((192, 3, 3), dtype="intc")
    translations = np.zeros((192, 3), dtype="double")
    num_sym = spg.symmetry_from_database(rotations, translations, hall_number)
    _set_error_message()

    if num_sym is None:
        return None
    else:
        return {
            "rotations": np.array(rotations[:num_sym], dtype="intc", order="C"),
            "translations": np.array(translations[:num_sym], dtype="double", order="C"),
        }


def get_magnetic_symmetry_from_database(uni_number, hall_number=0):
    """Return magnetic symmetry operations from UNI number between 1 and 1651.

    Optionally alternative settings can be specified with Hall number.

    The magnetic symmetry operations are given by a dictionary whose keys are
    'rotations', 'translations', and 'time_reversals'.
    For `time_reversals`, 0 and 1 indicate ordinary and anti-time-reversal
    operations, respectively.
    If it fails, None is returned.
    """
    _set_no_error()

    rotations = np.zeros((384, 3, 3), dtype="intc")
    translations = np.zeros((384, 3), dtype="double")
    time_reversals = np.zeros(384, dtype="intc")
    num_sym = spg.magnetic_symmetry_from_database(
        rotations, translations, time_reversals, uni_number, hall_number
    )
    _set_error_message()

    if num_sym is None:
        return None
    else:
        return {
            "rotations": np.array(rotations[:num_sym], dtype="intc", order="C"),
            "translations": np.array(translations[:num_sym], dtype="double", order="C"),
            "time_reversals": np.array(
                time_reversals[:num_sym], dtype="intc", order="C"
            ),
        }


############
# k-points #
############
def get_grid_point_from_address(grid_address, mesh):
    """Return grid point index by translating grid address."""
    _set_no_error()

    return spg.grid_point_from_address(
        np.array(grid_address, dtype="intc"), np.array(mesh, dtype="intc")
    )


def get_ir_reciprocal_mesh(
    mesh, cell, is_shift=None, is_time_reversal=True, symprec=1e-5, is_dense=False
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
        spg.ir_reciprocal_mesh(
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
        return None


def get_stabilized_reciprocal_mesh(
    mesh, rotations, is_shift=None, is_time_reversal=True, qpoints=None, is_dense=False
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
        spg.stabilized_reciprocal_mesh(
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
        return None


def get_grid_points_by_rotations(
    address_orig, reciprocal_rotations, mesh, is_shift=None, is_dense=False
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
    spg.grid_points_by_rotations(
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
    address_orig, reciprocal_rotations, mesh, bz_map, is_shift=None, is_dense=False
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

    if bz_map.dtype == "uintp" and bz_map.flags.c_contiguous:
        _bz_map = bz_map
    else:
        _bz_map = np.array(bz_map, dtype="uintp")

    rot_grid_points = np.zeros(len(reciprocal_rotations), dtype="uintp")
    spg.BZ_grid_points_by_rotations(
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
    num_bz_ir = spg.BZ_grid_address(
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
    """Run Delaunay reduction.

    Args:
        lattice: Lattice parameters in the form of
            [[a_x, a_y, a_z],
             [b_x, b_y, b_z],
             [c_x, c_y, c_z]]
        symprec:
            float: Tolerance to check if volume is close to zero or not and
                   if two basis vectors are orthogonal by the value of dot
                   product being close to zero or not.

    Returns:
        if the Delaunay reduction succeeded:
            Reduced lattice parameters are given as a numpy 'double' array:
            [[a_x, a_y, a_z],
             [b_x, b_y, b_z],
             [c_x, c_y, c_z]]
        otherwise None is returned.
    """
    _set_no_error()

    delaunay_lattice = np.array(np.transpose(lattice), dtype="double", order="C")
    result = spg.delaunay_reduce(delaunay_lattice, float(eps))
    _set_error_message()

    if result == 0:
        return None
    else:
        return np.array(np.transpose(delaunay_lattice), dtype="double", order="C")


def niggli_reduce(lattice, eps=1e-5):
    """Run Niggli reduction.

    Args:
        lattice: Lattice parameters in the form of
            [[a_x, a_y, a_z],
             [b_x, b_y, b_z],
             [c_x, c_y, c_z]]
        eps:
            float: Tolerance to check if difference of norms of two basis
                   vectors is close to zero or not and if two basis vectors are
                   orthogonal by the value of dot product being close to zero or
                   not. The detail is shown at
                   https://atztogo.github.io/niggli/.

    Returns:
        if the Niggli reduction succeeded:
            Reduced lattice parameters are given as a numpy 'double' array:
            [[a_x, a_y, a_z],
             [b_x, b_y, b_z],
             [c_x, c_y, c_z]]
        otherwise None is returned.
    """
    _set_no_error()

    niggli_lattice = np.array(np.transpose(lattice), dtype="double", order="C")
    result = spg.niggli_reduce(niggli_lattice, float(eps))
    _set_error_message()

    if result == 0:
        return None
    else:
        return np.array(np.transpose(niggli_lattice), dtype="double", order="C")


def get_error_message():
    """Return error message why spglib failed."""
    return spglib_error.message


def _expand_cell(cell):
    if isinstance(cell, tuple):
        lattice = np.array(np.transpose(cell[0]), dtype="double", order="C")
        positions = np.array(cell[1], dtype="double", order="C")
        numbers = np.array(cell[2], dtype="intc")
        if len(cell) > 3:
            magmoms = np.array(cell[3], order="C", dtype="double")
        else:
            magmoms = None
    else:
        warnings.warn("ASE Atoms-like input is deprecated.", DeprecationWarning)
        lattice = np.array(cell.get_cell().T, dtype="double", order="C")
        positions = np.array(cell.get_scaled_positions(), dtype="double", order="C")
        numbers = np.array(cell.get_atomic_numbers(), dtype="intc")
        magmoms = None

    if _check(lattice, positions, numbers, magmoms):
        return (lattice, positions, numbers, magmoms)
    else:
        return (None, None, None, None)


def _check(lattice, positions, numbers, magmoms):
    if lattice.shape != (3, 3):
        return False
    if positions.ndim != 2:
        return False
    if positions.shape[1] != 3:
        return False
    if numbers.ndim != 1:
        return False
    if len(numbers) != positions.shape[0]:
        return False
    if magmoms is not None:
        if len(magmoms) != len(numbers):
            return False
    return True


def _set_error_message():
    spglib_error.message = spg.error_message()


def _set_no_error():
    spglib_error.message = "no error"


def get_hall_number_from_symmetry(rotations, translations, symprec=1e-5):
    """Hall number is obtained from a set of symmetry operations.

    Deprecated at v2.0.

    If it fails, None is returned.

    """
    warnings.warn(
        "get_hall_number_from_symmetry() is deprecated. "
        "Use get_spacegroup_type_from_symmetry() instead.",
        DeprecationWarning,
    )

    r = np.array(rotations, dtype="intc", order="C")
    t = np.array(translations, dtype="double", order="C")
    hall_number = spg.hall_number_from_symmetry(r, t, symprec)
    return hall_number
