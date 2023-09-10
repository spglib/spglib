#!/usr/bin/env python

# This will (hopefully) be the code to extract symmetry operations
# from Hall symbols

import numpy as np

lattice_symbols = {
    'P': [[0, 0, 0]],
    'A': [[0, 0, 0], [0, 1./2, 1./2]],
    'B': [[0, 0, 0], [1./2, 0, 1./2]],
    'C': [[0, 0, 0], [1./2, 1./2, 0]],
    'I': [[0, 0, 0], [1./2, 1./2, 1./2]],
    'R': [[0, 0, 0], [2./3, 1./3, 1./3], [1./3, 2./3, 2./3]],
    'H': [[0, 0, 0], [2./3, 1./3, 0], [1./3, 2./3, 0]],
    'F': [[0, 0, 0], [0, 1./2, 1./2], [1./2, 0, 1./2], [1./2, 1./2, 0]],
    'p': [[0, 0, 0]],
    'c': [[0, 0, 0], [1./2, 1./2, 0]]
}

rotation_matrices = {
    '1x': [[1, 0, 0],
           [0, 1, 0],
           [0, 0, 1]],
    '1y': [[1, 0, 0],
           [0, 1, 0],
           [0, 0, 1]],
    '1z': [[1, 0, 0],
           [0, 1, 0],
           [0, 0, 1]],
    '2x': [[1, 0, 0],
           [0, -1, 0],
           [0, 0, -1]],
    '2y': [[-1, 0, 0],
           [0, 1, 0],
           [0, 0, -1]],
    '2z': [[-1, 0, 0],
           [0, -1, 0],
           [0, 0, 1]],
    '3x': [[1, 0, 0],
           [0, 0, -1],
           [0, 1, -1]],
    '3y': [[-1, 0, 1],
           [0, 1, 0],
           [-1, 0, 0]],
    '3z': [[0, -1, 0],
           [1, -1, 0],
           [0, 0, 1]],
    '4x': [[1, 0, 0],
           [0, 0, -1],
           [0, 1, 0]],
    '4y': [[0, 0, 1],
           [0, 1, 0],
           [-1, 0, 0]],
    '4z': [[0, -1, 0],
           [1, 0, 0],
           [0, 0, 1]],
    '6x': [[1, 0, 0],
           [0, 1, -1],
           [0, 1, 0]],
    '6y': [[0, 0, 1],
           [0, 1, 0],
           [-1, 0, 1]],
    '6z': [[1, -1, 0],
           [1, 0, 0],
           [0, 0, 1]],
    '2px': [[-1, 0, 0],     # b-c
            [0, 0, -1],
            [0, -1, 0]],
    '2ppx': [[-1, 0, 0],    # b+c
             [0, 0, 1],
             [0, 1, 0]],
    '2py': [[0, 0, -1],     # a-c
            [0, -1, 0],
            [-1, 0, 0]],
    '2ppy': [[0, 0, 1],    # a+c
             [0, -1, 0],
             [1, 0, 0]],
    '2pz': [[0, -1, 0],     # a-b
            [-1, 0, 0],
            [0, 0, -1]],
    '2ppz': [[0, 1, 0],    # a+b
             [1, 0, 0],
             [0, 0, -1]],
    '3*': [[0, 0, 1],     # a+b+c
           [1, 0, 0],
           [0, 1, 0]]
}

translations = {
    'a': [1./2, 0, 0],
    'b': [0, 1./2, 0],
    'c': [0, 0, 1./2],
    'n': [1./2, 1./2, 1./2],
    'u': [1./4, 0, 0],
    'v': [0, 1./4, 0],
    'w': [0, 0, 1./4],
    'd': [1./4, 1./4, 1./4]
}


def read_spg_csv(filename="spg.csv"):
    hall_symbols = []
    for line in open(filename):
        data = line.split(',')
        hall_symbols.append([data[6], data[4]])
    return hall_symbols


