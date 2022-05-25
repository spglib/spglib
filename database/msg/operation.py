from __future__ import annotations

from typing import List
from fractions import Fraction
from queue import Queue

import numpy as np
from numpy.typing import NDArray


# Maximal denominator of fractional coordinates of translations
DENOMINATOR = 12


class MagneticOperation:

    ndim = 3

    def __init__(self, augmented_matrix: NDArray, time_reversal: bool):
        self._augmented_matrix = augmented_matrix
        self._time_reversal = time_reversal

        self._linear = self.augmented_matrix[:self.ndim, :self.ndim]
        self._translation = self.augmented_matrix[:self.ndim, -1]

        # For comparison, store linear and translations as tuple of integers
        self._linear_tuple = ndarray_to_integer_tuple(
            self.augmented_matrix[: self.ndim, : self.ndim]
        )
        self._translation_tuple = tuple(
            Fraction(t).limit_denominator(DENOMINATOR)
            for t in self.augmented_matrix[: self.ndim, -1]
        )

    def __eq__(self, other) -> bool:
        if not isinstance(other, MagneticOperation):
            return False

        return (self._linear_tuple == other._linear_tuple) and (
            self._translation_tuple == other._translation_tuple
        ) and (
            self.time_reversal == other.time_reversal
        )

    def __hash__(self):
        return hash((self._linear_tuple, self._translation_tuple, self.time_reversal))

    def __mul__(self, rhs: MagneticOperation) -> MagneticOperation:
        if not isinstance(rhs, MagneticOperation):
            raise ValueError("undefined operation")

        return MagneticOperation(
            augmented_matrix=np.dot(self.augmented_matrix, rhs.augmented_matrix),
            time_reversal=(self.time_reversal != rhs.time_reversal),
        )

    def inverse(self) -> MagneticOperation:
        return MagneticOperation(
            augmented_matrix=np.linalg.inv(self.augmented_matrix),
            time_reversal=self.time_reversal,
        )

    @property
    def linear(self) -> NDArray:
        return self._linear

    @property
    def translation(self) -> NDArray:
        return self._translation

    @property
    def time_reversal(self) -> bool:
        return self._time_reversal

    @property
    def augmented_matrix(self) -> NDArray:
        return self._augmented_matrix

    @classmethod
    def identity(cls) -> MagneticOperation:
        return MagneticOperation.from_linear_translation_time_reversal()

    @classmethod
    def from_linear_translation_time_reversal(cls, linear=None, translation=None, time_reversal=False):
        aug = np.zeros((cls.ndim + 1, cls.ndim + 1))
        aug[-1, -1] = 1

        if linear is None:
            aug[:cls.ndim, :cls.ndim] = np.eye(cls.ndim)
        else:
            aug[:cls.ndim, :cls.ndim] = linear

        if translation is not None:
            aug[:cls.ndim, -1] = translation

        return cls(aug, time_reversal)


def ndarray_to_integer_tuple(array: NDArray):
    array_int = np.around(array).astype(int)
    array_t = tuple(map(tuple, array_int))
    return array_t


def remainder1_with_denominator(arr: NDArray, denominator: int) -> NDArray:
    """
    return arr (mod 1)
    """
    arr_int = np.around(arr * denominator).astype(int)
    arr_int_mod = np.remainder(arr_int, denominator)
    arr_mod = arr_int_mod / denominator
    return arr_mod


def remainder1_symmetry_operation(
    ops: MagneticOperation,
) -> MagneticOperation:
    new_translation = remainder1_with_denominator(ops.translation, DENOMINATOR)
    return MagneticOperation.from_linear_translation_time_reversal(
        ops.linear, new_translation, ops.time_reversal
    )


def traverse(
    generators: List[MagneticOperation],
) -> List[MagneticOperation]:
    """
    Generate all coset operations
    """
    coset = set()
    que = Queue()  # type: ignore
    identity = MagneticOperation.identity()
    que.put(identity)

    while not que.empty():
        g = que.get()
        if g in coset:
            continue
        coset.add(g)

        for h in generators:
            # Take modulus by translation subgroup
            gh = remainder1_symmetry_operation(g * h)
            que.put(gh)

    # Put identity in the first
    coset.remove(identity)
    ret = [identity] + list(coset)  # type: ignore

    return ret
