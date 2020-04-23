import unittest
import numpy as np
from spglib import get_symmetry_dataset, find_primitive
from vasp import read_vasp
import os

data_dir = os.path.dirname(os.path.abspath(__file__))
dirnames = ('trigonal', )
rhomb_numbers = (146, 148, 155, 160, 161, 166, 167)
tmat = [[0.6666666666666666, -0.3333333333333333, -0.3333333333333333],
        [0.3333333333333333, 0.3333333333333333, -0.6666666666666666],
        [0.3333333333333333, 0.3333333333333333, 0.3333333333333333]]


class TestRhombSettingHR(unittest.TestCase):

    def setUp(self):
        """Extract filename of rhombohedral cell"""
        self._filenames = []
        for d in dirnames:
            dirname = os.path.join(data_dir, "data", d)
            filenames = []
            trigo_filenames = os.listdir(dirname)
            for number in rhomb_numbers:
                filenames += [fname for fname in trigo_filenames
                              if str(number) in fname]
            self._filenames += [os.path.join(dirname, fname)
                                for fname in filenames]

    def tearDown(self):
        pass

    def test_rhomb_prim_agreement_over_settings(self):
        for fname in self._filenames:
            cell = read_vasp(fname)
            symprec = 1e-5
            dataset_H = get_symmetry_dataset(cell, symprec=symprec)
            hall_number_R = dataset_H['hall_number'] + 1
            dataset_R = get_symmetry_dataset(cell,
                                             hall_number=hall_number_R,
                                             symprec=symprec)
            plat, _, _ = find_primitive(cell)
            plat_H = np.dot(dataset_H['std_lattice'].T, tmat).T
            plat_R = dataset_R['std_lattice']
            np.testing.assert_allclose(plat, plat_H,
                                       atol=1e-5, err_msg="%s" % fname)
            np.testing.assert_allclose(plat_R, plat_H,
                                       atol=1e-5, err_msg="%s" % fname)
            np.testing.assert_allclose(plat_R, plat,
                                       atol=1e-5, err_msg="%s" % fname)



if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestRhombSettingHR)
    unittest.TextTestRunner(verbosity=2).run(suite)
    # unittest.main()
