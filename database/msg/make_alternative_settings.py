from __future__ import annotations

import sys
import os
import subprocess
from tempfile import TemporaryDirectory
from string import Template
import json
from itertools import product

from joblib import Parallel, delayed
import numpy as np
from numpy.typing import NDArray
import click

from operation import MagneticOperation
from make_msgtype_db import get_type_of_msg
from magnetic_hall import MagneticHallSymbol
from transform import Transformation, get_standard_hall_number
from load import get_msg_numbers, get_spg_table, get_msg_table

sys.path.append('..')
from hall2operations import encode_symmetry


MAX_ENCODED = (3 ** 9) * (12 ** 3)


TYPE3_SEARCH = """LoadPackage( "cryst" );;
SetCrystGroupDefaultAction(LeftAction);;

tx := [ [1, 0, 0, 1], [0, 1, 0, 0], [0, 0, 1, 0], [0, 0, 0, 1], ];;
ty := [ [1, 0, 0, 0], [0, 1, 0, 1], [0, 0, 1, 0], [0, 0, 0, 1], ];;
tz := [ [1, 0, 0, 0], [0, 1, 0, 0], [0, 0, 1, 1], [0, 0, 0, 1], ];;

F := AffineCrystGroup([
    tx, ty, tz,
    $fsg_str
]);;
D1 := AffineCrystGroup([
    tx, ty, tz,
    $xsg_str
]);;

AF_F := AffineNormalizer(F);;
T_gens := [];;
identity := IdentityMat(3);;
for g in GeneratorsOfGroup(AF_F) do
    linear := g{[1..3]}{[1..3]};;
    if linear <> identity then
        continue;
    fi;
    Add(T_gens, g);
od;
T_AF_F := AffineCrystGroup(T_gens);;
coset := RightCosetsNC(AF_F, T_AF_F);;

gens := [];;
found := [];;
for c in coset do
    r := Representative(c);;
    if Determinant(r) <> 1 then
        continue;
    fi;
    D2 := D1^r;;
    if D2 in found then
        continue;
    fi;
    Add(gens, r);
    Add(found, D2);

    rinv := r^-1;;
    if not rinv in gens then
        Add(gens, rinv);
    fi;
od;

for g in gens do
    Print(g, "\\n");
od;

quit;
"""

def enumerate_linears(max_element=1):
    # Enumerate interger matrix with determinant=1
    linears = []
    for nums in product(range(-max_element, max_element + 1), repeat=9):
        arr = np.array(nums).reshape(3, 3)
        if np.isclose(np.linalg.det(arr), 1):
            linears.append(arr.astype(int))
    return linears


def get_family_space_group(coset: list[MagneticOperation]) -> list[MagneticOperation]:
    fsg = []
    for g in coset:
        fsg.append(MagneticOperation.from_linear_translation_time_reversal(
            linear=g.linear,
            translation=g.translation,
            time_reversal=False,
        ))
    return fsg


def get_maximal_space_subgroup(coset: list[MagneticOperation]) -> list[MagneticOperation]:
    xsg = []
    for g in coset:
        if g.time_reversal:
            continue
        xsg.append(MagneticOperation.from_linear_translation_time_reversal(
            linear=g.linear,
            translation=g.translation,
            time_reversal=False,
        ))
    return xsg


def dump_operation(g: MagneticOperation) -> str:
    if g.time_reversal:
        raise ValueError("Only accept ordinary operation.")

    ret = "[ "
    for i in range(3):
        ret += f"[{g._linear_tuple[i][0]}, {g._linear_tuple[i][1]}, {g._linear_tuple[i][2]}, {g._translation_tuple[i]}], "
    ret += "[0, 0, 0, 1] ]"

    return ret


def get_conjugator_type3(gap_path, coset: list[MagneticOperation]) -> list[NDArray]:
    fsg = get_family_space_group(coset)
    xsg = get_maximal_space_subgroup(coset)

    fsg_str = ",\n    ".join(map(dump_operation, fsg))
    xsg_str = ",\n    ".join(map(dump_operation, xsg))
    with TemporaryDirectory() as tempdir:
        fin_path = os.path.join(tempdir, "input.g")

        t = Template(TYPE3_SEARCH)
        fin = t.substitute(fsg_str=fsg_str, xsg_str=xsg_str)

        with open(fin_path, 'w') as f:
            f.write(fin)

        # Ref: https://stackoverflow.com/questions/66808682/how-do-you-use-gap-in-batch-mode
        result = subprocess.run([
            gap_path,
            "-A",
            "--banner",
            "--quiet",
            "--quitonbreak",
        ], input=fin, text=True, capture_output=True)

    # Remove unwanted output like "> > > > > > > > > > > > > > > > > > >"
    out = result.stdout.replace(" ", '').replace('>', '').split('\n')

    ret = []
    for e in out:
        if e == "":
            continue
        arr = np.array(eval(e))

        # Skip pure origin shift or identity
        if np.allclose(arr[:-1, :-1], np.eye(3)):
            continue

        ret.append(arr.tolist())

    return ret


