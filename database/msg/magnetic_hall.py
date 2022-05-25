from __future__ import annotations

import sys

import numpy as np
from numpy.typing import NDArray

from operation import MagneticOperation, traverse

sys.path.append("../")
from hall2operations import lattice_symbols, rotation_matrices, translations


class MagneticHallSymbol:
    """
    Parser for magnetic Hall symbol. See A1.4.2.3 in ITB (2010).

    Extended Backus-Naur form (EBNF) for Magnetic Hall symbols
    ----------------------------------------------------------
    <Hall symbol>    := <L> <N>+ <V>?
    <L>              := "-"? <lattice symbol>
    <lattice symbol> := [PABCIRHF]  # Table A1.4.2.2
    <N>              := <nfold> <A>? <T>? "'"?  # "'" represents time reversal
    <nfold>          := "-"? ("1" | "2" | "3" | "4" | "6")
    For SG,  <A>     := [xyz] | "'" | '"' | "=" | "*"  # Table A.1.4.2.4, Table A.1.4.2.5, Table A.1.4.2.6
    For MSG, <A>     := [xyz] | "^" | '"' | "=" | "*"  # The original axis symbol "'" is substituted with "^"
    <T>              := [abcnuvwd] | [1-6]  # Table A.1.4.2.3
    <V>              := "(" [0-11] [0-11] [0-11] ")"
    """
    def __init__(self, hall_symbol: str):
        self._hall_symbol = hall_symbol
        self._parse(magnetic=True)
        self._coset = traverse(self.generators)

    @property
    def hall_symbol(self) -> str:
        return self._hall_symbol

    @property
    def inversion_at_origin(self) -> bool:
        return self._inversion_at_origin

    @property
    def lattice_symbol(self) -> str:
        return self._lattice_symbol

    @property
    def generators(self):
        return self._generators

    @property
    def coset(self):
        """
        Coset representative of magnetic space group over its translational subgroup
        """
        return self._coset

    def _parse(self, magnetic: bool = True):
        tokens = self.hall_symbol.split(" ")

        assert len(tokens) >= 1
        self._parse_L(tokens[0])

        self._Ns = []
        rotation_count = 0

        self._V = np.zeros(3)
        prev_nfold = None
        prev_axis = None
        for i in range(1, len(tokens)):
            if tokens[i][0] == "(":
                # tokens[i:] = ["(" + vx, vy, vz + ")"]
                self._V = self._parse_V(tokens[i:])
                break
            else:
                # We need to remember the location of "N" symbols and preceding "N" symbol
                # because its default axis depends on them!
                rotation, translation, nfold, axis, time_reversal = self._parse_N(
                    tokens[i], rotation_count, prev_nfold, prev_axis, magnetic
                )
                self._Ns.append((rotation, translation, time_reversal))
                prev_nfold = nfold
                prev_axis = axis
                rotation_count += 1

        # change basis by (I, v)
        # (R, tau) -> (R, tau + v - Rv)
        self._generators = []
        for t in lattice_symbols[self.lattice_symbol]:  # type: ignore
            if not np.allclose(t, 0):
                ops = MagneticOperation.from_linear_translation_time_reversal(translation=np.array(t))
                self._generators.append(ops)

        if self.inversion_at_origin:
            r, t = self.change_of_basis(-np.eye(3), np.zeros(3))
            ops = MagneticOperation.from_linear_translation_time_reversal(linear=r, translation=t)
            self._generators.append(ops)

        for rotation, translation, time_reversal in self._Ns:
            r, t = self.change_of_basis(rotation, translation)
            ops = MagneticOperation.from_linear_translation_time_reversal(linear=r, translation=t, time_reversal=time_reversal)
            self._generators.append(ops)

    def _parse_L(self, token):
        if token[0] == "-":
            self._inversion_at_origin = True
            self._lattice_symbol = token[1:]
        else:
            self._inversion_at_origin = False
            self._lattice_symbol = token

    @staticmethod
    def _parse_N(
        token, count: int, prev_nfold: str | None, prev_axis: str | None, magnetic: bool = False
    ):
        pos = 0

        # improper
        improper = False
        if token[pos] == "-":
            pos += 1
            improper = True

        # nfold
        nfold = token[pos]
        pos += 1

        # change axis symbol for MHall
        if magnetic:
            prime_axis_symbol = "^"
        else:
            prime_axis_symbol = "'"

        # axis
        axis = ""
        if pos <= len(token) - 1:
            if token[pos] == prime_axis_symbol:
                axis = "p"
                pos += 1
            elif token[pos] in ['"', '=']:
                # "
                axis = "pp"
                pos += 1

        if pos <= len(token) - 1:
            if token[pos] in ["x", "y", "z", "*"]:
                axis = token[pos]
                pos += 1
        if (axis in ["p", "pp"]) and (prev_axis in ["x", "y", "z"]):
            # See Table A1.4.2.5
            axis = axis + prev_axis

        # No need to specify axis for nfold=1
        if nfold == "1":
            axis = "z"

        # Default axes. See A1.4.2.3.1
        if axis in ["", "p", "pp"]:
            if count == 0:
                # axis direction of c
                axis += "z"
            elif count == 1:
                if prev_nfold in ["2", "4"]:
                    # axis direction of a
                    axis += "x"
                elif prev_nfold in ["3", "6"]:
                    # axis direction of a-b
                    axis = "pz"
                else:
                    raise ValueError("unreachable!")
            elif count == 2:
                if nfold == "3":
                    # axis direction of a+b+c
                    axis += "*"
                else:
                    raise ValueError("unreachable!")
            else:
                raise ValueError("unreachable!")

        rotation = np.array(rotation_matrices[nfold + axis])
        if improper:
            rotation = -rotation

        # translation
        translation = np.zeros(3)
        while pos <= len(token) - 1:
            # translations are applied additively
            if token[pos] in "123456":
                # always along z-axis!
                translation = [0, 0, int(token[pos]) / abs(int(nfold))]
            elif token[pos] in "abcnuvwd":
                translation += translations[token[pos]]
            else:
                break
            pos += 1

        # For magnetic hall symbols
        time_reversal = False
        if magnetic and (pos < len(token)) and (token[pos] == "'"):
            pos += 1
            time_reversal = True

        assert pos == len(token)
        return rotation, translation, nfold, axis, time_reversal

    @staticmethod
    def _parse_V(tokens):
        assert len(tokens) == 3
        assert tokens[0][0] == "("
        vx = int(tokens[0][1:])
        vy = int(tokens[1])
        vz = int(tokens[2][:-1])
        assert tokens[2][-1] == ")"

        translation = np.array([vx / 12, vy / 12, vz / 12])

        return translation

    def change_of_basis(self, rotation: NDArray, translation: NDArray):
        new_translation = translation + self._V - np.dot(rotation, self._V)
        return rotation, new_translation
