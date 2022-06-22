import unittest

import numpy as np

from spglib import get_magnetic_symmetry_dataset


class TestMagneticDataset(unittest.TestCase):
    def test_type1(self):
        # 0.107_Ho2Ge2O7.mcif
        # Type-I: 92.111 P 4abw 2nw
        lattice = np.diag([6.8083, 6.8083, 12.3795])
        positions = np.array(
            [
                [0.87664, 0.35295, 0.13499],
                [0.14705, 0.37664, 0.38499],
                [0.85295, 0.62336, 0.88499],
                [0.37664, 0.14705, 0.61501],
                [0.62336, 0.85295, 0.11501],
                [0.12336, 0.64705, 0.63499],
                [0.35295, 0.87664, 0.86501],
                [0.64705, 0.12336, 0.36501],
            ]
        )
        numbers = [0, 0, 0, 0, 0, 0, 0, 0]
        magmoms = np.array(
            [
                [1.67, -8.9, 0.0],
                [8.9, 1.67, 0.0],
                [-8.9, -1.67, 0.0],
                [1.67, 8.9, 0.0],
                [-1.67, -8.9, 0.0],
                [-1.67, 8.9, 0.0],
                [-8.9, 1.67, 0.0],
                [8.9, -1.67, 0.0],
            ]
        )

        dataset = get_magnetic_symmetry_dataset((lattice, positions, numbers, magmoms))
        assert dataset["hall_number"] == 369
        assert dataset['msg_type'] == 1
        assert dataset["uni_number"] == 771  # BNS=92.111
        assert np.all(dataset["time_reversals"] == False)

    def test_type3(self):
        # MAGNDATA 0.101_Mn2GeO4.mcif
        # Type-III
        # BNS: Pn'm'a" (62.446)
        # MHall: -P 2ac 2n'
        lattice = np.array(
            [
                [1.06949000e01, 0.00000000e00, 0.00000000e00],
                [3.84998337e-16, 6.28750000e00, 0.00000000e00],
                [3.09590711e-16, 3.09590711e-16, 5.05600000e00],
            ]
        )
        positions = np.array(
            [
                [0.0, 0.0, 0.0],
                [0.5, 0.0, 0.5],
                [0.5, 0.5, 0.5],
                [0.0, 0.5, 0.0],
                [0.2794, 0.25, 0.9906],
                [0.2206, 0.75, 0.4906],
                [0.7206, 0.75, 0.0094],
                [0.7794, 0.25, 0.5094],
            ]
        )
        numbers = [0, 0, 0, 0, 0, 0, 0, 0]
        magmoms = np.array(
            [
                [3.0, 0.4, 0.0],
                [-3.0, -0.4, 0.0],
                [-3.0, 0.4, 0.0],
                [3.0, -0.4, 0.0],
                [4.5, 0.0, 0.0],
                [-4.5, 0.0, 0.0],
                [4.5, 0.0, 0.0],
                [-4.5, 0.0, 0.0],
            ]
        )

        dataset = get_magnetic_symmetry_dataset((lattice, positions, numbers, magmoms))
        assert dataset['hall_number'] == 292
        assert dataset['msg_type'] == 3
        assert dataset['uni_number'] == 544

    def test_monoclinic(self):
        # MAGNDATA 0.103_Mn2GeO4.mcif
        # Type-I
        # unique axis-c
        a = 10.69260000
        b = 6.28510000
        c = 5.05570000
        lattice = np.diag([a, b, c])
        positions = np.array([
            [0.00000000, 0.00000000, 0.00000000],
            [0.50000000, 0.00000000, 0.50000000],
            [0.00000000, 0.50000000, 0.00000000],
            [0.50000000, 0.50000000, 0.50000000],
            [0.27940000, 0.25000000, 0.99030000],
            [0.22060000, 0.75000000, 0.49030000],
            [0.72060000, 0.75000000, 0.00970000],
            [0.77940000, 0.25000000, 0.50970000],
        ])
        numbers = np.array([0, 0, 0, 0, 0, 0, 0, 0])
        magmoms = np.array([
            [1.90000000, 2.80000000, 0.30000000],
            [-1.90000000, -2.80000000, 0.30000000],
            [1.70000000, 2.40000000, -0.30000000],
            [-1.70000000, -2.40000000, -0.30000000],
            [2.90000000, 3.40000000, 0.00000000],
            [-2.90000000, -3.40000000, 0.00000000],
            [2.90000000, 3.40000000, 0.00000000],
            [-2.90000000, -3.40000000, 0.00000000],
        ])

        dataset = get_magnetic_symmetry_dataset((lattice, positions, numbers, magmoms))
        assert dataset['msg_type'] == 1
        assert dataset['uni_number'] == 82  # BNS: 14.75
        assert np.allclose(dataset['transformation_matrix'], np.array([[0, 1, 0], [0, 0, 1], [1, 0, 0]]))
        assert np.allclose(dataset['std_rotation_matrix'], np.eye(3))
        std_lattice_expect = np.array([
            [0, b, 0],
            [0, 0, c],  # unique-axis
            [a, 0, 0]
        ])
        assert np.allclose(dataset['std_lattice'], std_lattice_expect)
        std_positions_expect = np.array([
            [0.00000000, 0.00000000, 0.00000000],
            [0.00000000, 0.50000000, 0.50000000],
            [0.50000000, 0.00000000, 0.00000000],
            [0.50000000, 0.50000000, 0.50000000],
            [0.25000000, 0.99030000, 0.27940000],
            [0.75000000, 0.49030000, 0.22060000],
            [0.75000000, 0.00970000, 0.72060000],
            [0.25000000, 0.50970000, 0.77940000],
        ])
        assert np.allclose(dataset['std_positions'], std_positions_expect)
        assert np.allclose(dataset['std_tensors'], magmoms)

    def test_trigonal(self):
        # MAGNDATA: 0.108_Mn3Ir.mcif
        # hall_number=459 (166:R)
        # Type-III, BNS: 166.101
        lattice = np.array([
            [3.77000000, 0.00000000, 0.00000000],
            [0.00000000, 3.77000000, 0.00000000],
            [0.00000000, 0.00000000, 3.77000000],
        ])
        positions = np.array([
            [0.00000000, 0.00000000, 0.00000000],
            [0.00000000, 0.50000000, 0.50000000],
            [0.50000000, 0.00000000, 0.50000000],
            [0.50000000, 0.50000000, 0.00000000],
        ])
        numbers = np.array([0, 1, 1, 1])
        magmoms = np.array([
            [0.00000000, 0.00000000, 0.00000000],
            [2.00000000, -1.00000000, -1.00000000],
            [-1.00000000, 2.00000000, -1.00000000],
            [-1.00000000, -1.00000000, 2.00000000],
        ])

        dataset = get_magnetic_symmetry_dataset((lattice, positions, numbers, magmoms))
        assert dataset['uni_number'] == 1331
        assert dataset['hall_number'] == 458  # choose hexagonal axes in default
        assert dataset['msg_type'] == 3
        assert len(dataset['time_reversals']) == 12
        # Use hexagonal axes for standardized cell
        assert np.around(1 / np.linalg.det(dataset['transformation_matrix'])) == 3
        assert dataset['std_positions'].shape[0] == positions.shape[0] * 3
        assert dataset['std_types'].shape[0] == numbers.shape[0] * 3
        assert dataset['std_tensors'].shape[0] == magmoms.shape[0] * 3

        # Test hR with the standardized cell
        std_cell = (dataset['std_lattice'], dataset['std_positions'], dataset['std_types'], dataset['std_tensors'])
        dataset2 = get_magnetic_symmetry_dataset(std_cell)
        assert dataset2['uni_number'] == 1331
        assert dataset2['hall_number'] == 458
        assert dataset2['n_operations'] == 12 * 3
        assert np.isclose(np.linalg.det(dataset2['primitive_lattice']), np.linalg.det(lattice))

    def test_conventional(self):
        # MAGNDATA: 0.110_Cr2O3.mcif
        # BNS: 15.87, MHall: C 2yc' -1'
        lattice = np.array([
            [4.95700000, 0.00000000, 0.00000000],
            [-2.47850000, 4.29288793, 0.00000000],
            [0.00000000, 0.00000000, 13.59230000],
        ])
        positions = np.array([
            [0.00000000, 0.00000000, 0.34751000],
            [0.33333333, 0.66666667, 0.01417667],
            [0.66666667, 0.33333333, 0.68084333],
            [0.33333333, 0.66666667, 0.51417667],
            [0.66666667, 0.33333333, 0.18084333],
            [0.00000000, 0.00000000, 0.84751000],
            [0.33333333, 0.66666667, 0.81915667],
            [0.00000000, 0.00000000, 0.15249000],
            [0.66666667, 0.33333333, 0.48582333],
            [0.00000000, 0.00000000, 0.65249000],
            [0.66666667, 0.33333333, 0.98582333],
            [0.33333333, 0.66666667, 0.31915667],
            [0.30570000, 0.00000000, 0.25000000],
            [0.63903333, 0.66666667, 0.91666667],
            [0.97236667, 0.33333333, 0.58333333],
            [0.02763333, 0.66666667, 0.41666667],
            [0.36096667, 0.33333333, 0.08333333],
            [0.69430000, 0.00000000, 0.75000000],
        ])
        numbers = np.array([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1])
        magmoms = np.array([
            [-1.00000000, 0.00000000, 0.00000000],
            [-1.00000000, 0.00000000, 0.00000000],
            [-1.00000000, 0.00000000, 0.00000000],
            [-1.00000000, 0.00000000, 0.00000000],
            [-1.00000000, 0.00000000, 0.00000000],
            [-1.00000000, 0.00000000, 0.00000000],
            [1.00000000, 0.00000000, 0.00000000],
            [1.00000000, 0.00000000, 0.00000000],
            [1.00000000, 0.00000000, 0.00000000],
            [1.00000000, 0.00000000, 0.00000000],
            [1.00000000, 0.00000000, 0.00000000],
            [1.00000000, 0.00000000, 0.00000000],
            [0.00000000, 0.00000000, 0.00000000],
            [0.00000000, 0.00000000, 0.00000000],
            [0.00000000, 0.00000000, 0.00000000],
            [0.00000000, 0.00000000, 0.00000000],
            [0.00000000, 0.00000000, 0.00000000],
            [0.00000000, 0.00000000, 0.00000000],
        ])

        dataset = get_magnetic_symmetry_dataset((lattice, positions, numbers, magmoms))
        assert dataset['uni_number'] == 94
        assert dataset['hall_number'] == 90
        assert dataset['rotations'].shape[0] == 12
        # Original hP setting with 18 atoms are converted to mS with 12 (= 18 / 3 * 2) atoms
        assert dataset['n_std_atoms'] == 12


if __name__ == "__main__":
    suite = unittest.TestLoader().loadTestsFromTestCase(TestMagneticDataset)
    unittest.TextTestRunner(verbosity=2).run(suite)