def get_conjugator_type4(coset: list[MagneticOperation], linears) -> list[NDArray]:
    """
    Return conjugators with no origin shift s.t.
        (P, 0)^-1 * coset * (P, 0)
    is conjugate with `coset`.
    """
    xsg = set(get_maximal_space_subgroup(coset))

    found = [set(coset)]
    conjugators = []
    for linear in linears:
        t = Transformation(linear=linear)
        coset2 = t.transform_coset(coset)
        xsg2 = set(get_maximal_space_subgroup(coset2))
        if xsg2 != xsg:
            continue
        coset2 = set(coset2)
        if coset2 in found:
            continue
        found.append(coset2)
        conjugators.append(t._augmented_matrix.tolist())

    return conjugators


def get_conjugator(hall_number, spg_table, msg_table, mapping, linears, gap_path):
    data = {}
    number = spg_table[hall_number]['number']
    choice = spg_table[hall_number]['choice']
    hall_number_std = get_standard_hall_number(number)
    # Transformation from ITA standard settings to the other settings
    trns = Transformation.to_standard(hall_number, choice, number).inverse()

    for bns_number, mhall_symbol in msg_table[hall_number_std].items():
        mhs = MagneticHallSymbol(mhall_symbol)
        coset = trns.transform_coset(mhs.coset)
        msg_type = get_type_of_msg(mhall_symbol)
        uni_number = mapping[bns_number]

        if msg_type == 3 and number >= 16:
            # Triclinic and monoclinic seem to have no alternative setting for type-III
            ret = get_conjugator_type3(gap_path, coset)
        elif msg_type == 4:
            ret = get_conjugator_type4(coset, linears)
        else:
            ret = []

        data[(hall_number, uni_number)] = ret
        # print(f"hall_number={hall_number}, uni_number={uni_number} ({msg_type}): {ret}")

    return data


def dump_cpp(raw_all_datum):
    all_datum = {}
    for data in raw_all_datum:
        conjugators = data['conjugators']
        all_datum[(data['hall_number'], data['uni_number'])] = conjugators

        # Check if symmetry encoding can be used
        for g in conjugators:
            assert np.max(np.abs(g)) <= 1

    mapping = {uni_number: [] for uni_number in range(1, 1651 + 1)}
    for (hall_number, uni_number), conjugators in all_datum.items():
        # sorted by uni_number->hall_number
        mapping[uni_number].append((hall_number, conjugators))

    MAX_HALL_NUMBERS = 18
    MAX_NUM_SETTINGS = 6

    contents = []
    contents.append(f"static const int alternative_transformations[][{MAX_HALL_NUMBERS}][{MAX_NUM_SETTINGS + 1}] = {{")
    contents.append("    { { 0 } }, /* dummy */")
    for uni_number in range(1, 1651 + 1):
        hall_conjugators = mapping[uni_number]
        assert len(hall_conjugators) <= MAX_HALL_NUMBERS
        contents.append("    {")
        for hall_number, conjugators in hall_conjugators:
            line = "        { "
            for conj in conjugators:
                conj = np.array(conj)
                linear = np.around(conj[:-1, :-1]).astype(int)
                assert np.isclose(np.linalg.det(linear), 1)  # Sanity check
                origin_shift = np.remainder(conj[:-1, -1], 1)
                encoded = encode_symmetry(linear, origin_shift)
                assert 0 <= encoded < MAX_ENCODED
                line += f"{encoded}, "
            line += f"0 }}, /* UNI={uni_number}, Hall={hall_number} */"
            contents.append(line)
        contents.append("    },")

    contents.append("};")

    for line in contents:
        print(line)


@click.command()
@click.option('--output', default="conjugators.json")
@click.option('--gap_path', default="./gap-4.11.1/bin/gap.sh")
@click.option('--dump', is_flag=True, default=False)
def main(output, gap_path, dump):
    if dump:
        with open(output, 'r') as f:
            all_datum = json.load(f)
        dump_cpp(all_datum)
        return

    msg_table = get_msg_table()
    spg_table = get_spg_table()

    msg_numbers = get_msg_numbers()
    mapping = {}
    for _, bns_number, _, uni_number in msg_numbers:
        mapping[bns_number] = uni_number

    linears = enumerate_linears()

    ret = Parallel(n_jobs=-1, verbose=11)(delayed(get_conjugator)(hall_number, spg_table, msg_table, mapping, linears, gap_path) for hall_number in range(1, 530 + 1))

    all_datum = []
    for r in ret:
        for (hall_number, uni_number), val in r.items():
            all_datum.append({
                'hall_number': hall_number,
                'uni_number': uni_number,
                'conjugators': val,
            })

    with open(output, 'w') as f:
        json.dump(all_datum, f, indent=2)

    return all_datum


def debug():
    # Testing
    hall_symbol ="P 2ac 2ab'"
    mhs = MagneticHallSymbol(hall_symbol)
    ret = get_conjugator_type3("./gap-4.11.1/bin/gap.sh", mhs.coset)
    print(ret)

    # hall_symbol = "P 2ac 2ab 1c'"
    # mhs = MagneticHallSymbol(hall_symbol)
    # linears = enumerate_linears()
    # ret = get_conjugator_type4(mhs.coset, linears)


if __name__ == '__main__':
    # debug()
    main()
