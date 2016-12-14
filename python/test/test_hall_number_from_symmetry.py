import unittest
import numpy as np
from spglib import get_symmetry_dataset, get_hall_number_from_symmetry
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

class TestGetHallNumberFromSymmetry(unittest.TestCase):

    def setUp(self):
        self._filenames = []
        for d in dirnames:
            self._filenames += ["%s/%s" % (d, fname)
                                for fname in listdir("./data/%s" % d)]

    def tearDown(self):
        pass

    def test_get_hall_number_from_symmetry(self):
        for fname in self._filenames:
            spgnum = int(fname.split('-')[1])
            cell = read_vasp("./data/%s" % fname)
            if 'distorted' in fname:
                dataset = get_symmetry_dataset(cell, symprec=1e-1)
                hall_number = get_hall_number_from_symmetry(
                    dataset['rotations'],
                    dataset['translations'],
                    symprec=1e-1)
                if hall_number != dataset['hall_number']:
                    print("%d != %d in %s" %
                          (hall_number, dataset['hall_number'], fname))
                    ref_cell = (dataset['std_lattice'],
                                dataset['std_positions'],
                                dataset['std_types'])
                    dataset = get_symmetry_dataset(ref_cell, symprec=1e-5)
                    hall_number = get_hall_number_from_symmetry(
                        dataset['rotations'],
                        dataset['translations'],
                        symprec=1e-5)
                    print("Using refinced cell: %d, %d in %s" %
                          (hall_number, dataset['hall_number'], fname))
            else:
                dataset = get_symmetry_dataset(cell, symprec=1e-5)
                hall_number = get_hall_number_from_symmetry(
                    dataset['rotations'],
                    dataset['translations'],
                    symprec=1e-5)
            self.assertEqual(hall_number, dataset['hall_number'],
                             msg=("%d != %d in %s" %
                                  (hall_number, dataset['hall_number'], fname)))

if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(
        TestGetHallNumberFromSymmetry)
    unittest.TextTestRunner(verbosity=2).run(suite)
    # unittest.main()