class HallSymbol:
    def __init__(self, hall_symbol):
        self.hall_symbol = hall_symbol.split()
        self._decompose()
        self._full_operations()

    def get_LNV(self):
        return self.L, self.N, self.V

    def get_operations(self):
        return self.G_R, self.G_T

    def _full_operations(self):
        gens_R, gens_T = self._generators()

        E = np.array(rotation_matrices['1x'])
        T0 = np.zeros(3, dtype=float)
        if self.L[0] == '-':
            G_R = [E, -E]
            G_T = [T0, T0]
        else:
            G_R = [E]
            G_T = [T0]

        for r, t in zip(gens_R, gens_T):
            G2_R, G2_T = self._get_group(r, t)
            G_R, G_T = self._multiply_groups(G_R, G_T, G2_R, G2_T)

        if self.V is not None:
            G_T = self._change_of_basis(G_R, G_T)

        G_R_with_centres = []
        G_T_with_centred = []
        for t in lattice_symbols[self.L[-1]]:
            self._lattice_translation(G_R_with_centres,
                                      G_T_with_centred,
                                      G_R, G_T, t)

        self.G_R = np.array(G_R_with_centres)
        self.G_T = np.array(G_T_with_centred)

        # Make sure the first operation has no rotation.
        assert (self.G_R[0] == rotation_matrices['1x']).all()

        # In Hall numbers 212, 213, 214, the first operation has non-zero
        # translation. This translation is subtracted from all operations.
        self.G_T -= self.G_T[0]
        self.G_T -= np.rint(self.G_T)
        cond = self.G_T < -1e-3
        self.G_T[cond] += 1

    def _change_of_basis(self, G_R, G_T):
        G_T_new = []
        v = self.V.astype(float) / 12
        for r, t in zip(G_R, G_T):
            G_T_new.append(-np.dot(r, v) + t + v)
        return G_T_new

    def _lattice_translation(self, G_R, G_T, G_R0, G_T0, translation):
        for r, t in zip(G_R0, G_T0):
            G_R.append(r.copy())
            t_new = t + translation
            G_T.append(t_new)

    def _multiply_groups(self, G1_R, G1_T, G2_R, G2_T):  # G2xG1
        G_R = []
        G_T = []
        for r1, t1 in zip(G2_R, G2_T):
            for r2, t2 in zip(G1_R, G1_T):
                G_R.append(np.dot(r1, r2))
                G_T.append(np.dot(r1, t2) + t1)

        return G_R, G_T

    def _get_group(self, r, t):
        G_R = [r, ]
        G_T = [t, ]
        while not (G_R[-1] == rotation_matrices['1x']).all():
            _r = np.dot(G_R[-1], r)
            _t = np.dot(G_R[-1], t) + G_T[-1]
            G_R.append(_r)
            G_T.append(_t)

        # Bring identity in front
        _r = G_R.pop()
        _t = G_T.pop()
        G_R.insert(0, _r)
        G_T.insert(0, _t)

        return G_R, G_T

    # def _get_group(self, r, t):
    #     G_R, G_T = self._get_group_recursive([np.array(r)], [np.array(t)])
    #     r = G_R.pop()
    #     t = G_T.pop()
    #     G_R.insert(0, r)
    #     G_T.insert(0, t)
    #     return G_R, G_T

    # def _get_group_recursive(self, G_R, G_T):
    #     if not (G_R[-1] == rotation_matrices['1x']).all():
    #         r = np.dot(G_R[-1], G_R[0])
    #         t = np.dot(G_R[-1], G_T[0]) + G_T[-1]
    #         G_R.append(r)
    #         G_T.append(t)
    #         self._get_group_recursive(G_R, G_T)
    #     return G_R, G_T

    def _generators(self):
        R = []
        T = []
        for N in self.N:
            rot = np.array(rotation_matrices[N[1] + N[2]])
            if N[0] == '-':
                rot = -rot
            R.append(rot)

            trans = np.zeros(3, dtype=float)
            if N[3] is not None:
                for t in N[3]:
                    if t in ('1', '2', '3', '4', '5'):
                        trans_screw = float(t) / int(N[1])
                        if N[2] == 'x':
                            trans[0] += trans_screw
                        elif N[2] == 'y':
                            trans[1] += trans_screw
                        elif N[2] == 'z':
                            trans[2] += trans_screw
                        else:
                            raise
                    else:
                        trans += np.array(translations[t])
            T.append(trans)

        return np.array(R, dtype=int), np.array(T, dtype=float)

    def _rotation_matrix(self, str):
        pass

    # Decompose Hall symbol
    # The following methods are used by _decompose().
    def _decompose(self):
        L = self.hall_symbol.pop(0)
        N = []
        V = None
        precededN = 0
        for i, ms in enumerate(self.hall_symbol):
            if ms[0] == '(':
                V = self._change_of_basis_symbol(self.hall_symbol[i + 2])
                break
            else:
                N.append(self._matrix_symbol(ms, i, precededN))
                precededN = int(N[-1][1][0])

        self.L = L
        self.N = N
        self.V = V

    def _matrix_symbol(self, N, i, precededN):
        if N[0] == '-':
            improper = '-'
            N = N[1:]
        else:
            improper = None

        N, R, A = self._rotation(N, i, precededN)

        if len(N) > 0:
            T = self._translation(N)
        else:
            T = None

        return [improper, R, A, T]

    def _rotation(self, N, i, precededN):
        A = None
        if N[0] == '2':
            if len(N) > 1:  # 2"
                if N[1] == '=':
                    R = '2pp'
                    A = 'z'
                    N = N[2:]
            if i == 1 and A is None:
                if precededN == 2 or precededN == 4:  # 2x
                    R = '2'
                    A = 'x'
                    N = N[1:]
                elif precededN == 3 or precededN == 6:  # 2'
                    R = '2p'
                    A = 'z'
                    N = N[1:]
        elif N[0] == '3':     # 3*
            if i == 2:
                R = '3'
                A = '*'
                N = N[1:]
            elif len(N) > 1:
                if N[1] == '*':
                    R = '3'
                    A = '*'
                    N = N[2:]

        if A is None:
            R = N[0]
            N = N[1:]
            if len(N) > 0 and i == 0:
                N, A = self._principal_axis(N)
            else:
                A = 'z'

        return N, R, A

    def _principal_axis(self, N):
        if N[0] == 'x':
            return N[1:], 'x'
        if N[0] == 'y':
            return N[1:], 'y'
        return N, 'z'

    def _translation(self, N):
        T = []
        for i in range(len(N)):
            T.append(N[i])
        return T

    def _change_of_basis_symbol(self, V):
        return np.array([0, 0, int(V[:-1])])


