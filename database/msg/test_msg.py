from pathlib import Path
import csv
from itertools import combinations

import numpy as np
import pytest
from ruamel.yaml import YAML

from operation import MagneticOperation, remainder1_symmetry_operation
from magnetic_hall import MagneticHallSymbol
from transform import Transformation, get_standard_hall_number


def test_magnetic_operations():
    tc = MagneticOperation.from_linear_translation_time_reversal(
        translation=np.array([0, 0, 1 / 2]), time_reversal=True
    )
    tc2 = tc * tc
    assert tc2 == MagneticOperation.from_linear_translation_time_reversal(
        translation=np.array([0, 0, 1])
    )

    # Translation by (0, 0, -1/2) is equivalent to that by (0, 0, 1/2)
    assert remainder1_symmetry_operation(tc.inverse()) == tc


@pytest.mark.parametrize(
    "hall_symbol,order_expected", [
        ("P 31 2 1c' (0 0 4)", 2 * 6),  # 151.32 (type-IV)
        ("P 6c 2c' -1'", 24),  # 194.265 (type-III)
        ("F 4d 2 3 1'", 2 * 96),  # 210.53 (type-II)
    ]
)
def test_magnetic_hall_symbol(hall_symbol, order_expected):
    mhs = MagneticHallSymbol(hall_symbol)
    order_actual = len(mhs.coset)
    assert order_actual == order_expected


@pytest.fixture
def spg_table():
    all_datum = {}
    with open(Path("../spg.csv"), 'r') as f:
        reader = csv.reader(f, delimiter=',')
        for row in reader:
            hall_number, choice, number, hall_symbol = int(row[0]), row[2], int(row[4]), row[6]
            all_datum[hall_number] = {
                'choice': choice,
                'number': number,
                'hall_symbol': hall_symbol,
            }

    assert len(all_datum) == 530
    return all_datum


def test_transformation(spg_table):
    for hall_number in range(1, 530 + 1):
        print(hall_number)
        choice = spg_table[hall_number]['choice']
        number = spg_table[hall_number]['number']
        hall_symbol = spg_table[hall_number]['hall_symbol']
        coset_expected = MagneticHallSymbol(hall_symbol).coset

        hall_number_std = get_standard_hall_number(number)
        hall_symbol_std = spg_table[hall_number_std]['hall_symbol']
        coset_std = MagneticHallSymbol(hall_symbol_std).coset

        # Transform from ITA standard setting to alternative setting
        from_std = Transformation.to_standard(hall_number, choice, number).inverse()
        coset_actual = from_std.transform_coset(coset_std)

        assert set(coset_actual) == set(coset_expected)


@pytest.fixture
def msg_table():
    with open(Path("./magnetic_hall_symbols.yaml"), 'r') as f:
        all_datum = dict(YAML().load(f))
    return all_datum


def test_magnetic_hall_table(msg_table):
    # Test MSG table with ITA standard settings
    count = 0
    for number in range(1, 230 + 1):
        print(f"No. {number}")
        hall_number_std = get_standard_hall_number(number)
        serial_and_symbols = list(msg_table[hall_number_std].items())

        hs_sg = MagneticHallSymbol(serial_and_symbols[0][1])  # type-I

        list_msg = []
        for bns_number, hall_symbol in serial_and_symbols:
            print(f"  {bns_number}: {hall_symbol}")
            mhs = MagneticHallSymbol(hall_symbol)
            assert len(mhs.coset) % len(hs_sg.coset) == 0

            index = len(mhs.coset) // len(hs_sg.coset)
            # For type-I and type-III, index == 1
            # For type-II and type-IV, index == 2
            assert (index == 1) or (index == 2)

            list_msg.append(set(mhs.coset))

            count += 1

        # check no duplicates
        for i, j in combinations(range(len(list_msg)), r=2):
            if list_msg[i] == list_msg[j]:
                print(serial_and_symbols[i], serial_and_symbols[j])
                assert False

    assert count == 1651
