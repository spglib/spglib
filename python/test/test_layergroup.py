from spglib import get_layergroup


def test_MoS2(root_data_dir, read_vasp):
    cell = read_vasp(root_data_dir / "layer" / "POSCAR-78")
    lg = get_layergroup(cell, aperiodic_dir=2, symprec=1e-5)
    assert lg["number"] == 78
