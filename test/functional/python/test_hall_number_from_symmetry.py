"""Test of get_hall_number_from_symmetry."""
from __future__ import annotations

import os
from pathlib import Path
from typing import Callable

import pytest

from spglib import get_hall_number_from_symmetry, get_symmetry_dataset

data_dir = os.path.dirname(os.path.abspath(__file__))


@pytest.mark.filterwarnings("ignore::DeprecationWarning")
def test_get_hall_number_from_symmetry(all_filenames: list[Path], read_vasp: Callable):
    """Test get_hall_number_from_symmetry."""
    for fname in all_filenames:
        cell = read_vasp(fname)
        if "distorted" in str(fname):
            dataset = get_symmetry_dataset(cell, symprec=1e-1)
            hall_number = get_hall_number_from_symmetry(
                dataset["rotations"], dataset["translations"], symprec=1e-1
            )
            if hall_number != dataset["hall_number"]:
                print("%d != %d in %s" % (hall_number, dataset["hall_number"], fname))
                ref_cell = (
                    dataset["std_lattice"],
                    dataset["std_positions"],
                    dataset["std_types"],
                )
                dataset = get_symmetry_dataset(ref_cell, symprec=1e-5)
                hall_number = get_hall_number_from_symmetry(
                    dataset["rotations"], dataset["translations"], symprec=1e-5
                )
                print(
                    "Using refined cell: %d, %d in %s"
                    % (hall_number, dataset["hall_number"], fname)
                )
        else:
            dataset = get_symmetry_dataset(cell, symprec=1e-5)
            hall_number = get_hall_number_from_symmetry(
                dataset["rotations"], dataset["translations"], symprec=1e-5
            )
        assert hall_number == dataset["hall_number"], "%d != %d in %s" % (
            hall_number,
            dataset["hall_number"],
            fname,
        )
