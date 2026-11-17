import os
import subprocess
import sys
from functools import partial

import numpy as np
import pytest
import spglib
from spglib import _internal

pytestmark = pytest.mark.filterwarnings("ignore::DeprecationWarning")


@pytest.fixture
def cell():
    return (np.eye(3), np.array([[0.0, 0.0, 0.0]]), [1])


@pytest.fixture(
    params=[0.0, -1.0, np.nan, np.inf, -np.inf],
    ids=["zero", "negative", "nan", "positive-infinity", "negative-infinity"],
)
def invalid_symprec(request):
    return request.param


@pytest.fixture(
    params=[
        "get_symmetry",
        "get_symmetry_dataset",
        "get_spacegroup",
        "get_spacegroup_type_from_symmetry",
        "get_hall_number_from_symmetry",
        "get_magnetic_symmetry",
        "get_magnetic_symmetry_dataset",
        "get_magnetic_spacegroup_type_from_symmetry",
        "standardize_cell",
        "refine_cell",
        "find_primitive",
        "get_ir_reciprocal_mesh",
        "get_symmetry_layerdataset",
        "get_layergroup",
    ],
    ids=str,
)
def symmetry_api(request, cell):
    name = request.param
    if name in ("get_symmetry_layerdataset", "get_layergroup"):
        return partial(getattr(_internal, name), cell)
    function = getattr(spglib, name)
    if name == "get_ir_reciprocal_mesh":
        return partial(function, [2, 2, 2], cell)
    if name in ("get_magnetic_symmetry", "get_magnetic_symmetry_dataset"):
        return partial(function, (*cell, [1.0]))
    if name in (
        "get_spacegroup_type_from_symmetry",
        "get_hall_number_from_symmetry",
        "get_magnetic_spacegroup_type_from_symmetry",
    ):
        arguments = {"rotations": [np.eye(3, dtype=int)], "translations": [[0, 0, 0]]}
        if name == "get_magnetic_spacegroup_type_from_symmetry":
            arguments["time_reversals"] = [False]
        return partial(function, **arguments)
    return partial(function, cell)


def test_rejects_invalid_symprec(symmetry_api, invalid_symprec):
    with pytest.raises(ValueError, match="symprec must be a finite positive number"):
        symmetry_api(symprec=invalid_symprec)


def test_accepts_valid_symprec(symmetry_api):
    assert symmetry_api(symprec=1e-5) is not None


def test_simple_cubic_spacegroup(cell):
    assert spglib.get_symmetry_dataset(cell, symprec=1e-5).number == 221


def test_invalid_symprec_raises_by_default():
    """Test the installed default without conftest's error-mode override."""
    environment = os.environ.copy()
    environment.pop("SPGLIB_OLD_ERROR_HANDLING", None)
    result = subprocess.run(
        [
            sys.executable,
            "-Werror",
            "-c",
            "import numpy as np; import spglib; "
            "spglib.get_symmetry_dataset((np.eye(3), [[0, 0, 0]], [1]), symprec=0)",
        ],
        env=environment,
        capture_output=True,
        text=True,
        timeout=30,
    )
    assert result.returncode == 1
    assert result.stderr.endswith(
        "ValueError: symprec must be a finite positive number\n"
    )
