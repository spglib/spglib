import unittest
try:
    from StringIO import StringIO
except ImportError:
    from io import StringIO
import numpy as np
from spglib import (get_ir_reciprocal_mesh,
                    get_stabilized_reciprocal_mesh,
                    get_symmetry_from_database)
from vasp import read_vasp

result_ir_rec_mesh = """   0    0   0   0
   1    1   0   0
   2    2   0   0
   1   -1   0   0
   1    0   1   0
   5    1   1   0
   6    2   1   0
   5   -1   1   0
   2    0   2   0
   6    1   2   0
  10    2   2   0
   6   -1   2   0
   1    0  -1   0
   5    1  -1   0
   6    2  -1   0
   5   -1  -1   0
   1    0   0   1
   5    1   0   1
   6    2   0   1
   5   -1   0   1
   5    0   1   1
  21    1   1   1
  22    2   1   1
  21   -1   1   1
   6    0   2   1
  22    1   2   1
  26    2   2   1
  22   -1   2   1
   5    0  -1   1
  21    1  -1   1
  22    2  -1   1
  21   -1  -1   1
   2    0   0   2
   6    1   0   2
  10    2   0   2
   6   -1   0   2
   6    0   1   2
  22    1   1   2
  26    2   1   2
  22   -1   1   2
  10    0   2   2
  26    1   2   2
  42    2   2   2
  26   -1   2   2
   6    0  -1   2
  22    1  -1   2
  26    2  -1   2
  22   -1  -1   2
   1    0   0  -1
   5    1   0  -1
   6    2   0  -1
   5   -1   0  -1
   5    0   1  -1
  21    1   1  -1
  22    2   1  -1
  21   -1   1  -1
   6    0   2  -1
  22    1   2  -1
  26    2   2  -1
  22   -1   2  -1
   5    0  -1  -1
  21    1  -1  -1
  22    2  -1  -1
  21   -1  -1  -1"""

class TestReciprocalMesh(unittest.TestCase):

    def setUp(self):
        self._filenames = ["cubic/POSCAR-217",]
        self._mesh = [4, 4, 4]

    def tearDown(self):
        pass

    def test_get_ir_reciprocal_mesh(self):
        for fname in self._filenames:
            cell = read_vasp("./data/%s" % fname)
            ir_rec_mesh = get_ir_reciprocal_mesh(self._mesh, cell)
            (mapping_table, grid_address) = ir_rec_mesh
            # for gp, ga in zip(mapping_table, grid_address):
            #     print("%4d  %3d %3d %3d" % (gp, ga[0], ga[1], ga[2]))
            data = np.loadtxt(StringIO(result_ir_rec_mesh), dtype='intc')
            self.assertTrue((data[:, 0] == mapping_table).all())
            self.assertTrue((data[:, 1:4] == grid_address).all())

    def test_get_stabilized_reciprocal_mesh(self):
        for fname in self._filenames:
            cell = read_vasp("./data/%s" % fname)
            rotations = get_symmetry_from_database(513)['rotations']
            ir_rec_mesh = get_stabilized_reciprocal_mesh(self._mesh, rotations)
            (mapping_table, grid_address) = ir_rec_mesh
            data = np.loadtxt(StringIO(result_ir_rec_mesh), dtype='intc')
            self.assertTrue((data[:, 0] == mapping_table).all())
            self.assertTrue((data[:, 1:4] == grid_address).all())

if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestReciprocalMesh)
    unittest.TextTestRunner(verbosity=2).run(suite)
    # unittest.main()
