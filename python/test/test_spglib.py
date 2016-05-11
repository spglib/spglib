import unittest
import numpy as np
from spglib import get_symmetry_dataset, refine_cell
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

    def test_refine_cell(self):
        for fname in self._filenames:
            spgnum = int(fname.split('-')[1])
            cell = read_vasp("./data/%s" % fname)
            if 'distorted' in fname:
                dataset_orig = get_symmetry_dataset(cell, symprec=1e-1)
            else:
                dataset_orig = get_symmetry_dataset(cell, symprec=1e-5)
            ref_cell = (dataset_orig['std_lattice'],
                        dataset_orig['std_positions'],
                        dataset_orig['std_types'])
            dataset = get_symmetry_dataset(ref_cell, symprec=1e-5)
            if dataset['number'] != spgnum:
                print("%s" % fname)
            self.assertEqual(dataset['number'], spgnum)

if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestSpglib)
    unittest.TextTestRunner(verbosity=2).run(suite)
    # unittest.main()
