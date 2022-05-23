from transform import  get_standard_hall_number
from load import get_msg_numbers, get_spg_table, get_msg_table


if __name__ == '__main__':
    """
    Create magnetic_spacegroup_uni_mapping[uni_number][2] = {number of corresponding Hall numbers, the smallest Hall number}
    """
    # mapping: bns_number -> uni_number
    msg_numbers = get_msg_numbers()
    mapping = {}
    for _, bns_number, _, uni_number in msg_numbers:
        mapping[bns_number] = uni_number

    spg_table = get_spg_table()
    msg_table = get_msg_table()
    all_datum = {}
    for hall_number in range(1, 530 + 1):
        number = spg_table[hall_number]['number']
        hall_number_std = get_standard_hall_number(number)

        for bns_number in msg_table[hall_number_std].keys():
            uni_number = mapping[bns_number]
            if uni_number in all_datum:
                all_datum[uni_number].append(hall_number)
            else:
                all_datum[uni_number] = [hall_number, ]

    contents = []
    contents.append("static const int magnetic_spacegroup_uni_mapping[][2] = {")
    contents.append("    {}, /* dummy */")
    for uni_number, hall_numbers in all_datum.items():
        size = len(hall_numbers)
        first_hall_number = min(hall_numbers)
        contents.append(f'    {{ {size:2d}, {first_hall_number:3d} }}, /* {uni_number} */')
    contents.append("};")

    for line in contents:
        print(line)
