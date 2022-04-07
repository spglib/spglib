#!/usr/bin/env python

import sys
import numpy as np

lattice_symbols = {
    'P': [[0, 0, 0]],
    'A': [[0, 0, 0], [0, 12, 12]],
    'B': [[0, 0, 0], [12, 0, 12]],
    'C': [[0, 0, 0], [12, 12, 0]],
    'I': [[0, 0, 0], [12, 12, 12]],
    'R': [[0, 0, 0]],
    'H': [[0, 0, 0], [16, 8, 8], [8, 16, 16]],
    'F': [[0, 0, 0], [0, 12, 12], [12, 0, 12], [12, 12, 0]],
    'p': [[0, 0, 0]],
    'c': [[0, 0, 0], [12, 12, 0]]
}


def position2operation(p):
    r = np.zeros((3, 3), dtype=int)
    xyz = 'xyz'

    for i in range(3):
        for j, c in enumerate(xyz):
            pos = p[i].find(c)
            # if exist xyz
            if pos > -1:
                if pos == 0:
                    r[i, j] = 1
                if pos > 0:
                    if p[i][pos-1] == '+':  # +y, +z
                        r[i, j] = 1
                    elif p[i][pos-1] == '-':  # -x, -y, -z
                        r[i, j] = -1
                    elif p[i][pos-1].isdigit():  # 2x
                        if pos > 1:
                            if p[i][pos-2] == '-':
                                r[i, j] = - int(p[i][pos-1])
                            else:
                                r[i, j] = int(p[i][pos-1])
                        else:
                            r[i, j] = int(p[i][pos-1])
                    else:
                        print("error")
                        sys.exit(1)

                # print p, p[i], r[i,j]

    t = np.zeros(3, dtype=int)

    # translation multiplied by 24
    try:
        table = str.maketrans({'x': '', 'y': '', 'z': '', '+': '', '-': ''})
    except AttributeError:
        pass
    for i in range(3):
        try:
            trans = p[i].translate(None, 'xyz+-')
        except TypeError:
            trans = p[i].translate(table)
        pos = trans.find('/')
        if pos > -1:
            frac = trans.split('/')
            t[i] = int(frac[0]) * (24 / int(frac[1]))

    return r, t


def encode_rotation(r):
    enc = []
    for i in range(3):
        enc.append((r[i][0] + 2) * 9 + (r[i][1] + 1) * 3 + r[i][2] + 1)
    return enc[0] * 45**2 + enc[1] * 45 + enc[2]


