from __future__ import annotations

from typing import List

import numpy as np
from numpy.typing import NDArray

from operation import MagneticOperation, remainder1_symmetry_operation


# ref: ITA Table1.5.1.1
transformation_matrix_monoclinic = {
    "a_to_b": np.array([[0, 1, 0], [0, 0, 1], [1, 0, 0]]),  # (a, b, c) -> (c, a, b)
    "-a_to_b": np.array([[0, -1, 0], [1, 0, 0], [0, 0, 1]]),  # (a, b, c) -> (b, -a, c)
    "-b_to_b": np.array([[0, 0, 1], [0, -1, 0], [1, 0, 0]]),  # (a, b, c) -> (c, -b, a)
    "c_to_b": np.array([[0, 0, 1], [1, 0, 0], [0, 1, 0]]),  # (a, b, c) -> (b, c, a)
    "-c_to_b": np.array([[1, 0, 0], [0, 0, 1], [0, -1, 0]]),  # (a, b, c) -> (a, -c, b)
    "b2_to_b1": np.array([[0, 0, -1], [0, 1, 0], [1, 0, -1]]),  # P -> P, A -> C
    "b3_to_b1": np.array([[-1, 0, 1], [0, 1, 0], [-1, 0, 0]]),  # P -> P, I -> C
}

transformation_matrix_orthorhombic = {
    "abc": np.eye(3),
    "ba-c": np.array([[0, 1, 0], [1, 0, 0], [0, 0, -1]]),
    "cab": np.array([[0, 0, 1], [1, 0, 0], [0, 1, 0]]),
    "-cba": np.array([[0, 0, -1], [0, 1, 0], [1, 0, 0]]),
    "bca": np.array([[0, 1, 0], [0, 0, 1], [1, 0, 0]]),
    "a-cb": np.array([[1, 0, 0], [0, 0, -1], [0, 1, 0]]),
}


