from itertools import permutations, product

import numpy as np
import pytest
from spglib import get_symmetry_dataset


@pytest.mark.parametrize("lengths", [(2, 3, 4), (2, 2, 4), (2, 2, 2), None])
@pytest.mark.parametrize("rotated", [False, True])
@pytest.mark.parametrize("angle_tolerance", [-1.0, 0.001])
def test_lattice_symmetry_operation_sets(lengths, rotated, angle_tolerance):
    """Check complete Bravais operation sets, including non-coordinate axes."""
    expected = set()
    if lengths is None:
        lattice = np.array([[2, 0, 0], [-1, np.sqrt(3), 0], [0, 0, 4]])
        sixfold = np.array([[1, -1, 0], [1, 0, 0], [0, 0, 1]])
        mirror = np.array([[0, 1, 0], [1, 0, 0], [0, 0, 1]])
        for power, reflected, z_sign in product(range(6), range(2), [-1, 1]):
            rotation = np.linalg.matrix_power(sixfold, power)
            rotation = rotation @ np.linalg.matrix_power(mirror, reflected)
            rotation[2, 2] = z_sign
            expected.add(tuple(rotation.ravel()))
        assert len(expected) == 24
    else:
        lattice = np.diag(lengths)
        for axes, signs in product(permutations(range(3)), product([-1, 1], repeat=3)):
            if any(lengths[i] != lengths[axes[i]] for i in range(3)):
                continue
            rotation = np.diag(signs) @ np.eye(3, dtype=int)[list(axes)]
            expected.add(tuple(rotation.ravel()))
    if rotated:
        rigid_rotation = np.array([[1, 0, 0], [0, 0.6, -0.8], [0, 0.8, 0.6]])
        lattice = lattice @ rigid_rotation.T
    dataset = get_symmetry_dataset(
        (lattice, [[0, 0, 0]], [1]), angle_tolerance=angle_tolerance
    )
    assert {tuple(rotation.ravel()) for rotation in dataset.rotations} == expected
    np.testing.assert_allclose(dataset.translations, 0, atol=1e-12)
