import unittest

from spglib import get_layergroup
from vasp import read_vasp
import os
from spglib import get_layergroup

data_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)),'data')
layer_path = os.path.join(data_dir, 'layer')

class TestLayerGroup(unittest.TestCase):
    def test_MoS2(self):
        cell = read_vasp(os.path.join(layer_path,'POSCAR-78'))
        lg = get_layergroup(cell, aperiodic_dir=2, symprec=1e-5)
        assert lg['number'] == 78


if __name__ == "__main__":
    suite = unittest.TestLoader().loadTestsFromTestCase(TestSpglib)
    unittest.TextTestRunner(verbosity=2).run(suite)