class Transformation:
    """
    Transformation of coordinate systems

    Consider change of a coordinate system from lattice basis (a_org, b_org, c_org) and origin O to (a_new, b_new, c_new) and O'.
    The "linear part" P relates lattice basises as:
    lattice basis is changed as,
        (a_new, b_new, c_new) = (a_org, b_org, c_org) P
    We choose original basis for the origin shift,
        p = (p1_org, p2_org, p3_org)^T s.t. p1_org * a_org + p2_org * b_arg + p3_org * c_arg = O' - O
    Then, fractional coordinates are changed as:
        (x_new, y_new, z_new)^T = (P, p)^{-1} (x_org, y_org, z_org)^T
    Symmetry operation is transformed as:
        (W, w) -> (P, p)^{-1} (W, w) (P, p) = (P^{-1}WP, P^{-1}(W * p + w - p))
    """

    ndim = 3

    def __init__(self, linear: NDArray, origin_shift=None):
        self._linear = linear  # linear part
        self._origin_shift = origin_shift  # shift vector
        if origin_shift is None:
            self._origin_shift = np.zeros(self.ndim)

        self._augmented_matrix = np.zeros((self.ndim + 1, self.ndim + 1))
        self._augmented_matrix[:self.ndim, :self.ndim] = self.linear
        self._augmented_matrix[:self.ndim, -1] = self.origin_shift
        self._augmented_matrix[-1, -1] = 1

        self._invaug = np.linalg.inv(self.augmented_matrix)

    @property
    def linear(self):
        return self._linear

    @property
    def origin_shift(self):
        return self._origin_shift

    @property
    def augmented_matrix(self):
        return self._augmented_matrix

    def transform_operation(
        self, ops: MagneticOperation
    ) -> MagneticOperation:
        """
        transform lattice vector A maps to AP, where A is column-wise.
        (W, w) -> (P, p)^{-1} (W, w) (P, p) = (P^{-1}WP, P^{-1}(W * p + w - p))
        """
        new_aug = np.dot(self._invaug, np.dot(ops.augmented_matrix, self.augmented_matrix))
        return MagneticOperation(new_aug, ops.time_reversal)

    def transform_coset(
        self, coset: List[MagneticOperation]
    ) -> List[MagneticOperation]:
        transformed = []
        for ops in coset:
            remainded = remainder1_symmetry_operation(self.transform_operation(ops))
            transformed.append(remainded)

        # When transform from conventional to primitive cell, the number of unique operations decreases.
        size = int(np.around(1 / abs(np.linalg.det(self.linear))))
        if size > 1:
            identity = MagneticOperation.identity()
            transformed_set = set(transformed)
            transformed_set.remove(identity)
            transformed = [identity] + list(transformed_set)
            assert len(transformed) * size == len(coset)

        return transformed

    def inverse(self):
        """
        (P, p)^{-1} = (P^{-1}, -P^{-1}p)
        """
        return Transformation(linear=self._invaug[:self.ndim, :self.ndim], origin_shift=self._invaug[:self.ndim, -1])

    @classmethod
    def to_standard(cls, hall_number: int, choice: str, number: int) -> Transformation:
        """
        Transformation to ITA standard setting
        """
        if len(choice) >= 1 and (choice[0] in ["1", "2"]):
            origin_choice = choice[0]
            axes_choice = choice[1:]
        else:
            origin_choice = ""
            axes_choice = choice[:]

        crystal_system = get_crystal_system(hall_number)
        if crystal_system == "monoclinic":
            t_linear = cls._get_monoclinic_transformation_matrix(axes_choice)
        elif crystal_system == "orthorhombic":
            t_linear = cls._get_orthorhombic_transformation_matrix(axes_choice)
        elif crystal_system == "trigonal":
            t_linear = cls._get_trigonal_transformation_matrix(choice)
        else:
            t_linear = np.eye(3)

        # translate inversion center
        origin_shift = None

        if origin_choice == "1":
            # origin shift from 1 to 2
            shift_mapping = {
                48: np.array([1 / 4, 1 / 4, 1 / 4]),
                50: np.array([1 / 4, 1 / 4, 0]),
                59: np.array([1 / 4, 1 / 4, 0]),
                68: np.array([0, 1 / 4, 1 / 4]),
                70: np.array([1 / 8, 1 / 8, 1 / 8]),
                85: np.array([-1 / 4, 1 / 4, 0]),
                86: np.array([-1 / 4, -1 / 4, -1 / 4]),
                88: np.array([0, 1 / 4, 1 / 8]),
                125: np.array([-1 / 4, -1 / 4, 0]),
                126: np.array([-1 / 4, -1 / 4, -1 / 4]),
                129: np.array([-1 / 4, 1 / 4, 0]),
                130: np.array([-1 / 4, 1 / 4, 0]),
                133: np.array([-1 / 4, 1 / 4, -1 / 4]),
                134: np.array([-1 / 4, 1 / 4, -1 / 4]),
                137: np.array([-1 / 4, 1 / 4, -1 / 4]),
                138: np.array([-1 / 4, 1 / 4, -1 / 4]),
                141: np.array([1 / 2, 1 / 4, 1 / 8]),
                142: np.array([1 / 2, 1 / 4, 1 / 8]),
                201: np.array([1 / 4, 1 / 4, 1 / 4]),
                203: np.array([1 / 8, 1 / 8, 1 / 8]),
                222: np.array([1 / 4, 1 / 4, 1 / 4]),
                224: np.array([1 / 4, 1 / 4, 1 / 4]),
                227: np.array([1 / 8, 1 / 8, 1 / 8]),
                228: np.array([3 / 8, 3 / 8, 3 / 8]),
            }
            origin_shift = np.dot(t_linear, shift_mapping[number])

        return Transformation(t_linear, origin_shift)

    @staticmethod
    def _get_monoclinic_transformation_matrix(choice):
        """
        transformation matrix to b1 (ITA standard setting)
        """
        cell_choice = 1
        setting = "b"

        # choice in hall symbols dataset is in '-?[abc][1-3]?'
        if choice[0] == "-":
            setting = choice[:2]  # like "-a"
        else:
            setting = choice[0]  # like "a"

        if choice[-1].isdigit():
            cell_choice = int(choice[-1])  # like 3

        P = np.eye(3)
        # to unique axis b
        if setting != "b":
            P = transformation_matrix_monoclinic["{}_to_b".format(setting)]

        # to choice b1
        if cell_choice == 2:
            P = np.dot(P, transformation_matrix_monoclinic["b2_to_b1"])
        elif cell_choice == 3:
            P = np.dot(P, transformation_matrix_monoclinic["b3_to_b1"])
        return P

    @staticmethod
    def _get_orthorhombic_transformation_matrix(choice):
        if choice == "":
            setting = "abc"
        else:
            setting = choice
        # to abc
        P = transformation_matrix_orthorhombic[setting]
        return P

    @staticmethod
    def _get_trigonal_transformation_matrix(choice):
        if choice == "R":
            # Primitive rhombohedral cell to triple hexagonal cell R1, obverse setting
            return np.array(
                [
                    [1, 0, 1],
                    [-1, 1, 1],
                    [0, -1, 1],
                ],
                dtype=int,
            )
        else:
            # 'H' and ''
            return np.eye(3)


def get_crystal_system(hall_number: int) -> str:
    crystal_system_range = {
        "triclinic": [1, 2],
        "monoclinic": [3, 107],
        "orthorhombic": [108, 348],
        "tetragonal": [349, 429],
        "trigonal": [430, 461],
        "hexagonal": [462, 488],
        "cubic": [489, 530],
    }

    crystal_system = None
    for csystem, (lb, ub) in crystal_system_range.items():
        if (lb <= hall_number) and (hall_number <= ub):
            crystal_system = csystem
    if crystal_system is None:
        raise ValueError("Unknown Hall number: {}".format(hall_number))
    return crystal_system


