import os
import unittest
try:
    from StringIO import StringIO
except ImportError:
    from io import StringIO
import numpy as np
from spglib import (get_ir_reciprocal_mesh,
                    get_stabilized_reciprocal_mesh,
                    get_symmetry_dataset)
from vasp import read_vasp

data_dir = os.path.dirname(os.path.abspath(__file__))

result_ir_rec_mesh = ("""   0    0   0   0
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
  21   -1  -1  -1""", """   0    0   0   0
   1    1   0   0
   2    2   0   0
   1   -1   0   0
   1    0   1   0
   5    1   1   0
   5    2   1   0
   1   -1   1   0
   2    0   2   0
   5    1   2   0
   2    2   2   0
   5   -1   2   0
   1    0  -1   0
   1    1  -1   0
   5    2  -1   0
   5   -1  -1   0
  16    0   0   1
  17    1   0   1
  18    2   0   1
  17   -1   0   1
  17    0   1   1
  21    1   1   1
  21    2   1   1
  17   -1   1   1
  18    0   2   1
  21    1   2   1
  18    2   2   1
  21   -1   2   1
  17    0  -1   1
  17    1  -1   1
  21    2  -1   1
  21   -1  -1   1""")

result_ir_rec_mesh_distortion = ("""  0    0   0   0
   1    1   0   0
   1   -1   0   0
   3    0   1   0
   4    1   1   0
   4   -1   1   0
   6    0   2   0
   7    1   2   0
   7   -1   2   0
   3    0  -1   0
   4    1  -1   0
   4   -1  -1   0
   3    0   0   1
   4    1   0   1
   4   -1   0   1
  15    0   1   1
  16    1   1   1
  16   -1   1   1
  18    0   2   1
  19    1   2   1
  19   -1   2   1
  15    0  -1   1
  16    1  -1   1
  16   -1  -1   1
   6    0   0   2
   7    1   0   2
   7   -1   0   2
  18    0   1   2
  19    1   1   2
  19   -1   1   2
  30    0   2   2
  31    1   2   2
  31   -1   2   2
  18    0  -1   2
  19    1  -1   2
  19   -1  -1   2
   3    0   0  -1
   4    1   0  -1
   4   -1   0  -1
  15    0   1  -1
  16    1   1  -1
  16   -1   1  -1
  18    0   2  -1
  19    1   2  -1
  19   -1   2  -1
  15    0  -1  -1
  16    1  -1  -1
  16   -1  -1  -1""", """   0    0   0   0
   1    1   0   0
   1   -1   0   0
   3    0   1   0
   4    1   1   0
   5   -1   1   0
   6    0   2   0
   7    1   2   0
   8   -1   2   0
   6    0  -2   0
   8    1  -2   0
   7   -1  -2   0
   3    0  -1   0
   5    1  -1   0
   4   -1  -1   0""", """   0    0   0   0
   1    1   0   0
   1   -1   0   0
   3    0   1   0
   4    1   1   0
   4   -1   1   0
   3    0   2   0
   4    1   2   0
   4   -1   2   0
   0    0  -1   0
   1    1  -1   0
   1   -1  -1   0
  12    0   0   1
  13    1   0   1
  13   -1   0   1
  15    0   1   1
  16    1   1   1
  16   -1   1   1
  15    0   2   1
  16    1   2   1
  16   -1   2   1
  12    0  -1   1
  13    1  -1   1
  13   -1  -1   1
  24    0   0   2
  25    1   0   2
  25   -1   0   2
  27    0   1   2
  28    1   1   2
  28   -1   1   2
  27    0   2   2
  28    1   2   2
  28   -1   2   2
  24    0  -1   2
  25    1  -1   2
  25   -1  -1   2
  12    0   0  -1
  13    1   0  -1
  13   -1   0  -1
  15    0   1  -1
  16    1   1  -1
  16   -1   1  -1
  15    0   2  -1
  16    1   2  -1
  16   -1   2  -1
  12    0  -1  -1
  13    1  -1  -1
  13   -1  -1  -1""", """   0    0   0   0
   1    1   0   0
   2   -1   0   0
   3    0   1   0
   4    1   1   0
   5   -1   1   0
   6    0   2   0
   7    1   2   0
   7   -1   2   0
   3    0  -2   0
   5    1  -2   0
   4   -1  -2   0
   0    0  -1   0
   2    1  -1   0
   1   -1  -1   0""")


class TestReciprocalMesh(unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_get_ir_reciprocal_mesh(self):
        file_and_mesh = (
            [os.path.join(data_dir, "data", "cubic", "POSCAR-217"), [4, 4, 4]],
            [os.path.join(data_dir, "data", "hexagonal", "POSCAR-182"),
             [4, 4, 2]])
        i = 0
        for fname, mesh in file_and_mesh:
            cell = read_vasp(fname)
            ir_rec_mesh = get_ir_reciprocal_mesh(mesh, cell)
            (mapping_table, grid_address) = ir_rec_mesh
            # for gp, ga in zip(mapping_table, grid_address):
            #     print("%4d  %3d %3d %3d" % (gp, ga[0], ga[1], ga[2]))
            # print("")
            data = np.loadtxt(StringIO(result_ir_rec_mesh[i]), dtype='intc')
            np.testing.assert_equal(data[:, 0], mapping_table)
            np.testing.assert_equal(data[:, 1:4], grid_address)
            i += 1

    def test_get_stabilized_reciprocal_mesh(self):
        file_and_mesh = (
            [os.path.join(data_dir, "data", "cubic", "POSCAR-217"), [4, 4, 4]],
            [os.path.join(data_dir, "data", "hexagonal", "POSCAR-182"),
             [4, 4, 2]])
        i = 0
        for fname, mesh in file_and_mesh:
            cell = read_vasp(fname)
            rotations = get_symmetry_dataset(cell)['rotations']
            ir_rec_mesh = get_stabilized_reciprocal_mesh(mesh, rotations)
            (mapping_table, grid_address) = ir_rec_mesh
            data = np.loadtxt(StringIO(result_ir_rec_mesh[i]), dtype='intc')
            np.testing.assert_equal(data[:, 0], mapping_table)
            np.testing.assert_equal(data[:, 1:4], grid_address)
            i += 1

    def test_get_ir_reciprocal_mesh_distortion(self):
        file_and_mesh = (
            [os.path.join(data_dir, "data", "cubic", "POSCAR-217"), [3, 4, 4]],
            [os.path.join(data_dir, "data", "hexagonal", "POSCAR-182"),
             [3, 5, 1]])
        i = 0
        for is_shift in ([0, 0, 0], [0, 1, 0]):
            for fname, mesh in file_and_mesh:
                cell = read_vasp(fname)
                ir_rec_mesh = get_ir_reciprocal_mesh(mesh, cell,
                                                     is_shift=is_shift)
                (mapping_table, grid_address) = ir_rec_mesh
                # for gp, ga in zip(mapping_table, grid_address):
                #     print("%4d  %3d %3d %3d" % (gp, ga[0], ga[1], ga[2]))
                # print("")
                data = np.loadtxt(StringIO(result_ir_rec_mesh_distortion[i]),
                                  dtype='intc')
                np.testing.assert_equal(data[:, 0], mapping_table)
                np.testing.assert_equal(data[:, 1:4], grid_address)
                i += 1

if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestReciprocalMesh)
    unittest.TextTestRunner(verbosity=2).run(suite)
    # unittest.main()
