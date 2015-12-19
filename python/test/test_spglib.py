import unittest
import numpy as np
from spglib import get_symmetry_dataset
from vasp import read_vasp
from os import listdir

dirnames = ('cubic',
            'hexagonal',
            'monoclinic',
            'orthorhombic',
            'tetragonal',
            'triclinic',
            'trigonal',
            'distorted',
            'virtual_structure')

class TestSpglib(unittest.TestCase):

    def setUp(self):
        self._filenames = []
        for d in dirnames:
            self._filenames += ["%s/%s" % (d, fname)
                                for fname in listdir("./data/%s" % d)]
    
    def tearDown(self):
        pass
    
    def test_get_symmetry_dataset(self):
        for fname in self._filenames:
            spgnum = int(fname.split('-')[1])
            cell = read_vasp("./data/%s" % fname)
            if 'distorted' in fname:
                dataset = get_symmetry_dataset(cell, symprec=1e-1)
            else:
                dataset = get_symmetry_dataset(cell, symprec=1e-5)
            self.assertEqual(dataset['number'], spgnum)

if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestSpglib)
    unittest.TextTestRunner(verbosity=2).run(suite)
    # unittest.main()
