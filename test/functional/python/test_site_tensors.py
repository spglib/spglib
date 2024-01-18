import unittest

import numpy as np
from spglib import get_magnetic_symmetry, get_symmetry


class TestGetOperationsWithSiteTensors(unittest.TestCase):
    def setUp(self):
        lattice = [[0, 2, 2], [2, 0, 2], [2, 2, 0]]
        positions = [[0, 0, 0]]
        numbers = [1]
        magmoms = [[0, 0, 1]]
        self._cell_Ni = (lattice, positions, numbers, magmoms)

        # CrCl2, adapted from Example 1 in Tutorial-MAXMAGN:
        # http://webbdcrista1.ehu.es/cryst/tutorials/Tutorial-MAXMAGN.pdf
        lattice = [
            [6.8257, 0, 0],
            [0, 6.2139, 0],
            [0, 0, 3.4947],
        ]
        positions = [
            # Cr (2a), site symmetry: < -x,-y,-z; -x,-y,z >
            [0, 0, 0],
            [0.5, 0.5, 0.5],
            # Cl (4g)
            [0.3586, 0.2893, 0],
            [-0.3586, -0.2893, 0],
            [-0.3586 + 0.5, 0.2893 + 0.5, 0.5],
            [0.3586 + 0.5, -0.2893 + 0.5, 0.5],
        ]
        numbers = [1, 1, 2, 2, 2, 2]
        self._cell_CrCl2 = (lattice, positions, numbers)

    def tearDown(self):
        pass

    def test_get_symmetry_non_collinear(self):
        sym = get_magnetic_symmetry(self._cell_Ni, with_time_reversal=False)
        self.assertEqual(8, len(sym["rotations"]))
        np.testing.assert_equal(sym["equivalent_atoms"], [0])

        # type-III magnetic space group
        sym2 = get_magnetic_symmetry(self._cell_Ni)
        self.assertEqual(16, len(sym2["rotations"]))
        self.assertEqual(16, len(sym2["translations"]))
        self.assertEqual(16, len(sym2["time_reversals"]))

    def test_get_symmetry_vectors(self):
        # Space group without magnetic moments
        # Pnnm (58), "-P 2 2n" (hall_number=275)
        # Generators: -x,-y,-z; -x,-y,z; -x+1/2,y+1/2,-z+1/2
        sym = get_symmetry(self._cell_CrCl2)
        self.assertEqual(8, len(sym["rotations"]))

        # Type-I magnetic space group (MSG)
        magmoms1 = [
            [0, 0, 1],
            [0, 0, -1],
            [0, 0, 0],
            [0, 0, 0],
            [0, 0, 0],
            [0, 0, 0],
        ]
        sym1 = get_magnetic_symmetry(self._cell_CrCl2 + (magmoms1,))
        self.assertTrue(np.allclose(sym1["time_reversals"], False))
        self.assertTrue(np.allclose(sym1["rotations"], sym["rotations"]))
        self.assertTrue(np.allclose(sym1["translations"], sym["translations"]))

        # Type-II MSG
        # 58.394, -P 2 2n 1'
        magmoms2 = [
            [0, 0, 0],
            [0, 0, 0],
            [0, 0, 0],
            [0, 0, 0],
            [0, 0, 0],
            [0, 0, 0],
        ]
        sym2 = get_magnetic_symmetry(self._cell_CrCl2 + (magmoms2,))
        self.assertEqual(len(sym2["rotations"]), 16)
        self.assertEqual(np.sum(sym2["time_reversals"]), 8)

        sym2_gray = get_magnetic_symmetry(
            self._cell_CrCl2 + (magmoms2,),
            with_time_reversal=False,
        )
        self.assertTrue(np.allclose(sym2_gray["rotations"], sym["rotations"]))
        self.assertTrue(np.allclose(sym2_gray["translations"], sym["translations"]))

        # Type-III MSG
        # 58.397: -P 2 2n'
        # Generators: -x,-y,-z; -x,-y,z; -x+1/2,y+1/2,-z+1/2'
        magmoms3 = [
            [0, 0, 1],
            [0, 0, 1],
            [0, 0, 0],
            [0, 0, 0],
            [0, 0, 0],
            [0, 0, 0],
        ]
        sym3 = get_magnetic_symmetry(self._cell_CrCl2 + (magmoms3,))
        self.assertTrue(np.allclose(sym3["rotations"], sym["rotations"]))
        self.assertTrue(np.allclose(sym3["translations"], sym["translations"]))
        self.assertTrue(np.sum(sym3["time_reversals"]), 4)

        # Type-IV MSG
        # https://github.com/spglib/spglib/issues/150
        # -P 2 2 -> -P 2 2 1c' (47.254)
        lat = [[1, 0, 0], [0, 2, 0], [0, 0, 3]]
        pos = [[0.0, 0.0, 0.0], [0.0, 0.0, 0.5]]
        num = [0, 0]
        magmom = [[0.0, 0.0, 1.0], [0.0, 0.0, -1.0]]
        sym_gray = get_symmetry((lat, pos, num))
        sym4 = get_magnetic_symmetry((lat, pos, num, magmom))
        self.assertTrue(np.allclose(sym_gray["rotations"], sym4["rotations"]))
        self.assertTrue(np.allclose(sym_gray["translations"], sym4["translations"]))
        self.assertTrue(sym4["time_reversals"].shape[0], 16)
        self.assertTrue(np.sum(sym4["time_reversals"]), 8)
        self.assertTrue(np.allclose(sym4["primitive_lattice"], lat))

    def test_with_distorted_magmom(self):
        magmoms = [
            [0, 0, 1.001],
            [0, 0, 0.999],
            [0, 0, 0],
            [0, 0, 0],
            [0, 0, 0],
            [0, 0, 0],
        ]
        sym = get_magnetic_symmetry(self._cell_CrCl2 + (magmoms,), mag_symprec=1e-2)
        expect = [0, 0, 2, 2, 2, 2]
        assert np.allclose(sym["equivalent_atoms"], expect)


if __name__ == "__main__":
    suite = unittest.TestLoader().loadTestsFromTestCase(
        TestGetOperationsWithSiteTensors,
    )
    unittest.TextTestRunner(verbosity=2).run(suite)
    # unittest.main()
