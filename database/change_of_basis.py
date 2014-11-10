#!/usr/bin/env python

import numpy as np

identity = [[ 1, 0, 0 ],
            [ 0, 1, 0 ],
            [ 0, 0, 1 ]]
permute_abc = [[ 0, 0, 1 ],
               [ 1, 0, 0 ],
               [ 0, 1, 0 ]]
invert_abc = [[ 0, 0, 1 ],
              [ 0,-1, 0 ],
              [ 1, 0, 0 ]]
monocli_cell_choice = [[-1, 0, 1 ],
					   [ 0, 1, 0 ],
					   [-1, 0, 0 ]]

def print_monocli():
    mat = identity
    transform_mats = []
    centerings = []
    center = 'C'
    for i in range(3):
        for j in range(2):
            for k in range(3):
                transform_mats.append(mat)
                centerings.append(center)
                mat = np.dot(permute_abc, mat)
                if center == 'C':
                    center = 'B'
                elif center == 'B':
                    center = 'A'
                elif center == 'A':
                    center = 'C'
                elif center == 'I':
                    center = 'I'
            mat = np.dot(invert_abc, mat)
            if center == 'C':
                center = 'A'
            elif center == 'A':
                center = 'C'
            elif center == 'B':
                center = 'B'
            elif center == 'I':
                center = 'I'
        mat = np.dot(monocli_cell_choice, mat)
        if center == 'C':
            center = 'A'
        elif center == 'A':
            center = 'I'
        elif center == 'I':
            center = 'C'
        elif center == 'B':
            print "Something wrong"
    
    for mat in np.array(transform_mats):
        print " {{%2d,%2d,%2d }," % tuple(mat[0])
        print "  {%2d,%2d,%2d }," % tuple(mat[1])
        print "  {%2d,%2d,%2d }}," % tuple(mat[2])
    for center in centerings:
        if center == 'C':
            print "C_FACE,"
        if center == 'A':
            print "A_FACE,"
        if center == 'B':
            print "B_FACE,"
        if center == 'I':
            print "BASE,"

def print_ortho():
    mat = identity
    transform_mats = []
    centerings = []
    center = 'C'
    for i in range(2):
        for j in range(3):
            transform_mats.append(mat)
            centerings.append(center)
            mat = np.dot(permute_abc, mat)
            if center == 'C':
                center = 'B'
            elif center == 'B':
                center = 'A'
            elif center == 'A':
                center = 'C'
            elif center == 'I':
                center = 'I'
        mat = np.dot(invert_abc, mat)
        if center == 'C':
            center = 'A'
        elif center == 'A':
            center = 'C'
        elif center == 'B':
            center = 'B'
        elif center == 'I':
            center = 'I'
    
    for mat in np.array(transform_mats):
        print " {{%2d,%2d,%2d }," % tuple(mat[0])
        print "  {%2d,%2d,%2d }," % tuple(mat[1])
        print "  {%2d,%2d,%2d }}," % tuple(mat[2])
    for center in centerings:
        if center == 'C':
            print "C_FACE,"
        if center == 'A':
            print "A_FACE,"
        if center == 'B':
            print "B_FACE,"
        if center == 'I':
            print "BASE,"
                                                 

# print_monocli()
print_ortho()
