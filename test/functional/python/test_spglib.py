from __future__ import annotations

import pathlib
from typing import TYPE_CHECKING

import numpy as np
import pytest
import spglib
from spglib import (
    MagneticSpaceGroupType,
    find_primitive,
    get_magnetic_spacegroup_type,
    get_magnetic_symmetry_from_database,
    get_pointgroup,
    get_spacegroup,
    get_spacegroup_type,
    get_symmetry_dataset,
    standardize_cell,
)

if TYPE_CHECKING:
    from conftest import CrystalData

# fmt: off
spg_to_hall = [
    1,   2,   3,   6,   9,   18,  21,  30,  39,  57,  # noqa: E241
    60,  63,  72,  81,  90,  108, 109, 112, 115, 116,  # noqa: E241
    119, 122, 123, 124, 125, 128, 134, 137, 143, 149,
    155, 161, 164, 170, 173, 176, 182, 185, 191, 197,
    203, 209, 212, 215, 218, 221, 227, 228, 230, 233,
    239, 245, 251, 257, 263, 266, 269, 275, 278, 284,
    290, 292, 298, 304, 310, 313, 316, 322, 334, 335,
    337, 338, 341, 343, 349, 350, 351, 352, 353, 354,
    355, 356, 357, 358, 359, 361, 363, 364, 366, 367,
    368, 369, 370, 371, 372, 373, 374, 375, 376, 377,
    378, 379, 380, 381, 382, 383, 384, 385, 386, 387,
    388, 389, 390, 391, 392, 393, 394, 395, 396, 397,
    398, 399, 400, 401, 402, 404, 406, 407, 408, 410,
    412, 413, 414, 416, 418, 419, 420, 422, 424, 425,
    426, 428, 430, 431, 432, 433, 435, 436, 438, 439,
    440, 441, 442, 443, 444, 446, 447, 448, 449, 450,
    452, 454, 455, 456, 457, 458, 460, 462, 463, 464,
    465, 466, 467, 468, 469, 470, 471, 472, 473, 474,
    475, 476, 477, 478, 479, 480, 481, 482, 483, 484,
    485, 486, 487, 488, 489, 490, 491, 492, 493, 494,
    495, 497, 498, 500, 501, 502, 503, 504, 505, 506,
    507, 508, 509, 510, 511, 512, 513, 514, 515, 516,
    517, 518, 520, 521, 523, 524, 525, 527, 529, 530, 531]
# fmt: on


def get_spgnum(name: str) -> int:
    """Get number reference from filename."""
    file_name = pathlib.Path(name).stem
    spgnum = int(file_name.split("_")[1])
    return spgnum


def test_get_symmetry_dataset(crystal_data: CrystalData):
    symprec = 1e-5
    dataset = get_symmetry_dataset(crystal_data.cell, symprec=symprec)
    spgnum = get_spgnum(crystal_data.name)
    assert dataset.number == spgnum

    for i in range(spg_to_hall[spgnum - 1], spg_to_hall[spgnum]):
        dataset_with_hall_number = get_symmetry_dataset(
            crystal_data.cell, hall_number=i, symprec=symprec
        )
        assert dataset_with_hall_number.hall_number == i
        spg_type = get_spacegroup_type(dataset_with_hall_number.hall_number)
        assert dataset_with_hall_number.international == spg_type.international_short
        assert dataset_with_hall_number.hall == spg_type.hall_symbol
        assert dataset_with_hall_number.choice == spg_type.choice
        assert dataset_with_hall_number.pointgroup == spg_type.pointgroup_international

    wyckoffs = dataset.wyckoffs
    assert wyckoffs == crystal_data.ref["wyckoffs"]


