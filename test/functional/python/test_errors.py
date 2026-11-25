from __future__ import annotations

import warnings

import pytest
import spglib
from spglib import niggli_reduce


@pytest.mark.parametrize("value", ["0", "1", "true", "false", "True", "False"])
def test_deprecation_warning_env(monkeypatch, value: str):
    monkeypatch.setenv("SPGLIB_OLD_ERROR_HANDLING", value)

    if value in ("1", "true", "True"):
        with pytest.warns(DeprecationWarning):
            warnings.simplefilter("always", DeprecationWarning)
            niggli_reduce([[1, 0, 0], [0, 1, 0], [0, 0, 1]])
    else:
        with warnings.catch_warnings():
            warnings.simplefilter("error")
            niggli_reduce([[1, 0, 0], [0, 1, 0], [0, 0, 1]])


@pytest.mark.parametrize("value", [True, False])
def test_deprecation_warning_OLD_ERROR_HANDLING(monkeypatch, value: bool):
    monkeypatch.setattr(spglib.error, "OLD_ERROR_HANDLING", value)

    if value:
        with pytest.warns(DeprecationWarning):
            warnings.simplefilter("always", DeprecationWarning)
            niggli_reduce([[1, 0, 0], [0, 1, 0], [0, 0, 1]])
    else:
        with warnings.catch_warnings():
            warnings.simplefilter("error")
            niggli_reduce([[1, 0, 0], [0, 1, 0], [0, 0, 1]])


def test_legacy_error_message_is_updated_and_cleared(monkeypatch):
    """A failed search remains diagnosable until the next successful call."""
    monkeypatch.setenv("SPGLIB_OLD_ERROR_HANDLING", "1")
    lattice = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
    with warnings.catch_warnings():
        warnings.simplefilter("ignore", DeprecationWarning)
        assert spglib.get_symmetry_dataset((lattice, [[0, 0, 0]], [1])) is not None
        assert spglib.get_error_message() == "no error"

        assert (
            spglib.get_symmetry_dataset((lattice, [[0, 0, 0], [0, 0, 0]], [1, 1]))
            is None
        )
        assert spglib.get_error_message() == "too close distance between atoms"

        assert spglib.get_symmetry_dataset((lattice, [[0, 0, 0]], [1])) is not None
        assert spglib.get_error_message() == "no error"
