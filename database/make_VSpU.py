#!/usr/bin/sage

import sys
import numpy as np
from sage.all import *

def get_VSpU( M ):
    N = copy(M)
    for i in range(N.nrows()):
        N[i,i%3] -= 1
    S,U,V = N.smith_form()

    Sp = Matrix( QQ, S.transpose() )
    for i in range(3):
        if not Sp[i,i]==0:
            Sp[i,i] = 1.0 / Sp[i,i]

    return V*Sp*U

def get_VSpU_sets(g1s, g2s):
    VSpU_sets = []
    generator_sets = []
    
    for g3 in ( False, True ): # for inversion
        for g1 in g1s:
            for g2 in g2s:
                if g2 is not False:
                    if np.equal(g1, g2).all():
                        continue
                    if np.equal(g1, np.dot(inv, g2)).all():
                        continue
                    genes = [g1, g2]
                else:
                    genes = [g1,]

                if g3:
                    genes_new = []
                    for g in genes:
                        if np.linalg.det(g) < 0:
                            genes_new.append(np.dot(inv, g))
                        else:
                            genes_new.append(g)
                    genes_new.append(inv)

                    if np.linalg.det(g1) < 0:
                        is_found = False
                        for g1_comp in g1s:
                            if np.equal(genes_new[0], g1_comp).all():
                                is_found = True
                                break
                        if is_found:
                            continue

                    if g2 is not False:
                        if np.linalg.det(g2) < 0:
                            is_found = False
                            for g2_comp in g2s:
                                if np.equal(genes_new[1], g2_comp).all():
                                    is_found = True
                                    break
                            if is_found:
                                continue

                else:
                    genes_new = genes

                M = Matrix(3, 3, genes_new[0])
                if len(genes_new) > 1:
                    for g in genes_new[1:]:
                        M = M.stack(Matrix(3, 3, g))
                VSpU_sets.append(get_VSpU(M))
                generator_sets.append( M )
    return VSpU_sets, generator_sets

def get_rotation_primitive( g1s, g2s, T ):
    if T is not None:
        g1s_new = []
        g2s_new = []
        g1s_old = []
        g2s_old = []
        for g in g1s:
            # print g, "-->"
            try:
                M = Matrix(ZZ,T*Matrix(QQ, g)*T.inverse())
                # print M
                g1s_new.append(np.array(M))
                g1s_old.append(g)
            except TypeError:
                print "Not integer matrix, pass this matrix"
        for g in g2s:
            if g:
                # print g, "-->"
                try:
                    M = Matrix(ZZ, T*Matrix(QQ, g)*T.inverse())
                    # print M
                    g2s_new.append(np.array(M))
                    g2s_old.append(g)
                except TypeError:
                    print "Not integer matrix, pass this matrix"
            else:
                g2s_new.append(False)
                g2s_old.append(False)
    return g1s_new, g2s_new, g1s_old, g2s_old

def write_generators(generator_sets):
    print "{"
    for count, M in enumerate( generator_sets ):
        print "  { /* %d */" % (count+1)
        for i in range( 3 ):
            print "    { ",
            for j in range( 3 ):
                for k in range( 3 ):
                    if M.nrows() // 3 > i:
                        print "%d," % M[i*3+j,k],
                    else:
                        print " 0,",
            print "},"
        print "  },"
    print "};"
      
def write_VSpU(VSpU_sets):
    print "{"
    for count, VSpU in enumerate( VSpU_sets ):
        print "  { /* %d */" % (count+1)
        for colvals in VSpU:
            print "    {",
            for i in range( 9 ):
                if i < len( colvals ):
                    num = colvals[i].numerator()
                    den = colvals[i].denominator()
                    if den == 1:
                        print "%d," % ( num ),
                    else:
                        print "%d.0/%d," % ( num, den ),
                else:
                    print " 0,",
            print "},"
        print "  },"
    print "};"


identity = [[ 1, 0, 0],
            [ 0, 1, 0],
            [ 0, 0, 1]]

inv = [[-1, 0, 0],
       [ 0,-1, 0],
       [ 0, 0,-1]]

# Tetragonal
rot4z = [[ 0,-1, 0],
         [ 1, 0, 0],
         [ 0, 0, 1]]
rot4zi = [[ 0, 1, 0],
          [-1, 0, 0],
          [ 0, 0,-1]]
rot2z = [[-1, 0, 0],
         [ 0,-1, 0],
         [ 0, 0, 1]]
rot2zi = [[ 1, 0, 0],
          [ 0, 1, 0],
          [ 0, 0,-1]]
rot2x = [[ 1, 0, 0],
         [ 0,-1, 0],
         [ 0, 0,-1]]
rot2xi = [[-1, 0, 0],
          [ 0, 1, 0],
          [ 0, 0, 1]]
