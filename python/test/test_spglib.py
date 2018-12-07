import unittest
from spglib import (get_symmetry_dataset, find_primitive,
                    get_spacegroup_type, standardize_cell)
from vasp import read_vasp
import yaml
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

spg_to_hall = [
      1,   2,   3,   6,   9,  18,  21,  30,  39,  57,
     60,  63,  72,  81,  90, 108, 109, 112, 115, 116,
    119, 122, 123, 124, 125, 128, 134, 137, 143, 149,
    155, 161, 164, 170, 173, 176, 182, 185, 191, 197,
    203, 209, 212, 215, 218, 221, 227, 228, 230, 233,
    239, 245, 251, 257, 263, 266, 269, 275, 278, 284,
    290, 292, 298, 304, 310, 313, 316, 322, 334, 335,
    337, 338, 341, 343, 349, 350, 351, 352, 353, 354,
    355, 356, 357, 358, 359, 361, 363, 364, 366, 367,
    368, 369, 370, 371, 372, 373, 374, 375, 376, 377,
    378, 379, 380, 381, 382, 383, 384, 385, 386, 387,
    388, 389, 390, 391, 392, 393, 394, 395, 396, 397,
    398, 399, 400, 401, 402, 404, 406, 407, 408, 410,
    412, 413, 414, 416, 418, 419, 420, 422, 424, 425,
    426, 428, 430, 431, 432, 433, 435, 436, 438, 439,
    440, 441, 442, 443, 444, 446, 447, 448, 449, 450,
    452, 454, 455, 456, 457, 458, 460, 462, 463, 464,
    465, 466, 467, 468, 469, 470, 471, 472, 473, 474,
    475, 476, 477, 478, 479, 480, 481, 482, 483, 484,
    485, 486, 487, 488, 489, 490, 491, 492, 493, 494,
    495, 497, 498, 500, 501, 502, 503, 504, 505, 506,
    507, 508, 509, 510, 511, 512, 513, 514, 515, 516,
    517, 518, 520, 521, 523, 524, 525, 527, 529, 530, 531]


class TestSpglib(unittest.TestCase):

    def setUp(self):
        self._filenames = []
        self._ref_filenames = []
        self._spgnum_ref = []
        for d in dirnames:
            dirname = os.path.join(data_dir, "data", d)
            refdirname = os.path.join(data_dir, "ref", d)
            filenames = os.listdir(dirname)
            self._spgnum_ref += [int(fname.split('-')[1])
                                 for fname in filenames]
            self._filenames += [os.path.join(dirname, fname)
                                for fname in filenames]
            self._ref_filenames += [os.path.join(refdirname, fname + "-ref")
                                    for fname in filenames]

    def _create_symref(self):
        pass

    def tearDown(self):
        pass

    def test_get_symmetry_dataset(self):
        for fname, spgnum, reffname in zip(self._filenames,
                                           self._spgnum_ref,
                                           self._ref_filenames):
            cell = read_vasp(fname)

            if 'distorted' in fname:
                symprec = 1e-1
            else:
                symprec = 1e-5
            dataset = get_symmetry_dataset(cell, symprec=symprec)
            self.assertEqual(dataset['number'], spgnum, msg=("%s" % fname))

            for i in range(spg_to_hall[spgnum - 1], spg_to_hall[spgnum]):
                dataset = get_symmetry_dataset(cell, hall_number=i,
                                               symprec=symprec)
                self.assertEqual(dataset['hall_number'], i, msg=("%s" % fname))
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

            wyckoffs = dataset['wyckoffs']
            wyckoffs_ref = yaml.load(open(reffname))['wyckoffs']
            for w, w_ref in zip(wyckoffs, wyckoffs_ref):
                self.assertEqual(w, w_ref, msg=("%s" % fname))

            # This is for writing out detailed symmetry info into files.
            # Now it is only for Wyckoff positions.
            # with open(reffname, 'w') as f:
            #     f.write("wyckoffs:\n")
            #     for w in dataset['wyckoffs']:
            #         f.write("- \"%s\"\n" % w)

    def test_standardize_cell(self):
        for fname, spgnum in zip(self._filenames, self._spgnum_ref):
            cell = read_vasp(fname)
            if 'distorted' in fname:
                symprec = 1e-1
            else:
                symprec = 1e-5

            std_cell = standardize_cell(cell,
                                        to_primitive=False,
                                        no_idealize=True,
                                        symprec=symprec)
            dataset = get_symmetry_dataset(std_cell, symprec=symprec)
            self.assertEqual(dataset['number'], spgnum,
                             msg=("%s" % fname))

    def test_standardize_cell_from_primitive(self):
        for fname, spgnum in zip(self._filenames, self._spgnum_ref):
            cell = read_vasp(fname)
            if 'distorted' in fname:
                symprec = 1e-1
            else:
                symprec = 1e-5

            prim_cell = standardize_cell(cell,
                                         to_primitive=True,
                                         no_idealize=True,
                                         symprec=symprec)
            std_cell = standardize_cell(prim_cell,
                                        to_primitive=False,
                                        no_idealize=True,
                                        symprec=symprec)
            dataset = get_symmetry_dataset(std_cell, symprec=symprec)
            self.assertEqual(dataset['number'], spgnum,
                             msg=("%s" % fname))

    def test_standardize_cell_to_primitive(self):
        for fname, spgnum in zip(self._filenames, self._spgnum_ref):
            cell = read_vasp(fname)
            if 'distorted' in fname:
                symprec = 1e-1
            else:
                symprec = 1e-5

            prim_cell = standardize_cell(cell,
                                         to_primitive=True,
                                         no_idealize=True,
                                         symprec=symprec)
            dataset = get_symmetry_dataset(prim_cell, symprec=symprec)
            self.assertEqual(dataset['number'], spgnum,
                             msg=("%s" % fname))

    def test_refine_cell(self):
        for fname, spgnum in zip(self._filenames, self._spgnum_ref):
            cell = read_vasp(fname)
            if 'distorted' in fname:
                dataset_orig = get_symmetry_dataset(cell, symprec=1e-1)
            else:
                dataset_orig = get_symmetry_dataset(cell, symprec=1e-5)
            ref_cell = (dataset_orig['std_lattice'],
                        dataset_orig['std_positions'],
                        dataset_orig['std_types'])
            dataset = get_symmetry_dataset(ref_cell, symprec=1e-5)
            self.assertEqual(dataset['number'], spgnum, msg=("%s" % fname))

    def test_find_primitive(self):
        for fname in self._filenames:
            cell = read_vasp(fname)
            if 'distorted' in fname:
                symprec = 1e-1
            else:
                symprec = 1e-5

            dataset = get_symmetry_dataset(cell, symprec=symprec)
            primitive = find_primitive(cell, symprec=symprec)

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
                else:  # spg_type['choice'] == 'R'
                    multiplicity = 1
            else:
                multiplicity = 1
            self.assertEqual(len(dataset['std_types']),
                             len(primitive[2]) * multiplicity,
                             msg=("multi: %d, %s" % (multiplicity, fname)))


if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestSpglib)
    unittest.TextTestRunner(verbosity=2).run(suite)
    # unittest.main()
