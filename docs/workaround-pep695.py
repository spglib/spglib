# /// script
# ///
"""Workaround for sphinx and pep695 support.

Sphinx currently only properly supports typealias documentation
using the PEP695 format (sphinx#13508, sphinx#8934).

We cannot use this new standard until the minimum python version
is 3.12. As a workaround, we can just convert these for the
documentation generation on RTD.
"""

from __future__ import annotations

import re
from pathlib import Path

ROOT_DIR = Path(__file__).parent.parent
PYTHON_API_DIR = ROOT_DIR / "python/spglib"


for py_file in PYTHON_API_DIR.glob("**/*.py"):
    with py_file.open("r+") as f:
        content = f.read()
        content = re.sub(r"(.+): TypeAlias =", r"type \1 =", content)
        f.seek(0)
        f.write(content)
        f.truncate()
