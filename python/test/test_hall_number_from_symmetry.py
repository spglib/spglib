import os
import unittest
from spglib import get_symmetry_dataset, get_hall_number_from_symmetry
from vasp import read_vasp
from test_spglib import dirnames

data_dir = os.path.dirname(os.path.abspath(__file__))


class TestGetHallNumberFromSymmetry(unittest.TestCase):

    def setUp(self):
        self._filenames = []
        for d in dirnames:
            dirname = os.path.join(data_dir, "data", d)
            filenames = os.listdir(dirname)
            self._filenames += [os.path.join(dirname, fname)
                                for fname in filenames]

    def tearDown(self):
        pass

    def test_get_hall_number_from_symmetry(self):
        for fname in self._filenames:
            cell = read_vasp(fname)
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
            self.assertEqual(
                hall_number, dataset['hall_number'],
                msg=("%d != %d in %s" %
                     (hall_number, dataset['hall_number'], fname)))


if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(
        TestGetHallNumberFromSymmetry)
    unittest.TextTestRunner(verbosity=2).run(suite)
    # unittest.main()