rot3xyz = [[ 0, 0, 1],
           [ 1, 0, 0],
           [ 0, 1, 0]]
rot3xyzi = [[ 0, 0,-1],
            [-1, 0, 0],
            [ 0,-1, 0]]
hexa2_ab = [[ 0,-1, 0],
            [-1, 0, 0],
            [ 0, 0,-1]]
hexa2_abi = [[ 0, 1, 0],
             [ 1, 0, 0],
             [ 0, 0, 1]]
rot6z = [[ 1,-1, 0],
         [ 1, 0, 0],
         [ 0, 0, 1]]
rot6zi = [[-1, 1, 0],
          [-1, 0, 0],
          [ 0, 0,-1]]
trigo2ab = [[ 0, 1, 0],
            [ 1, 0, 0],
            [ 0, 0,-1]]
trigo2abi = [[ 0,-1, 0],
             [-1, 0, 0],
             [ 0, 0, 1]]
rot3z = [[ 0,-1, 0],
         [ 1,-1, 0],
         [ 0, 0, 1]]
rot3zi = [[ 0, 1, 0],
          [-1, 1, 0],
          [ 0, 0,-1]]
rot2y = [[-1, 0, 0],
         [ 0, 1, 0],
         [ 0, 0,-1]]
rot2yi = [[ 1, 0, 0],
          [ 0,-1, 0],
          [ 0, 0, 1]]



BCC = Matrix(QQ, [[ 0, 1, 1],
                  [ 1, 0, 1],
                  [ 1, 1, 0]])
FCC = Matrix(QQ, [[-1, 1, 1],
                  [ 1,-1, 1],
                  [ 1, 1,-1]])
CC = Matrix(QQ, [[ 1,-1, 0],
                 [ 1, 1, 0],
                 [ 0, 0, 1]])
BC = Matrix(QQ, [[ 1, 0, 1],
                 [ 0, 1, 0],
                 [-1, 0, 1]])
AC = Matrix(QQ, [[ 1, 0, 0],
                 [ 0, 1, 1],
                 [ 0,-1, 1]])
RC = Matrix(QQ, [[ 1, 0, 1],
                 [-1, 1, 1],
                 [ 0,-1, 1]])


# Parse options
from optparse import OptionParser
parser = OptionParser()
parser.set_defaults(is_tricli=False,
                    is_monocli=False,
                    is_ortho=False,
                    is_tetra=False,
                    is_rhombo=False,
                    is_trigo=False,
                    is_hexa=False,
                    is_cubic=False,
                    is_bcc=False,
                    is_fcc=False,
                    is_ac=False,
                    is_bc=False,
                    is_cc=False,
                    is_rc=False)
parser.add_option("--tricli", dest="is_tricli",
                  action="store_true")
parser.add_option("--monocli", dest="is_monocli",
                  action="store_true")
parser.add_option("--ortho", dest="is_ortho",
                  action="store_true")
parser.add_option("--tetra", dest="is_tetra",
                  action="store_true")
parser.add_option("--rhombo", dest="is_rhombo",
                  action="store_true")
parser.add_option("--trigo", dest="is_trigo",
                  action="store_true")
parser.add_option("--hexa", dest="is_hexa",
                  action="store_true")
parser.add_option("--cubic", dest="is_cubic",
                  action="store_true")
parser.add_option("--bcc", dest="is_bcc",
                  action="store_true")
parser.add_option("--fcc", dest="is_fcc",
                  action="store_true")
parser.add_option("--ac", dest="is_ac",
                  action="store_true")
parser.add_option("--bc", dest="is_bc",
                  action="store_true")
parser.add_option("--cc", dest="is_cc",
                  action="store_true")
parser.add_option("--rc", dest="is_rc",
                  action="store_true")
parser.add_option("-g", dest="is_generator",
                  action="store_true")
(options, args) = parser.parse_args()

g1s = None
g2s = None
g1s_old = None
g2s_old = None

if options.is_tricli:
    g1s = ( identity, )
    g2s = ( False, )

if options.is_monocli:
    g1s = ( rot2x, rot2y, rot2z, rot2xi, rot2yi, rot2zi )
    g2s = ( False, )
    if options.is_bcc:
        g1s, g2s, g1s_old, g2s_old = get_rotation_primitive( g1s, g2s, BCC )
    if options.is_ac:
        g1s, g2s, g1s_old, g2s_old = get_rotation_primitive( g1s, g2s, AC )
    if options.is_bc:
        g1s, g2s, g1s_old, g2s_old = get_rotation_primitive( g1s, g2s, BC )
    if options.is_cc:
        g1s, g2s, g1s_old, g2s_old = get_rotation_primitive( g1s, g2s, CC )

