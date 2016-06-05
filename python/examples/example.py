#!/usr/bin/evn python

import sys
import spglib
import numpy as np

#######################################
# Uncomment to use Atoms class in ASE #
#######################################
# from ase import Atoms

#######################################################################
# Using the local Atoms-like class (BSD license) where a small set of #
# ASE Atoms features is comaptible but enough for this example.       #
#######################################################################
from atoms import Atoms

def show_symmetry(symmetry):
    for i in range(symmetry['rotations'].shape[0]):
        print("  --------------- %4d ---------------" % (i + 1))
        rot = symmetry['rotations'][i]
        trans = symmetry['translations'][i]
        print("  rotation:")
        for x in rot:
            print("     [%2d %2d %2d]" % (x[0], x[1], x[2]))
        print("  translation:")
        print("     (%8.5f %8.5f %8.5f)" % (trans[0], trans[1], trans[2]))

def show_lattice(lattice):
    print("Basis vectors:")
    for vec, axis in zip(lattice, ("a", "b", "c")):
        print("%s %10.5f %10.5f %10.5f" % (tuple(axis,) + tuple(vec)))

def show_cell(lattice, positions, numbers):
    show_lattice(lattice)
    print("Atomic points:")
    for p, s in zip(positions, numbers):
        print("%2d %10.5f %10.5f %10.5f" % ((s,) + tuple(p)))

silicon_ase = Atoms(symbols=['Si'] * 8,
                    cell=[(4, 0, 0),
                          (0, 4, 0),
                          (0, 0, 4)],
                    scaled_positions=[(0, 0, 0),
                                      (0, 0.5, 0.5),
                                      (0.5, 0, 0.5),
                                      (0.5, 0.5, 0),
                                      (0.25, 0.25, 0.25),
                                      (0.25, 0.75, 0.75),
                                      (0.75, 0.25, 0.75),
                                      (0.75, 0.75, 0.25)],
                    pbc=True)

silicon = ([(4, 0, 0),
            (0, 4, 0),
            (0, 0, 4)],
           [(0, 0, 0),
            (0, 0.5, 0.5),
            (0.5, 0, 0.5),
            (0.5, 0.5, 0),
            (0.25, 0.25, 0.25),
            (0.25, 0.75, 0.75),
            (0.75, 0.25, 0.75),
            (0.75, 0.75, 0.25)],
           [14,] * 8)

silicon_dist = ([(4.01, 0, 0),
                 (0, 4, 0),
                 (0, 0, 3.99)],
                [(0.001, 0, 0),
                 (0, 0.5, 0.5),
                 (0.5, 0, 0.5),
                 (0.5, 0.5, 0),
                 (0.25, 0.25, 0.251),
                 (0.25, 0.75, 0.75),
                 (0.75, 0.25, 0.75),
                 (0.75, 0.75, 0.25)],
                [14,] * 8)

silicon_prim = ([(0, 2, 2),
                 (2, 0, 2),
                 (2, 2, 0)],
                [(0, 0, 0),
                 (0.25, 0.25, 0.25)],
                [14, 14])

rutile = ([(4, 0, 0),
           (0, 4, 0),
           (0, 0, 3)],
          [(0, 0, 0),
           (0.5, 0.5, 0.5),
           (0.3, 0.3, 0.0),
           (0.7, 0.7, 0.0),
           (0.2, 0.8, 0.5),
           (0.8, 0.2, 0.5)],
          [14, 14, 8, 8, 8, 8])

rutile_dist = ([(3.97, 0, 0),
                (0, 4.03, 0),
                (0, 0, 3.0)],
               [(0, 0, 0),
                (0.5001, 0.5, 0.5),
                (0.3, 0.3, 0.0),
                (0.7, 0.7, 0.002),
                (0.2, 0.8, 0.5),
                (0.8, 0.2, 0.5)],
               [14, 14, 8, 8, 8, 8])

a = 3.07
c = 3.52
MgB2 = ([(a, 0, 0),
         (-a/2, a/2*np.sqrt(3), 0),
         (0, 0, c)],
        [(0, 0, 0),
         (1.0/3, 2.0/3, 0.5),
         (2.0/3, 1.0/3, 0.5)],
        [12, 5, 5])

a = [3., 0., 0.]
b = [-3.66666667, 3.68178701, 0.]
c = [-0.66666667, -1.3429469, 1.32364995]
niggli_lattice = np.array([a, b, c])

# For VASP case
# import vasp
# bulk = vasp.read_vasp(sys.argv[1])

print("[get_spacegroup]")
print("  Spacegroup of Silicon is %s." % spglib.get_spacegroup(silicon))
print('')
print("[get_spacegroup]")
print("  Spacegroup of Silicon (ASE Atoms-like format) is %s." %
      spglib.get_spacegroup(silicon_ase))
print('')
print("[get_spacegroup]")
print("  Spacegroup of Rutile is %s." % spglib.get_spacegroup(rutile))
print('')
print("[get_spacegroup]")
print("  Spacegroup of MgB2 is %s." % spglib.get_spacegroup(MgB2))
print('')
print("[get_symmetry]")
print("  Symmetry operations of Rutile unitcell are:")
print('')
symmetry = spglib.get_symmetry(rutile)
show_symmetry(symmetry)
print('')
print("[get_symmetry]")
print("  Symmetry operations of MgB2 are:")
print('')
symmetry = spglib.get_symmetry(MgB2)
show_symmetry(symmetry)
print('')
print("[get_pointgroup]")
print("  Pointgroup of Rutile is %s." %
      spglib.get_pointgroup(symmetry['rotations'])[0])
