"""Python bindings for C library for finding and handling crystal."""
# Copyright (C) 2015 Atsushi Togo
# This file is part of spglib.
# SPDX-License-Identifier: BSD-3-Clause

# TODO: _internal should not be exposed, we only continue to do so for
#  backwards compatibility
from ._internal import *  # noqa: F403
from ._internal import __all__ as _internal_all
from ._version import __version__, __version_tuple__
from .cell import *  # noqa: F403
from .cell import __all__ as _cell_all
from .error import *  # noqa: F403
from .error import __all__ as _error_all
from .kpoints import *  # noqa: F403
from .kpoints import __all__ as _kpoints_all
from .msg import *  # noqa: F403
from .msg import __all__ as _msg_all
from .reduce import *  # noqa: F403
from .reduce import __all__ as _reduce_all
from .spg import *  # noqa: F403
from .spg import __all__ as _spg_all
from .utils import *  # noqa: F403
from .utils import __all__ as _utils_all

__all__ = [
    "__version__",
    "__version_tuple__",
    *_internal_all,
    *_cell_all,
    *_error_all,
    *_kpoints_all,
    *_msg_all,
    *_reduce_all,
    *_spg_all,
    *_utils_all,
]
