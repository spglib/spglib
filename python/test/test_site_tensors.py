import unittest
import numpy as np
from spglib import get_symmetry


class TestGetOperationsWithSiteTensors(unittest.TestCase):

    def setUp(self):
        lattice = [[0, 2, 2], [2, 0, 2], [2, 2, 0]]
        positions = [[0, 0, 0]]
        numbers = [1]
        magmoms = [[0, 0, 1]]
        self._cell_Ni = (lattice, positions, numbers, magmoms)

    def tearDown(self):
        pass

    def test_get_symmetry_non_collinear(self):
        sym = get_symmetry(self._cell_Ni)
        self.assertEqual(8, len(sym['rotations']))
        np.testing.assert_equal(sym['equivalent_atoms'], [0])

    def test_get_symmetry_vectors(self):
        pass


if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(
        TestGetOperationsWithSiteTensors)
    unittest.TextTestRunner(verbosity=2).run(suite)
    # unittest.main()