print('')

dataset = spglib.get_symmetry_dataset( rutile )
print("[get_symmetry_dataset] ['international']")
print("  Spacegroup of Rutile is %s (%d)." % (dataset['international'],
                                              dataset['number']))
print('')
print("[get_symmetry_dataset] ['pointgroup']")
print("  Pointgroup of Rutile is %s." % (dataset['pointgroup']))
print('')
print("[get_symmetry_dataset] ['hall']")
print("  Hall symbol of Rutile is %s (%d)." % (dataset['hall'],
                                               dataset['hall_number']))
print('')
print("[get_symmetry_dataset] ['wyckoffs']")
alphabet = "abcdefghijklmnopqrstuvwxyz"
print("  Wyckoff letters of Rutile are: ", dataset['wyckoffs'])
print('')
print("[get_symmetry_dataset] ['equivalent_atoms']")
print("  Mapping to equivalent atoms of Rutile are: ")
for i, x in enumerate(dataset['equivalent_atoms']):
  print("  %d -> %d" % (i + 1, x + 1))
print('')
print("[get_symmetry_dataset] ['rotations'], ['translations']")
print("  Symmetry operations of Rutile unitcell are:")
for i, (rot,trans) in enumerate(zip(dataset['rotations'],
                                    dataset['translations'])):
    print("  --------------- %4d ---------------" % (i + 1))
    print("  rotation:")
    for x in rot:
        print("     [%2d %2d %2d]" % (x[0], x[1], x[2]))
    print("  translation:")
    print("     (%8.5f %8.5f %8.5f)" % (trans[0], trans[1], trans[2]))
print('')

print("[refine_cell]")
print(" Refine distorted rutile structure")
lattice, positions, numbers = spglib.refine_cell(rutile_dist, symprec=1e-1)
show_cell(lattice, positions, numbers)
print('')

print("[find_primitive]")
print(" Fine primitive distorted silicon structure")
lattice, positions, numbers = spglib.find_primitive(silicon_dist, symprec=1e-1)
show_cell(lattice, positions, numbers)
print('')

print("[standardize_cell]")
print(" Standardize distorted rutile structure:")
print(" (to_primitive=0 and no_idealize=0)")
lattice, positions, numbers = spglib.standardize_cell(rutile_dist,
                                                      to_primitive=0,
                                                      no_idealize=0,
                                                      symprec=1e-1)
show_cell(lattice, positions, numbers)
print('')

print("[standardize_cell]")
print(" Standardize distorted rutile structure:")
print(" (to_primitive=0 and no_idealize=1)")
lattice, positions, numbers = spglib.standardize_cell(rutile_dist,
                                                      to_primitive=0,
                                                      no_idealize=1,
                                                      symprec=1e-1)
show_cell(lattice, positions, numbers)
print('')

print("[standardize_cell]")
print(" Standardize distorted silicon structure:")
print(" (to_primitive=1 and no_idealize=0)")
lattice, positions, numbers = spglib.standardize_cell(silicon_dist,
                                                      to_primitive=1,
                                                      no_idealize=0,
                                                      symprec=1e-1)
show_cell(lattice, positions, numbers)
print('')

print("[standardize_cell]")
print(" Standardize distorted silicon structure:")
print(" (to_primitive=1 and no_idealize=1)")
lattice, positions, numbers = spglib.standardize_cell(silicon_dist,
                                                      to_primitive=1,
                                                      no_idealize=1,
                                                      symprec=1e-1)
show_cell(lattice, positions, numbers)
print('')

symmetry = spglib.get_symmetry(silicon)
print("[get_symmetry]")
print("  Number of symmetry operations of silicon conventional")
print("  unit cell is %d (192)." % len(symmetry['rotations']))
show_symmetry(symmetry)
print('')

symmetry = spglib.get_symmetry_from_database(525)
print("[get_symmetry_from_database]")
print("  Number of symmetry operations of silicon conventional")
print("  unit cell is %d (192)." % len(symmetry['rotations']))
show_symmetry(symmetry)
print('')

reduced_lattice = spglib.niggli_reduce(niggli_lattice)
print("[niggli_reduce]")
print("  Original lattice")
show_lattice(niggli_lattice)
print("  Reduced lattice")
show_lattice(reduced_lattice)
print('')


mapping, grid = spglib.get_ir_reciprocal_mesh([11, 11, 11],
                                              silicon_prim,
                                              is_shift=[0, 0, 0])
num_ir_kpt = len(np.unique(mapping))
print("[get_ir_reciprocal_mesh]")
print("  Number of irreducible k-points of primitive silicon with")
print("  11x11x11 Monkhorst-Pack mesh is %d (56)." % num_ir_kpt)
print('')

mapping, grid = spglib.get_ir_reciprocal_mesh([8, 8, 8],
                                              rutile,
                                              is_shift=[1, 1, 1])
num_ir_kpt = len(np.unique(mapping))
print("[get_ir_reciprocal_mesh]")
print("  Number of irreducible k-points of Rutile with")
print("  8x8x8 Monkhorst-Pack mesh is %d (40)." % num_ir_kpt)
print('')

mapping, grid = spglib.get_ir_reciprocal_mesh([9, 9, 8],
                                              MgB2,
                                              is_shift=[0, 0, 1])
num_ir_kpt = len(np.unique(mapping))
print("[get_ir_reciprocal_mesh]")
print("  Number of irreducible k-points of MgB2 with")
print("  9x9x8 Monkhorst-Pack mesh is %s (48)." % num_ir_kpt)
print('')
