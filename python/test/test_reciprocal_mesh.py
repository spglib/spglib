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
                    relocate_BZ_grid_address,
                    get_grid_points_by_rotations,
                    get_BZ_grid_points_by_rotations,
                    get_grid_point_from_address)
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
  21   -1  -1   1""", """   0    0   0   0
   1    1   0   0
   1   -1   0   0
   1    0   1   0
   4    1   1   0
   5   -1   1   0
   1    0  -1   0
   5    1  -1   0
   4   -1  -1   0
   1    0   0   1
   4    1   0   1
   5   -1   0   1
   4    0   1   1
   1    1   1   1
   5   -1   1   1
   5    0  -1   1
   5    1  -1   1
   5   -1  -1   1
   1    0   0  -1
   5    1   0  -1
   4   -1   0  -1
   5    0   1  -1
   5    1   1  -1
   5   -1   1  -1
   4    0  -1  -1
   5    1  -1  -1
   1   -1  -1  -1""")

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

result_ir_rec_mesh_silicon = """    0    0   0   0
   1    1   0   0
   2   -1   0   0
   1    0   1   0
   4    1   1   0
   5   -1   1   0
   2    0  -1   0
   5    1  -1   0
   2   -1  -1   0
   1    0   0   1
   4    1   0   1
   5   -1   0   1
   4    0   1   1
  13    1   1   1
   4   -1   1   1
   5    0  -1   1
   4    1  -1   1
   1   -1  -1   1
   2    0   0  -1
   5    1   0  -1
   2   -1   0  -1
   5    0   1  -1
   4    1   1  -1
   1   -1   1  -1
   2    0  -1  -1
   1    1  -1  -1
   0   -1  -1  -1"""

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
-1 -1 -1""", """  0   0   0
  1   0   0
 -1   0   0
  0   1   0
  1   1   0
 -1   1   0
  0  -1   0
  1  -1   0
 -1  -1   0
  0   0   1
  1   0   1
 -1   0   1
  0   1   1
  1   1   1
  2   1   1
  0  -1   1
  1   2   1
 -1  -1  -2
  0   0  -1
  1   0  -1
 -1   0  -1
  0   1  -1
  1   1   2
 -1  -2  -1
  0  -1  -1
 -2  -1  -1
 -1  -1  -1""")

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
 55  -1  -1  -1  -1  56""" , """  0   1  -1  -1  -1   2   3   4  -1  -1
 -1   5  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
  6   7  -1  -1  -1   8   9  10  -1  -1
 -1  11  12  13  14  -1  -1  -1  -1  16
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  15  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  22
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  17
 18  19  -1  -1  -1  20  21  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
 -1  -1  -1  -1  -1  -1  -1  -1  -1  23
 24  -1  -1  -1  25  26""")


class TestReciprocalMesh(unittest.TestCase):
    def setUp(self):
        identity = np.eye(3, dtype='intc')
        file_and_mesh = (
            [os.path.join(data_dir, "data", "cubic", "POSCAR-217"), [4, 4, 4]],
            [os.path.join(data_dir, "data", "hexagonal", "POSCAR-182"),
             [4, 4, 2]],
            [os.path.join(data_dir, "POSCAR_Si_prim"), [3, 3, 3]])

        self.meshes = []
        self.cells = []
        self.rotations = []
        self.grid_addresses = []
        for i, (fname, mesh) in enumerate(file_and_mesh):
            self.meshes.append(mesh)
            self.cells.append(read_vasp(fname))
            self.rotations.append(
                get_symmetry_dataset(self.cells[i])['rotations'])
            _, ga = get_stabilized_reciprocal_mesh(mesh, [identity, ])
            self.grid_addresses.append(ga)

    def tearDown(self):
        pass

    def test_get_ir_reciprocal_mesh(self):
        for i in range(len(self.cells)):
            ir_rec_mesh = get_ir_reciprocal_mesh(self.meshes[i], self.cells[i],
                                                 is_dense=False)
            (mapping_table, grid_address) = ir_rec_mesh
            # for gp, ga in zip(mapping_table, grid_address):
            #     print("%4d  %3d %3d %3d" % (gp, ga[0], ga[1], ga[2]))
            # print("")
            data = np.loadtxt(StringIO(result_ir_rec_mesh[i]), dtype='intc')
            np.testing.assert_equal(data[:, 0], mapping_table)
            np.testing.assert_equal(data[:, 1:4], grid_address)

            ir_rec_mesh = get_ir_reciprocal_mesh(self.meshes[i], self.cells[i],
                                                 is_dense=True)
            (mapping_table, grid_address) = ir_rec_mesh
            np.testing.assert_equal(data[:, 0], mapping_table)
            np.testing.assert_equal(data[:, 1:4], grid_address)

    def test_get_ir_reciprocal_mesh_distortion(self):
        j = 0
        for is_shift in ([0, 0, 0], [0, 1, 0]):
            for i, mesh in enumerate(([3, 4, 4], [3, 5, 1])):
                ir_rec_mesh = get_ir_reciprocal_mesh(mesh, self.cells[i],
                                                     is_shift=is_shift,
                                                     is_dense=False)
                (mapping_table, grid_address) = ir_rec_mesh
                # for gp, ga in zip(mapping_table, grid_address):
                #     print("%4d  %3d %3d %3d" % (gp, ga[0], ga[1], ga[2]))
                # print("")
                data = np.loadtxt(StringIO(result_ir_rec_mesh_distortion[j]),
                                  dtype='intc')
                np.testing.assert_equal(data[:, 0], mapping_table)
                np.testing.assert_equal(data[:, 1:4], grid_address)

                ir_rec_mesh = get_ir_reciprocal_mesh(mesh, self.cells[i],
                                                     is_shift=is_shift,
                                                     is_dense=True)
                (mapping_table, grid_address) = ir_rec_mesh
                np.testing.assert_equal(data[:, 0], mapping_table)
                np.testing.assert_equal(data[:, 1:4], grid_address)

                j += 1

    def test_get_ir_reciprocal_mesh_Si_shift_111(self):
        ir_rec_mesh = get_ir_reciprocal_mesh([3, 3, 3], self.cells[2],
                                             is_shift=[1, 1, 1],
                                             is_dense=False)
        (mapping_table, grid_address) = ir_rec_mesh
        # for gp, ga in zip(mapping_table, grid_address):
        #     print("%4d  %3d %3d %3d" % (gp, ga[0], ga[1], ga[2]))
        # print("")
        data = np.loadtxt(StringIO(result_ir_rec_mesh_silicon), dtype='intc')
        np.testing.assert_equal(data[:, 0], mapping_table)
        np.testing.assert_equal(data[:, 1:4], grid_address)

        ir_rec_mesh = get_ir_reciprocal_mesh([3, 3, 3], self.cells[2],
                                             is_shift=[1, 1, 1],
                                             is_dense=True)
        (mapping_table, grid_address) = ir_rec_mesh
        np.testing.assert_equal(data[:, 0], mapping_table)
        np.testing.assert_equal(data[:, 1:4], grid_address)

    def test_get_stabilized_reciprocal_mesh(self):
        for i in range(len(self.cells)):
            ir_rec_mesh = get_stabilized_reciprocal_mesh(
                self.meshes[i], self.rotations[i], is_dense=False)
            (mapping_table, grid_address) = ir_rec_mesh
            data = np.loadtxt(StringIO(result_ir_rec_mesh[i]), dtype='intc')
            np.testing.assert_equal(data[:, 0], mapping_table)
            np.testing.assert_equal(data[:, 1:4], grid_address)

            ir_rec_mesh = get_stabilized_reciprocal_mesh(
                self.meshes[i], self.rotations[i], is_dense=True)
            (mapping_table, grid_address) = ir_rec_mesh
            np.testing.assert_equal(data[:, 0], mapping_table)
            np.testing.assert_equal(data[:, 1:4], grid_address)

    def test_relocate_BZ_grid_address(self):
        for i, (cell, mesh, grid_address) in enumerate(
                zip(self.cells, self.meshes, self.grid_addresses)):
            reclat = np.linalg.inv(cell[0])
            bz_grid_address, bz_map = relocate_BZ_grid_address(
                grid_address,
                mesh,
                reclat,
                is_dense=False)

            # print(i)
            # for j in range(len(bz_map) // 10):
            #     print(("%3d " * 10) % tuple(bz_map[j * 10: (j + 1) * 10]))
            # n = len(bz_map) % 10
            # print(("%3d " * n) % tuple(bz_map[-n:]))
            # print("")

            # for adrs in bz_grid_address:
            #     print("%3d %3d %3d" % tuple(adrs))
            # print("")

            data_adrs = np.loadtxt(StringIO(result_bz_grid_address[i]),
                                   dtype='intc')
            np.testing.assert_equal(data_adrs, bz_grid_address)

            data_map = np.array([int(i) for i in result_bz_map[i].split()])
            data_map[data_map == -1] = np.prod(mesh) * 8
            np.testing.assert_equal(data_map, bz_map)

            bz_grid_address, bz_map = relocate_BZ_grid_address(
                grid_address,
                mesh,
                reclat,
                is_dense=True)
            np.testing.assert_equal(data_adrs, bz_grid_address)
            np.testing.assert_equal(data_map, bz_map)

    def test_get_grid_points_and_bz_grid_points_by_rotations(self):
        data = [[21, 55, 31, 61, 61, 21, 55, 31, 21, 55, 31, 61,
                 61, 21, 55, 31, 21, 55, 31, 61, 61, 21, 55, 31,
                 21, 55, 31, 61, 61, 21, 55, 31, 21, 55, 31, 61,
                 61, 21, 55, 31, 21, 55, 31, 61, 61, 21, 55, 31],
                [21, 30, 25, 31, 22, 27, 31, 22, 27, 21, 30, 25],
                [13, 3, 18, 1, 2, 26, 6, 9, 13, 3, 18, 1, 2, 26,
                 6, 9, 13, 3, 18, 1, 2, 26, 6, 9, 26, 6, 9, 2, 1,
                 13, 3, 18, 26, 6, 9, 2, 1, 13, 3, 18, 26, 6, 9,
                 2, 1, 13, 3, 18]]

        data_bz = [[21, 55, 31, 61, 61, 21, 55, 31, 21, 55, 31, 61,
                    61, 21, 55, 31, 21, 55, 31, 61, 61, 21, 55, 31,
                    21, 55, 31, 61, 61, 21, 55, 31, 21, 55, 31, 61,
                    61, 21, 55, 31, 21, 55, 31, 61, 61, 21, 55, 31],
                   [21, 30, 25, 31, 22, 27, 56, 43, 52, 42, 55, 48],
                   [13, 3, 18, 1, 2, 26, 6, 9, 13, 3, 18, 1, 2, 26,
                    6, 9, 13, 3, 18, 1, 2, 26, 6, 9, 26, 6, 9, 2, 1,
                    13, 3, 18, 26, 6, 9, 2, 1, 13, 3, 18, 26, 6, 9,
                    2, 1, 13, 3, 18]]

        for i, (cell, mesh, grid_address, rotations) in enumerate(
                zip(self.cells, self.meshes, self.grid_addresses,
                    self.rotations)):
            rec_rots = [r.T for r in rotations]

            gps = get_grid_points_by_rotations([1, 1, 1],
                                               rec_rots,
                                               mesh,
                                               is_dense=False)

            # The order of numbers of data[i] can change when the order
            # of rotations changes. But frequencies should not change.
            # for n in np.unique(data[i]):
            #     print(n, (data[i] == n).sum())
            # for n in np.unique(gps):
            #     print(n, (gps == n).sum())

            # print(i)
            # print(", ".join(["%d" % g for g in gps]))

            np.testing.assert_equal(data[i], gps)
            gps = get_grid_points_by_rotations([1, 1, 1],
                                               rec_rots,
                                               mesh,
                                               is_dense=True)
            np.testing.assert_equal(data[i], gps)

            bz_grid_address, bz_map = relocate_BZ_grid_address(
                grid_address,
                mesh,
                np.linalg.inv(cell[0]))
            bz_gps = get_BZ_grid_points_by_rotations([1, 1, 1],
                                                     rec_rots,
                                                     mesh,
                                                     bz_map,
                                                     is_dense=False)

            # print(i)
            # print(", ".join(["%d" % g for g in bz_gps]))

            np.testing.assert_equal(data_bz[i], bz_gps)
            diff_address = bz_grid_address[:len(grid_address)] - grid_address
            np.testing.assert_equal(diff_address % mesh, 0)

            bz_gps = get_BZ_grid_points_by_rotations([1, 1, 1],
                                                     rec_rots,
                                                     mesh,
                                                     bz_map,
                                                     is_dense=True)
            np.testing.assert_equal(data_bz[i], bz_gps)

    def test_get_grid_point_from_address(self):
        mesh = (5, 5, 5)
        adrs_ref = [0, 25, 50, 75, 100, 5, 30, 55, 80, 105, 10, 35, 60, 85,
                    110, 15, 40, 65, 90, 115, 20, 45, 70, 95, 120, 1, 26, 51,
                    76, 101, 6, 31, 56, 81, 106, 11, 36, 61, 86, 111, 16, 41,
                    66, 91, 116, 21, 46, 71, 96, 121, 2, 27, 52, 77, 102, 7,
                    32, 57, 82, 107, 12, 37, 62, 87, 112, 17, 42, 67, 92, 117,
                    22, 47, 72, 97, 122, 3, 28, 53, 78, 103, 8, 33, 58, 83,
                    108, 13, 38, 63, 88, 113, 18, 43, 68, 93, 118, 23, 48, 73,
                    98, 123, 4, 29, 54, 79, 104, 9, 34, 59, 84, 109, 14, 39,
                    64, 89, 114, 19, 44, 69, 94, 119, 24, 49, 74, 99, 124]
        adrs = [get_grid_point_from_address([i, j, k], mesh)
                for i, j, k in list(np.ndindex(mesh))]
        np.testing.assert_equal(adrs_ref, adrs)


if __name__ == '__main__':
    suite = unittest.TestLoader().loadTestsFromTestCase(TestReciprocalMesh)
    unittest.TextTestRunner(verbosity=2).run(suite)
    # unittest.main()
