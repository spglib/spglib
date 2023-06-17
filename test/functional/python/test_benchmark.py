from __future__ import annotations

from pathlib import Path
from typing import Callable

import pytest

from spglib import get_symmetry_dataset


@pytest.mark.benchmark(group="space-group")
def test_get_symmetry_dataset(
    benchmark, all_filenames: list[Path], read_vasp: Callable
):
    """Benchmarking get_symmetry_dataset on all structures under test/data."""
    # Load all cells beforehands
    cells = [read_vasp(fname) for fname in all_filenames]
    print(f"Benchmark get_symmetry_dataset on {len(cells)} structures")

    def _get_symmetry_dataset_for_cells():
        for cell in cells:
            _ = get_symmetry_dataset(cell, symprec=1e-5)

    benchmark.pedantic(_get_symmetry_dataset_for_cells, rounds=4)
