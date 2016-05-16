import unittest
import numpy as np
from spglib import get_symmetry
from vasp import read_vasp
from os import listdir

class TestGetSymmetry(unittest.TestCase):

    def setUp(self):
        lattice = [[4, 0, 0], [0, 4, 0], [0, 0, 4]]
        positions = [[0, 0, 0], [0.5, 0.5, 0.5]]
        numbers = [1, 1]
        magmoms = [0, 0]
        self._cell = (lattice, positions, numbers, magmoms)

    def tearDown(self):
        pass

    def test_get_symmetry_ferro(self):
        self._cell[3][0] = 1
        self._cell[3][1] = 1
        sym = get_symmetry(self._cell)
        self.assertEqual(96, len(sym['rotations']))
        self.assertTrue((sym['equivalent_atoms'] == [0, 0]).all())

    def test_get_symmetry_anti_ferro(self):
        self._cell[3][0] = 1
        self._cell[3][1] = -1
        sym = get_symmetry(self._cell)
        self.assertEqual(96, len(sym['rotations']))
        self.assertTrue((sym['equivalent_atoms'] == [0, 0]).all())

    def test_get_symmetry_broken_magmoms(self):
        self._cell[3][0] = 1
        self._cell[3][1] = 2
        sym = get_symmetry(self._cell)
        self.assertEqual(48, len(sym['rotations']))
        self.assertTrue((sym['equivalent_atoms'] == [0, 1]).all())

if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestGetSymmetry)
    unittest.TextTestRunner(verbosity=2).run(suite)
    # unittest.main()
