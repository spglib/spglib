import os
import unittest
try:
    from StringIO import StringIO
except ImportError:
    from io import StringIO
import numpy as np
from spglib import (get_ir_reciprocal_mesh,
                    get_stabilized_reciprocal_mesh,
                    get_symmetry_dataset,
                    relocate_BZ_grid_address)
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

result_bz_grid_address = ("""  0  0  0
 1  0  0
 2  0  0
-1  0  0
 0  1  0
 1  1  0
 2  1  0
-1  1  0
 0  2  0
 1  2  0
 2  2  0
-1  2  0
 0 -1  0
 1 -1  0
 2 -1  0
-1 -1  0
 0  0  1
 1  0  1
 2  0  1
-1  0  1
 0  1  1
 1  1  1
 2  1  1
-1  1  1
 0  2  1
 1  2  1
 2  2  1
-1  2  1
 0 -1  1
 1 -1  1
 2 -1  1
-1 -1  1
 0  0  2
 1  0  2
 2  0  2
-1  0  2
 0  1  2
 1  1  2
 2  1  2
-1  1  2
 0  2  2
 1  2  2
 2  2  2
-1  2  2
 0 -1  2
 1 -1  2
 2 -1  2
-1 -1  2
 0  0 -1
 1  0 -1
 2  0 -1
-1  0 -1
 0  1 -1
 1  1 -1
 2  1 -1
-1  1 -1
 0  2 -1
 1  2 -1
 2  2 -1
-1  2 -1
 0 -1 -1
 1 -1 -1
 2 -1 -1
-1 -1 -1
-2  0  0
-2  1  0
 0 -2  0
 1 -2  0
 2 -2  0
-2  2  0
-2 -2  0
-1 -2  0
-2 -1  0
-2  0  1
-2  1  1
 0 -2  1
 1 -2  1
 2 -2  1
-2  2  1
-2 -2  1
-1 -2  1
-2 -1  1
 0  0 -2
 1  0 -2
 2  0 -2
-2  0  2
-2  0 -2
-1  0 -2
 0  1 -2
 1  1 -2
 2  1 -2
-2  1  2
-2  1 -2
-1  1 -2
 0  2 -2
 0 -2  2
 0 -2 -2
 1  2 -2
 1 -2  2
 1 -2 -2
 2  2 -2
 2 -2  2
 2 -2 -2
-2  2  2
-2  2 -2
-2 -2  2
-2 -2 -2
-1  2 -2
-1 -2  2
-1 -2 -2
 0 -1 -2
 1 -1 -2
 2 -1 -2
-2 -1  2
-2 -1 -2
-1 -1 -2
-2  0 -1
-2  1 -1
 0 -2 -1
 1 -2 -1
 2 -2 -1
-2  2 -1
-2 -2 -1
-1 -2 -1
-2 -1 -1""", """ 0  0  0
 1  0  0
 2  0  0
-1  0  0
 0  1  0
 1  1  0
-2  1  0
-1  1  0
 0  2  0
 1 -2  0
 2 -2  0
-1  2  0
 0 -1  0
 1 -1  0
 2 -1  0
-1 -1  0
 0  0  1
 1  0  1
 2  0  1
-1  0  1
 0  1  1
 1  1  1
-2  1  1
-1  1  1
 0  2  1
 1 -2  1
 2 -2  1
-1  2  1
 0 -1  1
 1 -1  1
 2 -1  1
-1 -1  1
-2  0  0
 0 -2  0
-2  2  0
 0  0 -1
 1  0 -1
 2  0 -1
-2  0  1
-2  0 -1
-1  0 -1
 0  1 -1
 1  1 -1
-2  1 -1
-1  1 -1
 0  2 -1
 0 -2  1
 0 -2 -1
 1 -2 -1
 2 -2 -1
-2  2  1
-2  2 -1
-1  2 -1
 0 -1 -1
 1 -1 -1
 2 -1 -1
-1 -1 -1""")

result_bz_map = ("""  0   1   2  -1  -1  -1  64   3   4   5
  6  -1  -1  -1  65   7   8   9  10  -1
 -1  -1  69  11  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  66  67
 68  -1  -1  -1  70  71  12  13  14  -1
 -1  -1  72  15  16  17  18  -1  -1  -1
 73  19  20  21  22  -1  -1  -1  74  23
 24  25  26  -1  -1  -1  78  27  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  75  76  77  -1  -1  -1  79  80
 28  29  30  -1  -1  -1  81  31  32  33
 34  -1  -1  -1  85  35  36  37  38  -1
 -1  -1  91  39  40  41  42  -1  -1  -1
103  43  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  95  98 101  -1
 -1  -1 105 108  44  45  46  -1  -1  -1
113  47  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  82  83  84  -1  -1  -1
 86  87  88  89  90  -1  -1  -1  92  93
 94  97 100  -1  -1  -1 104 107  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  96  99 102  -1  -1  -1 106 109
110 111 112  -1  -1  -1 114 115  48  49
 50  -1  -1  -1 116  51  52  53  54  -1
 -1  -1 117  55  56  57  58  -1  -1  -1
121  59  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1 118 119 120  -1
 -1  -1 122 123  60  61  62  -1  -1  -1
124  63""", """  0   1   2  -1  -1  -1  32   3   4   5
 -1  -1  -1  -1   6   7   8  -1  -1  -1
 -1  -1  34  11  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  33   9
 10  -1  -1  -1  -1  -1  12  13  14  -1
 -1  -1  -1  15  16  17  18  -1  -1  -1
 38  19  20  21  -1  -1  -1  -1  22  23
 24  -1  -1  -1  -1  -1  50  27  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  46  25  26  -1  -1  -1  -1  -1
 28  29  30  -1  -1  -1  -1  31  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  35  36  37  -1  -1  -1  39  40
 41  42  -1  -1  -1  -1  43  44  45  -1
 -1  -1  -1  -1  51  52  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 47  48  49  -1  -1  -1  -1  -1  53  54
 55  -1  -1  -1  -1  56""")


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

    def test_relocate_BZ_grid_address(self):
        file_and_mesh = (
            [os.path.join(data_dir, "data", "cubic", "POSCAR-217"), [4, 4, 4]],
            [os.path.join(data_dir, "data", "hexagonal", "POSCAR-182"),
             [4, 4, 2]])

        for i, (fname, mesh) in enumerate(file_and_mesh):
            cell = read_vasp(fname)
            _, grid_address = get_ir_reciprocal_mesh(mesh, cell)
            reclat = np.linalg.inv(cell[0])
            bz_grid_address, bz_map = relocate_BZ_grid_address(
                grid_address,
                mesh,
                reclat,
                is_shift=np.zeros(3, dtype='intc'))
            data = np.loadtxt(StringIO(result_bz_grid_address[i]),
                              dtype='intc')
            np.testing.assert_equal(data, bz_grid_address)

            data = [int(i) for i in result_bz_map[i].split()]
            np.testing.assert_equal(data, bz_map)

            # for i in range(len(bz_map) // 10):
            #     print(("%3d " * 10) % tuple(bz_map[i * 10: (i + 1) * 10]))
            # n = len(bz_map) % 10
            # print(("%3d " * n) % tuple(bz_map[-n:]))


if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestReciprocalMesh)
    unittest.TextTestRunner(verbosity=2).run(suite)
    # unittest.main()
