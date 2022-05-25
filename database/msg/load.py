from pathlib import Path
import csv

from ruamel.yaml import YAML


def get_msg_numbers():
    all_datum = []
    with open(Path(__file__).resolve().parent / "msg_numbers.csv", 'r') as f:
        reader = csv.reader(f, delimiter=',')
        next(reader)  # skip header
        for row in reader:
            if len(row) == 0:
                break

            litvin_number, bns_number, og_number, uni_number = row
            all_datum.append((
                int(litvin_number),
                bns_number,
                og_number,
                int(uni_number),
            ))

    assert len(all_datum) == 1651
    return all_datum


def get_spg_table():
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


def get_msg_table():
    # Load MSG for ITA standard settings
    with open(Path("./magnetic_hall_symbols.yaml"), 'r') as f:
        all_datum = dict(YAML().load(f))
    return all_datum