def test_nonfinite_cell_values_are_rejected():
    lattice = np.eye(3)
    positions = np.array([[0.0, 0.0, 0.0]])
    numbers = np.array([1])
    for value in (np.nan, np.inf, -np.inf):
        bad_lattice = lattice.copy()
        bad_lattice[0, 0] = value
        with pytest.raises(spglib.SpglibError, match="non-finite"):
            get_symmetry_dataset((bad_lattice, positions, numbers))

    bad_positions = positions.copy()
    bad_positions[0, 0] = np.nan
    with pytest.raises(spglib.SpglibError, match="non-finite"):
        get_symmetry_dataset((lattice, bad_positions, numbers))


def test_standardize_cell_and_pointgroup(crystal_data: CrystalData):
    spgnum = get_spgnum(crystal_data.name)
    symprec = 1e-5
    std_cell = standardize_cell(
        crystal_data.cell,
        to_primitive=False,
        no_idealize=True,
        symprec=symprec,
    )
    dataset = get_symmetry_dataset(std_cell, symprec=symprec)
    assert dataset.number == spgnum

    # The test for point group has to be done after standardization.
    ptg_symbol, _, _ = get_pointgroup(dataset.rotations)
    assert dataset.pointgroup == ptg_symbol


def test_standardize_cell_from_primitive(crystal_data: CrystalData):
    spgnum = get_spgnum(crystal_data.name)
    symprec = 1e-5
    prim_cell = standardize_cell(
        crystal_data.cell,
        to_primitive=True,
        no_idealize=True,
        symprec=symprec,
    )
    std_cell = standardize_cell(
        prim_cell,
        to_primitive=False,
        no_idealize=True,
        symprec=symprec,
    )
    dataset = get_symmetry_dataset(std_cell, symprec=symprec)
    assert dataset.number == spgnum


def test_standardize_cell_to_primitive(crystal_data: CrystalData):
    spgnum = get_spgnum(crystal_data.name)
    symprec = 1e-5
    prim_cell = standardize_cell(
        crystal_data.cell,
        to_primitive=True,
        no_idealize=True,
        symprec=symprec,
    )
    dataset = get_symmetry_dataset(prim_cell, symprec=symprec)
    assert dataset.number == spgnum


def test_refine_cell(crystal_data_dataset):
    crystal_data = crystal_data_dataset["crystal_data"]
    dataset_0 = crystal_data_dataset["dataset"]
    symprec = crystal_data_dataset["symprec"]
    spgnum = get_spgnum(crystal_data.name)
    ref_cell_0 = (
        dataset_0.std_lattice,
        dataset_0.std_positions,
        dataset_0.std_types,
    )
    dataset_1 = get_symmetry_dataset(ref_cell_0, symprec=symprec)
    # Check the same space group type is found.
    assert dataset_1.number == spgnum

    # Check if the same structure is obtained when applying
    # standardization again, i.e., examining non cycling behaviour.
    # Currently only for orthorhombic.

    ref_cell_1 = (
        dataset_1.std_lattice,
        dataset_1.std_positions,
        dataset_1.std_types,
    )
    dataset_2 = get_symmetry_dataset(ref_cell_1, symprec=symprec)
    np.testing.assert_equal(
        dataset_1.std_types,
        dataset_2.std_types,
    )
    np.testing.assert_allclose(
        dataset_1.std_lattice,
        dataset_2.std_lattice,
        atol=symprec,
    )
    diff = dataset_1.std_positions - dataset_2.std_positions
    diff -= np.rint(diff)
    np.testing.assert_allclose(diff, 0, atol=symprec)


def test_get_spacegroup():
    cell = (
        [
            [1.0, 0.0, 0.0],
            [0.0, 1.0, 0.0],
            [0.0, 0.0, 1.0],
        ],
        [
            [0.0, 0.0, 0.0],
            [0.0, 0.5, 0.5],
            [0.5, 0.0, 0.5],
            [0.5, 0.5, 0.0],
        ],
        [0, 0, 0, 0],
    )
    assert get_spacegroup(cell) == "Fm-3m (225)"
    assert get_spacegroup(cell, symbol_type=1) == "Oh^5 (225)"