if options.is_ortho:
    g1s = ( rot2z, rot2zi )
    g2s = ( rot2x, rot2xi )
    if options.is_bcc:
        g1s, g2s, g1s_old, g2s_old = get_rotation_primitive( g1s, g2s, BCC )
    if options.is_fcc:
        g1s, g2s, g1s_old, g2s_old = get_rotation_primitive( g1s, g2s, FCC )
    if options.is_ac:
        g1s, g2s, g1s_old, g2s_old = get_rotation_primitive( g1s, g2s, AC )
    if options.is_bc:
        g1s, g2s, g1s_old, g2s_old = get_rotation_primitive( g1s, g2s, BC )
    if options.is_cc:
        g1s, g2s, g1s_old, g2s_old = get_rotation_primitive( g1s, g2s, CC )

if options.is_tetra:
    g1s = ( rot4z, rot4zi )
    g2s = ( False, rot2x, rot2xi )
    if options.is_bcc:
        g1s, g2s, g1s_old, g2s_old = get_rotation_primitive( g1s, g2s, BCC )

# if options.is_rhombo:
#     g1s = ( rot3xyz, rot3xyzi )
#     g2s = ( False, hexa2_ab, trigo2ab )

if options.is_trigo:
    g1s = ( rot3z, rot3zi )
    g2s = ( False, hexa2_ab, trigo2ab, hexa2_abi, trigo2abi )
    if options.is_rc: # hP
        g1s, g2s, g1s_old, g2s_old = get_rotation_primitive( g1s, g2s, RC )

if options.is_rhombo: # hR
    g1s = ( rot3z, rot3zi )
    g2s = ( False, hexa2_ab, trigo2ab, hexa2_abi, trigo2abi )
    g1s, g2s, g1s_old, g2s_old = get_rotation_primitive( g1s, g2s, RC )
    g1s_old = None
    g2s_old = None
        
if options.is_hexa:
    g1s = ( rot6z, rot6zi )
    g2s = ( False, hexa2_ab, hexa2_abi )

if options.is_cubic:
    g1s = ( rot4z, rot2z, rot4zi, rot2zi )
    g2s = ( rot3xyz, rot3xyzi )
    if options.is_bcc:
        g1s, g2s, g1s_old, g2s_old = get_rotation_primitive( g1s, g2s, BCC )
    if options.is_fcc:
        g1s, g2s, g1s_old, g2s_old = get_rotation_primitive( g1s, g2s, FCC )

if g1s is None:
    print "Option is needed. See \'make_VSpU.py -h\'"
else:
    if g1s_old is None:
        VSpU_sets, generator_sets = get_VSpU_sets( g1s, g2s )
    else:
        VSpU_sets = get_VSpU_sets( g1s, g2s )[0]
        generator_sets = get_VSpU_sets( g1s_old, g2s_old )[1]

    centering = ""
    if options.is_fcc:
        centering = "_F"
    if options.is_bcc:
        centering = "_I"
    if options.is_ac:
        centering = "_A"
    if options.is_bc:
        centering = "_B"
    if options.is_cc:
        centering = "_C"

    if options.is_generator:
        if options.is_tricli:
            print "static int tricli_generators[][3][9] ="
        if options.is_monocli:
            print "static int monocli_generators[][3][9] ="
        if options.is_ortho:
            print "static int ortho_generators[][3][9] ="
        if options.is_tetra:
            print "static int tetra_generators[][3][9] ="
        if options.is_trigo:
            if options.is_rc:
                print "static int rhombo_h_generators[][3][9] ="
            else:
                print "static int trigo_generators[][3][9] ="
        if options.is_rhombo:
            print "static int rhombo_p_generators[][3][9] ="
        if options.is_hexa:
            print "static int hexa_generators[][3][9] ="
        if options.is_cubic:
            print "static int cubic_generators[][3][9] ="
        write_generators(generator_sets)
        print
    else:
        if options.is_tricli:
            print "static double tricli_VSpU[][3][9] ="
        if options.is_monocli:
            print "static double monocli%s_VSpU[][3][9] =" % centering
        if options.is_ortho:
            print "static double ortho%s_VSpU[][3][9] =" % centering
        if options.is_tetra:
            print "static double tetra%s_VSpU[][3][9] =" % centering
        if options.is_trigo:
            if options.is_rc:
                print "static double rhombo_h_VSpU[][3][9] ="
            else:
                print "static double trigo_VSpU[][3][9] ="
        if options.is_rhombo:
                print "static double rhombo_p_VSpU[][3][9] ="
        if options.is_hexa:
                print "static double hexa_VSpU[][3][9] ="
        if options.is_cubic:
                print "static double cubic%s_VSpU[][3][9] ="
        write_VSpU(VSpU_sets)
        print
