"""Tests of delaunay_reduce and niggli_reduce."""

from pathlib import Path

import numpy as np
from spglib import delaunay_reduce, niggli_reduce

cwd = Path(__file__).parent


def _get_lattice_parameters(lattice):
    return np.sqrt(np.dot(lattice, lattice.T).diagonal())


def _get_angles(lattice):
    a, b, c = _get_lattice_parameters(lattice)
    alpha = np.arccos(np.vdot(lattice[1, :], lattice[2, :]) / b / c)
    beta = np.arccos(np.vdot(lattice[2, :], lattice[0, :]) / c / a)
    gamma = np.arccos(np.vdot(lattice[0, :], lattice[1, :]) / a / b)
    return np.array([alpha, beta, gamma]) / np.pi * 180


def _reduce(input_lattices, func, ref_data):
    for i, (input_lattice, reference_lattice) in enumerate(
        zip(input_lattices, ref_data),
    ):
        reduced_lattice = func(input_lattice)
        # self._show_lattice(i, reduced_lattice)
        assert np.allclose(
            [np.linalg.det(input_lattice)],
            [np.linalg.det(reduced_lattice)],
        )
        T = np.dot(np.linalg.inv(reference_lattice.T), input_lattice.T)
        assert np.allclose(T, np.rint(T))
        assert np.allclose(
            _metric_tensor(reduced_lattice),
            _metric_tensor(reference_lattice),
        ), "\n".join(
            [
                "# %d" % (i + 1),
                "Input lattice",
                "%s" % input_lattice,
                " angles: %s" % np.array(_get_angles(input_lattice)),
                "Reduced lattice in reference data",
                "%s" % reference_lattice,
                " angles: %s" % np.array(_get_angles(reference_lattice)),
                "Reduced lattice",
                "%s" % reduced_lattice,
                " angles: %s" % np.array(_get_angles(reduced_lattice)),
                _str_lattice(reduced_lattice),
            ],
        )


def _str_lattice(lattice):
    lines = []
    for v in lattice:
        lines.append(" ".join(["%20.16f" % x for x in v]))
    return "\n".join(lines)


def _metric_tensor(lattice):
    """Return metric tensor.

    Parameters
    ----------
    lattice: Lattice parameters in the form of
        [[a_x, a_y, a_z],
         [b_x, b_y, b_z],
         [c_x, c_y, c_z]]

    Returns
    -------
    Metric tensor:
        [[a.a, a.b, a.c],
         [b.a, b.b, b.c],
         [c.a, c.b, c.c]]

    """
    return np.dot(lattice, np.transpose(lattice))


def _read_file(filename):
    all_lattices = []
    with open(filename) as f:
        lattice = []
        for line in f:
            if line[0] == "#":
                continue
            lattice.append([float(x) for x in line.split()])
            if len(lattice) == 3:
                all_lattices.append(lattice)
                lattice = []
    return np.array(all_lattices)


def _test_niggli_angles(niggli_lattices):
    for i, reference_lattice in enumerate(niggli_lattices):
        angles = np.array(_get_angles(reference_lattice))
        assert (angles > 90 - 1e-3).all() or (angles < 90 + 1e-3).all(), "%d %s" % (
            i + 1,
            angles,
        )


def _show_lattice(self, i, lattice):
    print("# %d" % (i + 1))
    print(self._str_lattice(lattice))
    # for v in lattice:
    #     print(" ".join(["%20.16f" % x for x in v]))


def test_niggli_reduce():
    """Test niggli_reduce."""
    input_lattices = _read_file(cwd / "lattices.dat")
    niggli_lattices = _read_file(cwd / "niggli_lattices.dat")
    _test_niggli_angles(niggli_lattices)
    _reduce(input_lattices, niggli_reduce, niggli_lattices)


def test_delaunay_reduce():
    """Test niggli_reduce."""
    input_lattices = _read_file(cwd / "lattices.dat")
    delaunay_lattices = _read_file(cwd / "delaunay_lattices.dat")
    _reduce(input_lattices, delaunay_reduce, delaunay_lattices)