def decode_rotation(c):
    r = np.zeros((3, 3), dtype=int)
    dec = [c // 45**2, (c % 45**2) // 45, c % 45]
    for i in range(3):
        r[i][0] = dec[i] // 9 - 2
        r[i][1] = (dec[i] % 9) // 3 - 1
        r[i][2] = dec[i] % 3 - 1
    return r


def decode_trans(c):
    return c // 576, (c % 576) // 24, (c % 24)


def run_test(len_wyckoffs, len_site, o_flat):
    sum_elem = 1
    for i in range(1, 531):
        sum_site = 0
        for j in range(len_wyckoffs[i], len_wyckoffs[i+1]):
            print(len_site[j+1] - len_site[j]),

            sum_site += len_site[j+1] - len_site[j]

            for x in o_flat[len_site[j]:len_site[j+1]]:
                t = decode_trans(x // 3**9)
                r = decode_rotation(x % 3**9)
                print(r, t)

        print("  %d" % sum_site)
        sum_elem += sum_site
    return sum_elem


def damp_array_wyckoffs(len_wyckoff):
    print("static const int position_wyckoff[] =")
    text = "  { %4d," % 0
    for i, x in enumerate(len_wyckoffs[1:]):
        if i % 10 == 0:
            print(text)
            text = "   "
        text += " %4d," % x
    print(text + " };")


def damp_array_numsites(len_site):
    text = "  { %5d," % 0
    for i, x in enumerate(len_site[1:]):
        if i % 10 == 0:
            print(text)
            text = "   "
        text += " %5d," % x
    print(text + " };")


def damp_array_positions(orbits):
    text = "  { %10d," % 0
    for i, x in enumerate(orbits[1:]):
        if i % 5 == 0:
            print(text)
            text = "   "
        text += " %10d," % x
    print(text + " };")


def damp_array_positions_short(orbits, len_site):
    count = 0
    print("static const int coordinates_first[] =")
    text = "  { %9d," % 0
    for i, x in enumerate(len_site[1:-1]):
        if count % 5 == 0:
            print(text)
            text = "   "
        text += " %9d," % orbits[x]
        count += 1
    print(text + " };")

    count = 0
    print("static const int num_sitesym[] =")
    text = "  { %3d," % 0
    for i, x in enumerate(len_site[1:-1]):
        if count % 10 == 0:
            print(text)
            text = "   "
        text += " %3d," % (len_site[i+2] - len_site[i+1])
        count += 1
    print(text + " };")


def damp_array_site_symmetries(site_symmetries):
    print("static const char site_symmetries[][7] =")
    text = "  { %8s," % "\"      \""
    for i, x in enumerate(site_symmetries):
        if i % 6 == 0:
            print(text)
            text = "   "
        text += " %8s," % x
    print(text + " };")


def read_wyckoff_csv(filename):
    with open(filename) as wyckoff_file:
        return parse_wyckoff_csv(wyckoff_file)


def parse_wyckoff_csv(wyckoff_file):
    """Parse Wyckoff.csv

    There are 530 data sets. For one example:

    9:C 1 2 1:::::::
    ::4:c:1:(x,y,z):(-x,y,-z)::
    ::2:b:2:(0,y,1/2):::
    ::2:a:2:(0,y,0):::

    """

    rowdata = []
    points = []
    hP_nums = [433, 436, 444, 450, 452, 458, 460]
    for i, line in enumerate(wyckoff_file):
        if line.strip() == 'end of data':
            break
        rowdata.append(line.strip().split(':'))

        # 2:P -1  ::::::: <-- store line number if first element is number
        if rowdata[-1][0].isdigit():
            points.append(i)
    points.append(i)

    wyckoff = []
    for i in range(len(points) - 1):  # 0 to 529
        symbol = rowdata[points[i]][1]  # e.g. "C 1 2 1"
        if i + 1 in hP_nums:
            symbol = symbol.replace('R', 'H', 1)
        wyckoff.append({'symbol': symbol.strip()})

    # When the number of positions is larger than 4,
    # the positions are written in the next line.
    # So those positions are connected.
    for i in range(len(points) - 1):
        count = 0
        wyckoff[i]['wyckoff'] = []
        for j in range(points[i] + 1, points[i + 1]):
            # Hook if the third element is a number (multiplicity), e.g.,
            #
            # 232:P 2/b 2/m 2/b:::::::  <- ignored
            # ::8:r:1:(x,y,z):(-x,y,-z):(x,-y+1/2,-z):(-x,-y+1/2,z)
            # :::::(-x,-y,-z):(x,-y,z):(-x,y+1/2,z):(x,y+1/2,-z)  <- ignored
            # ::4:q:..m:(x,0,z):(-x,0,-z):(x,1/2,-z):(-x,1/2,z)
            # ::4:p:..2:(0,y,1/2):(0,-y+1/2,1/2):(0,-y,1/2):(0,y+1/2,1/2)
            # ::4:o:..2:(1/2,y,0):(1/2,-y+1/2,0):(1/2,-y,0):(1/2,y+1/2,0)
            # ...
            if rowdata[j][2].isdigit():
                pos = []
                w = {'letter': rowdata[j][3].strip(),
                     'multiplicity': int(rowdata[j][2]),
                     'site_symmetry': rowdata[j][4].strip(),
                     'positions': pos}
                wyckoff[i]['wyckoff'].append(w)

                for k in range(4):
                    if rowdata[j][k + 5]:  # check if '(x,y,z)' or ''
                        count += 1
                        pos.append(rowdata[j][k + 5])
            else:
                for k in range(4):
                    if rowdata[j][k + 5]:
                        count += 1
                        pos.append(rowdata[j][k + 5])

        # assertion
        for w in wyckoff[i]['wyckoff']:
            n_pos = len(w['positions'])
            n_pos *= len(lattice_symbols[wyckoff[i]['symbol'][0]])
            assert n_pos == w['multiplicity']

    return wyckoff


def get_wyckoff_positions(wyckoff):
    positions = []
    for i, w in enumerate(wyckoff):
        # print "%d:%s" % (i+1, w['symbol'])
        positions_hall = []
        for p in w['wyckoff']:
            # print "  %d %s %s" % (p['multiplicity'],
            #                        p['letter'],
            #                        p['site_symmetry']),
            # print
            # for o in p['positions']:
            #     print o,
            # print

            positions_site = []
            for pure_trans in lattice_symbols[w['symbol'][0]]:
                for o in p['positions']:
                    r, t = position2operation(o.strip('()').split(','))
                    t += np.array(pure_trans)
                    t = t % 24
                    positions_site.append((r, t))
            positions_hall.append(positions_site)
        positions.append(positions_hall)
    return positions


def get_site_symmetries(wyckoff):
    """List up site symmetries

    The data structure is as follows:

        wyckoff[0]['wyckoff'][0]['site_symmetry']

    Note
    ----
    Maximum length of string is 6.

    """

    ssyms = []
    for w in wyckoff:
        ssyms += ["\"%-6s\"" % w_s['site_symmetry'] for w_s in w['wyckoff']]

    damp_array_site_symmetries(ssyms)


def encode_wyckoff_positions(positions):
    maxval = 0
    maxmulti = 0
    maxr = None
    maxt = None
    number = 0
    pos_encoded = []
    numerators = np.zeros(24, dtype=int)

    for i, positions_hall in enumerate(positions):
        pos_encoded_hall = []
        for positions_site in positions_hall:
            pos_encoded_site = []
            for r, t in positions_site:
                # for j in range(3):
                #     numerators[t[j]] += 1

                t_encode = t[0] * 576 + t[1] * 24 + t[2]
                r_encode = encode_rotation(r)
                total = t_encode * 45**3 + r_encode

                pos_encoded_site.append(total)

                if (r - decode_rotation(r_encode)).any():
                    print("Error in r %d" % (i+1))
                    sys.exit(1)
                # else:
                #     if (abs(decode_rotation(r_encode)) > 1).any():
                #         print i+1
                #         print decode_rotation(r_encode)
                #         print t
                #         print
                if (t - decode_trans(t_encode)).any():
                    print("Error in t")
                    sys.exit(1)

                if total > maxval:
                    maxval = total
                    maxr = r.copy()
                    maxt = t.copy()
                    number = i+1

            pos_encoded_hall.append(pos_encoded_site)
        pos_encoded.append(pos_encoded_hall)

    # for i, n in enumerate(numerators):
    #     print i, n

    # print "MAX: %d" % maxmulti
    # print "MAX: %d (%d)" % (maxval,number)
    # print maxr
    # print maxt

    return pos_encoded


def get_data_arrays(encoded_positions):
    len_wyckoffs = [0]
    len_site = [0]
    sum_wyckoffs = 1
    sum_site = 1
    encpos_flat = [0]
    for h in encoded_positions:
        len_wyckoffs.append(sum_wyckoffs)
        sum_wyckoffs += len(h)
        for w in h:
            len_site.append(sum_site)
            sum_site += len(w)
            encpos_flat += w

    len_wyckoffs.append(sum_wyckoffs)
    len_site.append(sum_site)

    return len_wyckoffs, len_site, encpos_flat


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser()
    parser.set_defaults(is_site=False,
                        is_position=False,
                        is_wyckoff=False,
                        is_site_short=False,
                        is_site_symbol=False,
                        is_wyckoff_short=False)

    parser.add_argument("--site", dest="is_site",
                        action="store_true",
                        help="Index of site symmetries of each space group")
    parser.add_argument("--site-symbol", dest="is_site_symbol",
                        action="store_true",
                        help="Site symmetry symbols")
    parser.add_argument("--position", dest="is_position",
                        action="store_true",
                        help=("Index of positions of each site symmetries and "
                              "space group"))
    parser.add_argument("--wyckoff", dest="is_wyckoff",
                        action="store_true",
                        help="Encoded Wyckoff position operators")
    parser.add_argument("--wyckoff_short", dest="is_wyckoff_short",
                        action="store_true",
                        help="Encoded Wyckoff position operators")
    parser.add_argument("filename", nargs='?',
                        help="Filename of csv file")
    args = parser.parse_args()
    wyckoff = read_wyckoff_csv(args.filename)
    operations = get_wyckoff_positions(wyckoff)
    encoded_positions = encode_wyckoff_positions(operations)
    len_wyckoffs, len_site, encpos_flat = get_data_arrays(encoded_positions)

    # Test
    # sum_elem = run_test(len_wyckoffs, len_site, o_flat)
    # print "%d = %d" % (len(o_flat), sum_elem)

    # Damp full arrays
    if args.is_position:
        damp_array_wyckoffs(len_wyckoffs)
    if args.is_site:
        damp_array_numsites(len_site)
    if args.is_wyckoff:
        damp_array_positions(encpos_flat)
    if args.is_wyckoff_short:
        damp_array_positions_short(encpos_flat, len_site)
    if args.is_site_symbol:
        get_site_symmetries(wyckoff)
