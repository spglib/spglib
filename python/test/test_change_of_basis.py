import unittest
import numpy as np
from spglib import get_symmetry_dataset, get_symmetry
from vasp import read_vasp
import os

data_dir = os.path.dirname(os.path.abspath(__file__))

dirnames = ('cubic',
            'hexagonal',
            'monoclinic',
            'orthorhombic',
            'tetragonal',
            'triclinic',
            'trigonal',
            'distorted',
            'virtual_structure')


class TestChangeOfBasis(unittest.TestCase):

    def setUp(self):
        self._filenames = []
        self._spgnum_ref = []
        for d in dirnames:
            dirname = os.path.join(data_dir, "data", d)
            filenames = os.listdir(dirname)
            self._spgnum_ref += [int(fname.split('-')[1])
                                 for fname in filenames]
            self._filenames += [os.path.join(dirname, fname)
                                for fname in filenames]

    def tearDown(self):
        pass

    def test_change_of_basis(self):
        for fname in self._filenames:
            cell = read_vasp(fname)
            if 'distorted' in fname:
                symprec = 1e-1
            else:
                symprec = 1e-5

            dataset = get_symmetry_dataset(cell, symprec=symprec)
            std_lat = dataset['std_lattice']
            std_pos = dataset['std_positions']
            tmat = dataset['transformation_matrix']
            orig_shift = dataset['origin_shift']
            lat = np.dot(cell[0].T, np.linalg.inv(tmat))
            pos = np.dot(cell[1], tmat.T) + orig_shift
            self._compare(lat, pos, std_lat, std_pos, 2 * symprec)

    def test_std_rotation(self):
        for fname in self._filenames:
            cell = read_vasp(fname)
            if 'distorted' in fname:
                symprec = 1e-1
            else:
                symprec = 1e-5

            dataset = get_symmetry_dataset(cell, symprec=symprec)
            std_lat = dataset['std_lattice']
            tmat = dataset['transformation_matrix']
            lat = np.dot(cell[0].T, np.linalg.inv(tmat))
            lat_rot = np.dot(dataset['std_rotation_matrix'], lat)

            np.testing.assert_allclose(std_lat, lat_rot.T, atol=symprec,
                                       err_msg="%s" % fname)

    def _test_get_symmetry(self):
        """
        ***************************************************************
        This test must be executed with spglib compiled with -DSPGTEST.
        ***************************************************************
        """

        for fname in self._filenames:
            cell = read_vasp(fname)
            cell_spin = cell + ([1, ] * len(cell[2]),)
            if 'distorted' in fname:
                symprec = 1e-1
            else:
                symprec = 1e-5

            dataset = get_symmetry_dataset(cell, symprec=symprec)
            symmetry = get_symmetry(cell_spin, symprec=symprec)

            self.assertEqual(len(dataset['rotations']),
                             len(symmetry['rotations']),
                             msg=("%s" % fname))

            for r, t in zip(dataset['rotations'], dataset['translations']):
                found = False
                for r_, t_ in zip(symmetry['rotations'],
                                  symmetry['translations']):
                    if (r == r_).all():
                        diff = t - t_
                        diff -= np.rint(diff)
                        if (abs(diff) < symprec).all():
                            found = True
                            break
                self.assertTrue(found, msg="%s" % fname)

    def _compare(self, lat, pos, std_lat, std_pos, symprec):
        for p in pos:
            diff = std_pos - p
            diff -= np.rint(diff)
            diff = np.dot(diff, lat.T)
            delta = np.sqrt((diff ** 2).sum(axis=1))
            indices = np.where(delta < symprec)[0]
            self.assertEqual(len(indices), 1,
                             msg=("multi: %s %s" % (p, indices)))


if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestChangeOfBasis)
    unittest.TextTestRunner(verbosity=2).run(suite)
    # unittest.main()