def test_find_primitive(crystal_data_dataset):
    crystal_data = crystal_data_dataset["crystal_data"]
    dataset = crystal_data_dataset["dataset"]
    symprec = crystal_data_dataset["symprec"]
    primitive = find_primitive(crystal_data.cell, symprec=symprec)

    spg_type = get_spacegroup_type(dataset.hall_number)
    c = spg_type.international_short[0]
    if c in ["A", "B", "C", "I"]:
        multiplicity = 2
    elif c == "F":
        multiplicity = 4
    elif c == "R":
        assert spg_type.choice == "H"
        if spg_type.choice == "H":
            multiplicity = 3
        else:  # spg_type['choice'] == 'R'
            multiplicity = 1
    else:
        multiplicity = 1
    assert len(dataset.std_types) == len(primitive[2]) * multiplicity


def test_magnetic_spacegroup_type():
    # P 3 -2"
    actual1 = get_magnetic_spacegroup_type(1279)
    expect1 = MagneticSpaceGroupType(
        uni_number=1279,
        litvin_number=1279,
        bns_number="156.49",
        og_number="156.1.1279",
        number=156,
        type=1,
    )
    assert actual1 == expect1

    # -P 2 2ab 1'
    actual2 = get_magnetic_spacegroup_type(452)
    expect2 = MagneticSpaceGroupType(
        uni_number=452,
        litvin_number=442,
        bns_number="55.354",
        og_number="55.2.442",
        number=55,
        type=2,
    )
    assert actual2 == expect2

    # P 31 2 1c' (0 0 1)
    actual3 = get_magnetic_spacegroup_type(1262)
    expect3 = MagneticSpaceGroupType(
        uni_number=1262,
        litvin_number=1270,
        bns_number="151.32",
        og_number="153.4.1270",
        number=151,
        type=4,
    )
    assert actual3 == expect3


def test_magnetic_symmetry_database():
    # UNI: R31'_c[R3] (1242), BNS: R_I3 (146.12)

    # Hexagonal axes: hall_number: 433
    data_h_actual = get_magnetic_symmetry_from_database(1242)
    for key in ["rotations", "translations", "time_reversals"]:
        assert len(data_h_actual[key]) == 18

    # Rhombohedral axes: hall_number: 434
    data_r_actual = get_magnetic_symmetry_from_database(1242, hall_number=434)
    data_r_expect = {
        "rotations": np.array(
            [
                # x,y,z
                [
                    [1, 0, 0],
                    [0, 1, 0],
                    [0, 0, 1],
                ],
                # y,z,x
                [
                    [0, 0, 1],
                    [1, 0, 0],
                    [0, 1, 0],
                ],
                # y+1/2,z+1/2,x+1/2'
                [
                    [0, 0, 1],
                    [1, 0, 0],
                    [0, 1, 0],
                ],
                # z,x,y
                [
                    [0, 1, 0],
                    [0, 0, 1],
                    [1, 0, 0],
                ],
                # x+1/2,y+1/2,z+1/2'
                [
                    [1, 0, 0],
                    [0, 1, 0],
                    [0, 0, 1],
                ],
                # z+1/2,x+1/2,y+1/2'
                [
                    [0, 1, 0],
                    [0, 0, 1],
                    [1, 0, 0],
                ],
            ],
            dtype=np.int32,
        ),
        "translations": np.array(
            [
                [0, 0, 0],
                [0, 0, 0],
                [0.5, 0.5, 0.5],
                [0, 0, 0],
                [0.5, 0.5, 0.5],
                [0.5, 0.5, 0.5],
            ],
        ),
        "time_reversals": np.array(
            [
                [0, 0, 1, 0, 1, 1],
            ],
        ),
    }
    for key in ["rotations", "translations", "time_reversals"]:
        assert np.allclose(data_r_actual[key], data_r_expect[key])
