import unittest
import numpy as np
from spglib import (get_symmetry_dataset, get_symmetry,
                    get_symmetry_from_database)
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
        self._datasets = []
        self._cells = []
        self._symprecs = []
        for d in dirnames:
            dirname = os.path.join(data_dir, "data", d)
            filenames = os.listdir(dirname)
            self._filenames += [os.path.join(dirname, fname)
                                for fname in filenames]

        for fname in self._filenames:
            cell = read_vasp(fname)
            if 'distorted' in fname:
                symprec = 1e-1
            else:
                symprec = 1e-5
            self._datasets.append(
                get_symmetry_dataset(cell, symprec=symprec))
            self._cells.append(cell)
            self._symprecs.append(symprec)

    def tearDown(self):
        pass

    def test_change_of_basis(self):
        for fname, dataset, cell, symprec in zip(
                self._filenames, self._datasets, self._cells, self._symprecs):
            std_lat = dataset['std_lattice']
            std_pos = dataset['std_positions']
            tmat = dataset['transformation_matrix']
            orig_shift = dataset['origin_shift']
            lat = np.dot(cell[0].T, np.linalg.inv(tmat))
            pos = np.dot(cell[1], tmat.T) + orig_shift
            for p in pos:
                diff = std_pos - p
                diff -= np.rint(diff)
                diff = np.dot(diff, lat.T)
                delta = np.sqrt((diff ** 2).sum(axis=1))
                indices = np.where(delta < symprec)[0]
                self.assertEqual(len(indices), 1,
                                 msg=("multi: %s %s" % (p, indices)))

    def test_std_symmetry(self):
        for fname, dataset, cell, symprec in zip(
                self._filenames, self._datasets, self._cells, self._symprecs):
            symmetry = get_symmetry_from_database(dataset['hall_number'])
            std_pos = dataset['std_positions']

            # for r, t in zip(symmetry['rotations'], symmetry['translations']):
            #     for rp in (np.dot(std_pos, r.T) + t):
            #         diff = std_pos - rp
            #         diff -= np.rint(diff)
            #         num_match = len(np.where(abs(diff).sum(axis=1) < 1e-3)[0])
            #         self.assertEqual(num_match, 1, msg="%s" % fname)

            # Equivalent above by numpy hack
            # 15 sec on macOS 2.3 GHz Intel Core i5 (4times faster than above)
            rot = symmetry['rotations']
            trans = symmetry['translations']
            # (n_sym, 3, n_atom)
            rot_pos = np.dot(rot, std_pos.T) + trans[:, :, None]
            for p in std_pos:
                diff = rot_pos - p[None, :, None]
                diff -= np.rint(diff)
                num_match = (abs(diff).sum(axis=1) < 1e-3).sum(axis=1)
                self.assertTrue((num_match == 1).all(), msg="%s" % fname)

    def test_std_rotation(self):
        for fname, dataset, cell, symprec in zip(
                self._filenames, self._datasets, self._cells, self._symprecs):
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

        for fname, dataset, cell, symprec in zip(
                self._filenames, self._datasets, self._cells, self._symprecs):
            cell_spin = cell + ([1, ] * len(cell[2]),)
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


if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestChangeOfBasis)
    unittest.TextTestRunner(verbosity=2).run(suite)
    # unittest.main()