def dump_operations(filename):
    hall_symbols = read_spg_csv(filename)
    count = 0
    print("  0       ,  /* dummy */")
    for i in range(len(hall_symbols)):
        hs = HallSymbol(hall_symbols[i][0])
        G_R, G_T = hs.get_operations()
        for j, (r, t) in enumerate(zip(G_R, G_T)):
            count += 1
            total = encode_symmetry(r, t)
            r_enc_dec, t_enc_dec = decode_symmetry(total)
            assert np.allclose(r_enc_dec, r.reshape(-1).tolist())
            assert np.allclose(t_enc_dec, t * 12)

            text = "  %-8d," % (total)
            text += " /* %4d (%3d) [" % (count, i + 1)
            text += "%2d," * 9 % tuple(r_enc_dec)
            text += "%2d,%2d,%2d] */" % tuple(t_enc_dec)
            print(text)


def dump_operations_old(filename):
    hall_symbols = read_spg_csv(filename)
    count = 0
    for i in range(530):
        hs = HallSymbol(hall_symbols[i][0])
        G_R, G_T = hs.get_operations()
        for j, (r, t) in enumerate(zip(G_R, G_T)):
            count += 1
            text = "{%3d," % (i + 1)
            text += "%2d,%2d,%2d,%2d,%2d,%2d,%2d,%2d,%2d," % tuple(r.ravel())
            text += "%2d,%2d,%2d" % tuple((t * 12 + 0.1).astype(int))
            text += "}, /* %4d */" % count
            print(text)


# Ternary numerical system
def encode_symmetry(r, t):
    r_encode = encode_rotation(r)
    t_encode = encode_trans(t)
    total = t_encode * 3 ** 9 + r_encode
    return total


def encode_rotation(r):
    r_sum = 0
    for i, x in enumerate(r.ravel()):
        r_sum += (x + 1) * 3**(8 - i)
    return r_sum


def encode_trans(t):
    x = np.rint(t * 12).astype(int)
    t_encode = x[0] * 144 + x[1] * 12 + x[2]
    return t_encode


def decode_symmetry(c):
    r_encoded = c % (3 ** 9)
    t_encoded = c // (3 ** 9)
    r_aligned = decode_rotation(r_encoded)  # (9, )
    t_multiplied = decode_trans(t_encoded)  # (3, )
    return r_aligned, t_multiplied


def decode_rotation(c):
    r = []
    for i in range(8, -1, -1):
        r.append((c % (3**(i+1))) // (3**i) - 1)
    return np.array(r)


def decode_trans(c):
    return c // 144, (c % 144) // 12, (c % 12)


def get_reference_to_operations(filename):
    hall_symbols = read_spg_csv(filename)
    count = 0
    for i in range(len(hall_symbols)):
        hs = HallSymbol(hall_symbols[i][0])
        G_R, G_T = hs.get_operations()
        print("  {%4d,%5d}, /* %3d */ " % (len(G_R), count + 1, i + 1))
        count += len(G_R)


def watch_hs(filename, number):
    print("  {   0,    0}, /*   0 */")
    num = number - 1
    hall_symbols = read_spg_csv(filename)
    hs = HallSymbol(hall_symbols[num][0])
    for char, vals in zip(('L', 'N', 'V'), hs.get_LNV()):
        print("%s: %s" % (char, vals))
    G_R, G_T = hs.get_operations()
    print(number, ":", hall_symbols[num][0], "(", len(G_R), ")")
    for i, (r, t) in enumerate(zip(G_R, G_T)):
        print("-----", i + 1, "-----")
        print(r, t)


if __name__ == '__main__':
    """

    Usage
    -----

    To watch symmetry operations of a Hall symbol,

    % python hall2operations.py --hs spg.csv 213

    To dump symmetry operations of all Hall symbols that are copied to
    spglb_database.c,

    % python hall2operations.py --dump spg.csv

    To dump address of symmetry operation data that is copied to
    spglb_database.c,

    % python hall2operations.py --reference spg.csv

    """

    from optparse import OptionParser
    parser = OptionParser()
    parser.set_defaults(watch_hs=False,
                        dump_operations=False,
                        shift=None,
                        origin=None)
    parser.add_option("--hs", dest="watch_hs",
                      action="store_true",
                      help="spg.csv [spg NUM]")
    parser.add_option("--dump", dest="is_dump",
                      action="store_true")
    parser.add_option("--reference", dest="is_reference",
                      action="store_true")

    (options, args) = parser.parse_args()

    if options.is_dump:
        dump_operations(args[0])
    if options.is_reference:
        get_reference_to_operations(args[0])
    elif options.watch_hs:
        watch_hs(args[0], int(args[1]))
