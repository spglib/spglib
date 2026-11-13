import numpy as np
import pytest
import spglib


@pytest.mark.parametrize(
    "numbers, message",
    [
        pytest.param(
            [1.0], "numbers has to be an array-like of integers.", id="integral-float"
        ),
        pytest.param(
            [1.5], "numbers has to be an array-like of integers.", id="fractional-float"
        ),
        pytest.param(
            np.array([1.0]),
            "numbers has to be an array-like of integers.",
            id="float-array",
        ),
        pytest.param(
            [np.nan], "numbers has to be an array-like of integers.", id="nan"
        ),
        pytest.param(
            [np.inf], "numbers has to be an array-like of integers.", id="infinity"
        ),
        pytest.param(
            [True], "numbers has to be an array-like of integers.", id="boolean"
        ),
        pytest.param(
            ["1"], "numbers has to be an array-like of integers.", id="string"
        ),
        pytest.param(
            [1 + 0j], "numbers has to be an array-like of integers.", id="complex"
        ),
        pytest.param(
            np.array([1], dtype=object),
            "numbers has to be an array-like of integers.",
            id="object-array",
        ),
        pytest.param(1, "numbers has to be a (num_atoms,) array.", id="scalar"),
        pytest.param(
            [[1]], "numbers has to be a (num_atoms,) array.", id="two-dimensional"
        ),
        pytest.param(
            [[1, 2], [3]],
            "setting an array element with a sequence. The requested array has an "
            "inhomogeneous shape after 1 dimensions. The detected shape was (2,) + "
            "inhomogeneous part.",
            id="ragged",
        ),
    ],
)
def test_get_symmetry_dataset_rejects_invalid_atom_types(numbers, message):
    """Reject unsupported species dtypes and shapes without coercion."""
    cell = (np.eye(3), np.array([[0.0, 0.0, 0.0]]), numbers)
    with pytest.raises(spglib.SpglibError) as error:
        spglib.get_symmetry_dataset(cell)
    assert str(error.value) == f"Generic Spglib error:\n{message}"


@pytest.mark.parametrize(
    "numbers",
    [
        [1],
        (1,),
        np.array([1], dtype=np.int16),
        np.array([1], dtype=np.int32),
        np.array([1], dtype=np.int64),
        np.array([1], dtype=np.uint8),
        [-1],
    ],
)
def test_get_symmetry_dataset_accepts_integer_atom_types(numbers):
    """Integer array-likes retain the symmetry of a primitive cubic cell."""
    cell = (np.eye(3), np.array([[0.0, 0.0, 0.0]]), numbers)
    dataset = spglib.get_symmetry_dataset(cell)
    assert dataset.number == 221
    np.testing.assert_array_equal(dataset.equivalent_atoms, [0])
