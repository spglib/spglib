import sys

import numpy as np

from operation import MagneticOperation
from magnetic_hall import MagneticHallSymbol
from transform import Transformation, get_standard_hall_number
from load import get_msg_numbers, get_spg_table, get_msg_table

sys.path.append('..')
from hall2operations import encode_symmetry


def get_all_msg_settings():
    msg_table = get_msg_table()
    spg_table = get_spg_table()

    msg_numbers = get_msg_numbers()
    mapping = {}
    for _, bns_number, _, uni_number in msg_numbers:
        mapping[bns_number] = uni_number

    all_datum = []
    offset = 1
    for hall_number in range(1, 530 + 1):
        number = spg_table[hall_number]['number']
        choice = spg_table[hall_number]['choice']
        hall_number_std = get_standard_hall_number(number)
        # Transformation from ITA standard settings to the other settings
        trns = Transformation.to_standard(hall_number, choice, number).inverse()

        for bns_number, mhall_symbol in msg_table[hall_number_std].items():
            mhs = MagneticHallSymbol(mhall_symbol)
            coset = trns.transform_coset(mhs.coset)
            uni_number = mapping[bns_number]
            all_datum.append((coset, uni_number, hall_number, offset))
            offset += len(coset)

    return all_datum


def encode_magnetic_operation(ops: MagneticOperation) -> int:
    linear = np.around(ops.linear).astype(int)
    assert np.all(np.abs(linear) <= 1)  # sanity check
    rt_encoded = encode_symmetry(linear, ops.translation)
    if ops.time_reversal:
        rt_encoded += 34012224  # 3**9 * 12**3

    assert rt_encoded < (2 ** 31)  # within int
    return rt_encoded


if __name__ == "__main__":
    all_datum = get_all_msg_settings()
    contents = []

    # Dump `magnetic_symmetry_operations`
    # Magnetic symmetry operations is encoded as well for time_reversal=False.
    # For time_reversal=True operation, encoded number is shifted.
    contents.append("static const int magnetic_symmetry_operations[] = {")
    contents.append("           0, /*dummy*/")
    count = 1
    for i, (coset, uni_number, _, _) in enumerate(all_datum):
        for ops in coset:
            encoded = encode_magnetic_operation(ops)
            linear = np.around(ops.linear).astype(int)
            linear_str = "%2d, %2d, %2d, %2d, %2d, %2d, %2d, %2d, %2d" % tuple(linear.reshape(-1).tolist())
            translation_str = "%2d, %2d, %2d" % tuple(np.around(12 * ops.translation))
            time_reversal_str = "-1" if ops.time_reversal else " 1"
            contents.append(f'    {encoded:8d}, /* {count:5d} ({uni_number:4d}) [{linear_str}] [{translation_str}] {time_reversal_str} */')
            count += 1
    contents.append('};')
    contents.append('')

    # Dump `magnetic_spacegroup_operation_index`
    # magnetic_spacegroup_operation_index[uni_number][18][2] = array of {order, offset} of the Hall number in `magnetic_symmetry_operations`
    mapping = {uni_number: [] for uni_number in range(1, 1651 + 1)}
    for coset, uni_number, hall_number, offset in all_datum:
        order = len(coset)
        mapping[uni_number].append((hall_number, order, offset))

    contents.append("static const int magnetic_spacegroup_operation_index[][18][2] = {")
    contents.append("    { {0, 0} }, /* dummy */")
    count = 1
    for uni_number in range(1, 1651 + 1):
        hall_order_offset = mapping[uni_number]
        assert len(hall_order_offset) <= 18

        orders_offsets = []
        for _, order, offset in hall_order_offset:
            orders_offsets.append((order, offset))

        orders_offsets_str = ", ".join([f"{{ {order}, {offset} }}" for order, offset in orders_offsets])
        contents.append(f"    {{ {orders_offsets_str} }}, /* {uni_number} */")
    contents.append("};")

    for line in contents:
        print(line)
