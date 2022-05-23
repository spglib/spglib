from load import get_msg_numbers


if __name__ == '__main__':
    """
    Create `msg_database.c:magnetic_spacegroup_types`
    """
    msg_numbers = get_msg_numbers()

    contents = []

    contents.append("static const MagneticSpacegroupType magnetic_spacegroup_types[] = {")
    contents.append('    {    0,    0,        "",            "",   0 }, /* Error */')

    max_bns_number_length = 7
    max_og_number_length = 11

    # Sorted by UNI number
    for i, (litvin_number, bns_number, og_number, uni_number) in enumerate(msg_numbers):
        number = int(bns_number.split('.')[0])
        assert len(bns_number) <= max_bns_number_length
        assert len(og_number) <= max_og_number_length
        quoted_bns_number = f'"{bns_number}"'
        quoted_og_number = f'"{og_number}"'
        contents.append(f'    {{ {uni_number:4d}, {litvin_number:4d}, {quoted_bns_number:>9s}, {quoted_og_number:>13s}, {number:3d} }}, /* {i+1:4d} */')
        assert uni_number == i + 1

    contents.append("};")

    for line in contents:
        print(line)
