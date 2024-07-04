import sys

if sys.version_info < (3, 9):
    from typing import Mapping
else:
    from collections.abc import Mapping


__all__ = [
    "Mapping",
]
