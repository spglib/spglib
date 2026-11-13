"""Exception types raised in the python API.

.. warning::
   Exception support is still a work in progress. Not all exceptions
   may be raised as :py:exc:`SpglibError`. Please report any exceptions
   that are not caught by :py:exc:`SpglibError`.

.. deprecated:: 2.7.0
   Exception raising is now the default. To retain the deprecated error
   handling temporarily, set :py:attr:`spglib.error.OLD_ERROR_HANDLING` to
   ``True`` or set ``SPGLIB_OLD_ERROR_HANDLING`` to ``1``.

   For compatibility with older releases, the following snippet explicitly
   enables exception raising.

   .. code-block:: python

       try:
           spglib.error.OLD_ERROR_HANDLING = False
       except AttributeError:
           pass

   or alternatively you can enable it by setting the environment variable
   ``SPGLIB_OLD_ERROR_HANDLING`` to either ``0`` or ``False``.

   The old error handling will be removed in version 3.0. Please test out
   exception raising and provide feedback on the GitHub issues.

   Note that :py:attr:`spglib.error.OLD_ERROR_HANDLING` will be removed in a
   future version, so the snippet above is not advised to be used for long-term
   support. Use it only for experimentations and preparing the code base to
   ``try ... except`` the spglib api calls.

.. versionadded:: 2.7.0
"""

# Copyright (C) 2015 Atsushi Togo
# This file is part of spglib.
# SPDX-License-Identifier: BSD-3-Clause
from __future__ import annotations

import os
import warnings

from ._compat.warnings import deprecated

__all__ = [
    "SpglibError",
    "get_error_message",
    "OLD_ERROR_HANDLING",
]


class SpglibError(Exception):
    r"""Base exception type for all errors raised by spglib.

    Use this error type in a ``try ... except`` to process any physical or
    other spglib defined errors, such as the number of atom types not matching
    the positions or the distance between atoms being too close. For example:

    .. code-block:: python

       symprec = 1e-10
       while (symprec<1):
           try:
               dataset = spglib.get_symmetry(cell, symprec)
           except spglib.SpglibError as exc:
               # Expected issues by spglib
               # Try again if the symprec was too tight
               if str(exc) == "too close distance between atoms":
                   symprec *= 10
                   continue
               # Otherwise fail gracefully
               print(f"Failed to calculate symmetry:\n{exc}")
               break
           except Exception:
               # Unexpected issues, raise as an unexpected exception
               raise

    .. note::
       More fine-grained exceptions are not defined yet. Please provide
       feedback on what kind of exception separation would be desired.
    """


# TODO: Provide more exception types.


# Old deprecated error functions
OLD_ERROR_HANDLING: bool = False
"""
Use the old error handling.

Note that this variable may be removed in the future or change value in the future.
You can also use :envvar:`SPGLIB_OLD_ERROR_HANDLING` instead of altering this value.
"""


_spglib_error = ""


def _check_OLD_ERROR_HANDLING() -> bool:
    env_var = os.environ.get("SPGLIB_OLD_ERROR_HANDLING")
    if env_var is not None:
        if env_var.lower() in ("false", "0"):
            return False
        return True
    return OLD_ERROR_HANDLING


def _set_or_throw_error(exc: Exception, _throw: bool = False) -> None:
    if _throw or not _check_OLD_ERROR_HANDLING():
        if isinstance(exc, SpglibError):
            # Our native errors we pass transparently
            raise exc
        # Otherwise we try to recast them to SplibError
        raise SpglibError(f"Generic Spglib error:\n{exc}") from exc
    warnings.warn(
        "Set OLD_ERROR_HANDLING to false and catch the errors directly.",
        DeprecationWarning,
        stacklevel=2,
    )
    _spglib_error = str(exc)


def _set_no_error(_throw: bool = False) -> None:
    if _throw or not _check_OLD_ERROR_HANDLING():
        return
    warnings.warn(
        "Set OLD_ERROR_HANDLING to false and catch the errors directly.",
        DeprecationWarning,
        stacklevel=2,
    )
    _spglib_error = "no error"


@deprecated("Set OLD_ERROR_HANDLING to false and catch the errors directly")
def get_error_message() -> str:
    """Return error message why spglib failed.

    .. warning::
        This method is not thread safe, i.e., only safely usable
        when calling one spglib method per process.

    .. versionadded:: 1.9.5
    .. deprecated:: 2.7.0

    """
    return _spglib_error
