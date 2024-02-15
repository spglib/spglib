import pytest
from spglib.spglib import _expand_cell


def test_expand_cell():
    with pytest.raises(TypeError, match="cell has to be a tuple of 3 or 4 elements."):
        _expand_cell(([], [], [], [], []))
