import unittest

import numpy as np

from spglib import get_magnetic_symmetry_dataset


class TestMagneticDataset(unittest.TestCase):

    def test_1(self):
        # 0.107_Ho2Ge2O7.mcif
        # Type-I: 92.111 P 4abw 2nw
        lattice = np.diag([6.8083, 6.8083, 12.3795])
        positions = np.array([
            [0.87664, 0.35295, 0.13499],
            [0.14705, 0.37664, 0.38499],
            [0.85295, 0.62336, 0.88499],
            [0.37664, 0.14705, 0.61501],
            [0.62336, 0.85295, 0.11501],
            [0.12336, 0.64705, 0.63499],
            [0.35295, 0.87664, 0.86501],
            [0.64705, 0.12336, 0.36501],
        ])
        numbers = [0, 0, 0, 0, 0, 0, 0, 0]
        magmoms = np.array([
            [ 1.67, -8.9 ,  0.  ],
            [ 8.9 ,  1.67,  0.  ],
            [-8.9 , -1.67,  0.  ],
            [ 1.67,  8.9 ,  0.  ],
            [-1.67, -8.9 ,  0.  ],
            [-1.67,  8.9 ,  0.  ],
            [-8.9 ,  1.67,  0.  ],
            [ 8.9 , -1.67,  0.  ],
        ])

        dataset = get_magnetic_symmetry_dataset((lattice, positions, numbers, magmoms))
        assert dataset['hall_number'] == 369
        assert dataset['uni_number'] == 771  # BNS=92.111
        assert np.all(dataset['time_reversals'] == False)


if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestMagneticDataset)
    unittest.TextTestRunner(verbosity=2).run(suite)
