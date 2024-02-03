import pytest
from spglib.spglib import _expand_cell


def test_expand_cell():
    with pytest.raises(TypeError) as e:
        _expand_cell(None)
    assert e.value
