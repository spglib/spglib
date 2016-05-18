import unittest
import numpy as np
from spglib import (get_symmetry_dataset, refine_cell, find_primitive,
                    get_spacegroup_type, get_symmetry)
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
            self.assertEqual(dataset['number'], spgnum,
                             msg=("%s" % fname))
            spg_type = get_spacegroup_type(dataset['hall_number'])
            self.assertEqual(dataset['international'],
                             spg_type['international_short'],
                             msg=("%s" % fname))
            self.assertEqual(dataset['hall'], spg_type['hall_symbol'],
                             msg=("%s" % fname))
            self.assertEqual(dataset['choice'], spg_type['choice'],
                             msg=("%s" % fname))
            self.assertEqual(dataset['pointgroup'],
                             spg_type['pointgroup_schoenflies'],
                             msg=("%s" % fname))

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
            self.assertEqual(dataset['number'], spgnum,
                             msg=("%s" % fname))

    def test_find_primitive(self):
        for fname in self._filenames:
            spgnum = int(fname.split('-')[1])
            cell = read_vasp("./data/%s" % fname)
            if 'distorted' in fname:
                dataset = get_symmetry_dataset(cell, symprec=1e-1)
                primitive = find_primitive(cell, symprec=1e-1)
            else:
                dataset = get_symmetry_dataset(cell, symprec=1e-5)
                primitive = find_primitive(cell, symprec=1e-5)
            spg_type = get_spacegroup_type(dataset['hall_number'])
            c = spg_type['international_short'][0]
            if c in ['A', 'B', 'C', 'I']:
                multiplicity = 2
            elif c == 'F':
                multiplicity = 4
            elif c == 'R':
                self.assertEqual(spg_type['choice'], 'H')
                if spg_type['choice'] == 'H':
                    multiplicity = 3
                else: # spg_type['choice'] == 'R'
                    multiplicity = 1
            else:
                multiplicity = 1
            self.assertEqual(len(dataset['std_types']),
                             len(primitive[2]) * multiplicity,
                             msg=("multi: %d, %s" % (multiplicity, fname)))

    def test_get_symmetry(self):
        for fname in self._filenames:
            spgnum = int(fname.split('-')[1])
            cell = read_vasp("./data/%s" % fname)
            if 'distorted' in fname:
                num_sym_dataset = len(
                    get_symmetry_dataset(cell, symprec=1e-1)['rotations'])
                num_sym = len(get_symmetry(cell, symprec=1e-1)['rotations'])
            else:
                num_sym_dataset = len(
                    get_symmetry_dataset(cell, symprec=1e-5)['rotations'])
                num_sym = len(get_symmetry(cell, symprec=1e-5)['rotations'])
            self.assertEqual(num_sym_dataset, num_sym)

if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestSpglib)
    unittest.TextTestRunner(verbosity=2).run(suite)
    # unittest.main()