def get_standard_hall_number(number: int) -> int:
    """
    convert ITA number with standard setting to Hall number
    """
    # fmt: off
    mapping = [
        # triclinic
        1,
        2,
        # monoclinic
        3,  # 3:b
        6,  # 4:b
        9,  # 5:b1
        18,  # 6:b
        21,  # 7:b1
        30,  # 8:b1
        39,  # 9:b1
        57,  # 10:b
        60,  # 11:b
        63,  # 12:b1
        72,  # 13:b1
        81,  # 14:b1
        90,  # 15:b1
        # orthorhombic
        108,  # 16
        109,  # 17
        112,  # 18
        115,  # 19
        116,  # 20
        119,  # 21
        122,  # 22
        123,  # 23
        124,  # 24
        125,  # 25
        128,  # 26
        134,  # 27
        137,  # 28
        143,  # 29
        149,  # 30
        155,  # 31
        161,  # 32
        164,  # 33
        170,  # 34
        173,  # 35
        176,  # 36
        182,  # 37
        185,  # 38
        191,  # 39
        197,  # 40
        203,  # 41
        209,  # 42
        212,  # 43
        215,  # 44
        218,  # 45
        221,  # 46
        227,  # 47
        229,  # 48:2
        230,  # 49
        234,  # 50:2
        239,  # 51
        245,  # 52
        251,  # 53
        257,  # 54
        263,  # 55
        266,  # 56
        269,  # 57
        275,  # 58
        279,  # 59:2
        284,  # 60
        290,  # 61
        292,  # 62
        298,  # 63
        304,  # 64
        310,  # 65
        313,  # 66
        316,  # 67
        323,  # 68:2
        334,  # 69
        336,  # 70:2
        337,  # 71
        338,  # 72
        341,  # 73
        343,  # 74
        # tetragonal
        349,  # 75
        350,  # 76
        351,  # 77
        352,  # 78
        353,  # 79
        354,  # 80
        355,  # 81
        356,  # 82
        357,  # 83
        358,  # 84
        360,  # 85:2
        362,  # 86:2
        363,  # 87
        365,  # 88:2
        366,  # 89
        367,  # 90
        368,  # 91
        369,  # 92
        370,  # 93
        371,  # 94
        372,  # 95
        373,  # 96
        374,  # 97
        375,  # 98
        376,  # 99
        377,  # 100
        378,  # 101
        379,  # 102
        380,  # 103
        381,  # 104
        382,  # 105
        383,  # 106
        384,  # 107
        385,  # 108
        386,  # 109
        387,  # 110
        388,  # 111
        389,  # 112
        390,  # 113
        391,  # 114
        392,  # 115
        393,  # 116
        394,  # 117
        395,  # 118
        396,  # 119
        397,  # 120
        398,  # 121
        399,  # 122
        400,  # 123
        401,  # 124
        403,  # 125:2
        405,  # 126:2
        406,  # 127
        407,  # 128
        409,  # 129:2
        411,  # 130:2
        412,  # 131
        413,  # 132
        415,  # 133:2
        417,  # 134:2
        418,  # 135
        419,  # 136
        421,  # 137:2
        423,  # 138:2
        424,  # 139
        425,  # 140
        427,  # 141:2
        429,  # 142:2
        # trigonal
        430,  # 143
        431,  # 144
        432,  # 145
        433,  # 146:H
        435,  # 147
        436,  # 148:H
        438,  # 149
        439,  # 150
        440,  # 151
        441,  # 152
        442,  # 153
        443,  # 154
        444,  # 155:H
        446,
        447,
        448,
        449,
        450,  # 160:H
        452,  # 161:H
        454,
        455,
        456,
        457,
        458,  # 166:H
        460,  # 167:H
        # hexagonal
        462,
        463,
        464,
        465,
        466,
        467,
        468,
        469,
        470,
        471,
        472,
        473,
        474,
        475,
        476,
        477,
        478,
        479,
        480,
        481,
        482,
        483,
        484,
        485,
        486,
        487,
        488,
        # cubic
        489,
        490,
        491,
        492,
        493,
        494,  # 200
        496,  # 201:2
        497,  # 202
        499,  # 203:2
        500,
        501,
        502,
        503,
        504,
        505,
        506,
        507,
        508,
        509,
        510,
        511,
        512,
        513,
        514,
        515,
        516,
        517,  # 221
        519,  # 222:2
        520,  # 223
        522,  # 224:2
        523,  # 225
        524,  # 226
        526,  # 227:2
        528,  # 228:2
        529,
        530,
    ]
    # fmt: on
    return mapping[number - 1]
