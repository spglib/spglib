import re

from load import get_msg_numbers, get_msg_table


def get_type_of_msg(hall_symbol):
    # awkward classification...
    if "'" not in hall_symbol:
        return 1  # type-I
    if " 1'" in hall_symbol:
        return 2  # type-II
    if len(re.findall(r" 1[a-z]+'", hall_symbol)) > 0:
        return 4  # type-IV
    return 3  # type-III


if __name__ == '__main__':
    """
    Create `msg_database.c:magnetic_spacegroup_types`
    """
    msg_numbers = get_msg_numbers()

    msg_table = get_msg_table()
    bns_to_mhall = {}
    for dct in msg_table.values():
        bns_to_mhall.update(dct)

    contents = []

    contents.append("static const MagneticSpacegroupType magnetic_spacegroup_types[] = {")
    contents.append('    {    0,    0,        "",            "",   0, 0 }, /* Error */')

    max_bns_number_length = 7
    max_og_number_length = 11

    type_counts = {t: 0 for t in [1, 2, 3, 4]}

    # Sorted by UNI number
    for i, (litvin_number, bns_number, og_number, uni_number) in enumerate(msg_numbers):
        number = int(bns_number.split('.')[0])

        # type of MSG
        hall_symbol = bns_to_mhall[bns_number]
        type_of_msg = get_type_of_msg(hall_symbol)
        type_counts[type_of_msg] += 1

        assert len(bns_number) <= max_bns_number_length
        assert len(og_number) <= max_og_number_length
        quoted_bns_number = f'"{bns_number}"'
        quoted_og_number = f'"{og_number}"'
        contents.append(f'    {{ {uni_number:4d}, {litvin_number:4d}, {quoted_bns_number:>9s}, {quoted_og_number:>13s}, {number:3d}, {type_of_msg:1d} }}, /* {i+1:4d} */')
        assert uni_number == i + 1

    # Sanity check
    print(type_counts)
    assert type_counts[1] == 230
    assert type_counts[2] == 230
    assert type_counts[3] == 674
    assert type_counts[4] == 517

    contents.append("};")

    for line in contents:
        print(line)
