#!/usr/local/sage-4.5.3/sage

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

def get_VSpU_sets( g1s, g2s ):
    VSpU_sets = []
    generator_sets = []
    for g1 in g1s:
        for g2 in g2s:
            if (not g1) and (not g2):
                continue
            for g3 in ( False, inv ):
                if g1:
                    M = Matrix(3, 3, g1)
                    for g in ( g2, g3 ):
                        if g:
                            M = M.stack( Matrix(3, 3, g) )
                else:
                    M = Matrix(3, 3, g2)
                    if g3:
                        M = M.stack( Matrix(3, 3, g3) )
                VSpU_sets.append( get_VSpU( M ) )
                generator_sets.append( M )
    return VSpU_sets, generator_sets

def get_rotation_primitive( g1s, g2s, T ):
    if not T==None:
        g1s_new = []
        g2s_new = []
        for g in g1s:
            g1s_new.append(Matrix(ZZ,T*Matrix(QQ, g)*T.inverse()).list())
        for g in g2s:
            if g:
                g2s_new.append(Matrix(ZZ, T*Matrix(QQ, g)*T.inverse()).list())
            else:
                g2s_new.append( False )
    return g1s_new, g2s_new
      
def write_VSpU_for_c( VSpU_sets, generator_sets ):
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

I = [[ 1, 0, 0],
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
CC = Matrix(QQ, [[ 1, 1, 0],
                 [-1, 1, 0],
                 [ 0, 0, 1]])
BC = Matrix(QQ, [[ 1, 0, 1],
                 [ 0, 1, 0],
                 [-1, 0, 1]])
AC = Matrix(QQ, [[ 1, 0, 0],
                 [ 0, 1, 1],
                 [ 0,-1, 1]])

# Parse options
from optparse import OptionParser
parser = OptionParser()
parser.set_defaults( is_tricli=False,
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
                     is_cc=False )
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
(options, args) = parser.parse_args()

g1s=None
g2s=None

if options.is_tricli:
    g1s = ( I, )
    g2s = ( False, )

if options.is_monocli:
    g1s = ( rot2x, rot2y, rot2z, rot2xi, rot2yi, rot2yi )
    g2s = ( False, )
    if options.is_bcc:
        g1s, g2s = get_rotation_primitive( g1s, g2s, BCC )
    if options.is_ac:
        g1s, g2s = get_rotation_primitive( g1s, g2s, AC )
    if options.is_bc:
        g1s, g2s = get_rotation_primitive( g1s, g2s, BC )
    if options.is_cc:
        g1s, g2s = get_rotation_primitive( g1s, g2s, CC )

if options.is_ortho:
    g1s = ( rot2z, rot2zi )
    g2s = ( False, rot2x, rot2xi )
    if options.is_bcc:
        g1s, g2s = get_rotation_primitive( g1s, g2s, BCC )
    if options.is_fcc:
        g1s, g2s = get_rotation_primitive( g1s, g2s, FCC )
    if options.is_ac:
        g1s, g2s = get_rotation_primitive( g1s, g2s, AC )
    if options.is_bc:
        g1s, g2s = get_rotation_primitive( g1s, g2s, BC )
    if options.is_cc:
        g1s, g2s = get_rotation_primitive( g1s, g2s, CC )

if options.is_tetra:
    g1s = ( rot4z, rot4zi )
    g2s = ( False, rot2x, rot2xi )
    if options.is_bcc:
        g1s, g2s = get_rotation_primitive( g1s, g2s, BCC )

if options.is_rhombo:
    g1s = ( rot3xyz, rot3xyzi )
    g2s = ( False, hexa2_ab, hexa2_abi, trigo2ab, trigo2ab )

if options.is_trigo:
    g1s = ( rot3z, rot3zi )
    g2s = ( False, hexa2_ab, hexa2_abi, trigo2ab, trigo2ab )

if options.is_hexa:
    g1s = ( rot6z, rot6zi )
    g2s = ( False, hexa2_ab, hexa2_abi )

if options.is_cubic:
    g1s = ( rot4z, rot2z, rot4zi, rot2zi )
    g2s = ( False, rot3xyz, rot3xyzi )
    if options.is_bcc:
        g1s, g2s = get_rotation_primitive( g1s, g2s, BCC )
    if options.is_fcc:
        g1s, g2s = get_rotation_primitive( g1s, g2s, FCC )

if g1s==None:
    print "Option is needed. See \'make_VSpU.py -h\'"
else:
    VSpU_sets, generator_sets = get_VSpU_sets( g1s, g2s )
    write_VSpU_for_c( VSpU_sets, generator_sets )
