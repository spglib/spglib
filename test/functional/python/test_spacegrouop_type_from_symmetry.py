"""Test of spacegroup_type_from_symmetry."""
from __future__ import annotations

import os
from pathlib import Path
from typing import Callable

import pytest

from spglib import (
    get_spacegroup_type,
    get_spacegroup_type_from_symmetry,
    get_symmetry_dataset,
)

data_dir = os.path.dirname(os.path.abspath(__file__))


@pytest.mark.parametrize("lattice_None", [True, False])
def test_spacegroup_type_from_symmetry(
    all_filenames: list[Path], read_vasp: Callable, lattice_None: bool
):
    """Test spacegroup_type_from_symmetry."""
    for fname in all_filenames:
        cell = read_vasp(fname)
        if lattice_None:
            lattice = None
        else:
            lattice = cell[0]
        if "distorted" in str(fname):
            dataset = get_symmetry_dataset(cell, symprec=1e-1)
            spgtype = get_spacegroup_type_from_symmetry(
                dataset["rotations"],
                dataset["translations"],
                lattice=lattice,
                symprec=1e-1,
            )
            if spgtype["number"] != dataset["number"]:
                print("%d != %d in %s" % (spgtype["number"], dataset["number"], fname))
                ref_cell = (
                    dataset["std_lattice"],
                    dataset["std_positions"],
                    dataset["std_types"],
                )
                dataset = get_symmetry_dataset(ref_cell, symprec=1e-5)
                spgtype = get_spacegroup_type_from_symmetry(
                    dataset["rotations"],
                    dataset["translations"],
                    lattice=lattice,
                    symprec=1e-5,
                )
                print(
                    "Using refined cell: %d, %d in %s"
                    % (spgtype["number"], dataset["number"], fname)
                )
        else:
            dataset = get_symmetry_dataset(cell, symprec=1e-5)
            spgtype = get_spacegroup_type_from_symmetry(
                dataset["rotations"],
                dataset["translations"],
                lattice=lattice,
                symprec=1e-5,
            )

        assert spgtype["number"] == dataset["number"], "%d != %d in %s" % (
            spgtype["number"],
            dataset["number"],
            fname,
        )
        spgtype_ref = get_spacegroup_type(dataset["hall_number"])
        for key in spgtype:
            assert spgtype[key] == spgtype_ref[key]
