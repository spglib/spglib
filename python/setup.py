import os
from distutils.core import setup, Extension
from numpy.distutils.misc_util import get_numpy_include_dirs

include_dirs = ['src']
sources = ['src/arithmetic.c',
           'src/cell.c',
           'src/delaunay.c',
           'src/hall_symbol.c',
           'src/kgrid.c',
           'src/kpoint.c',
           'src/mathfunc.c',
           'src/niggli.c',
           'src/pointgroup.c',
           'src/primitive.c',
           'src/refinement.c',
           'src/sitesym_database.c',
           'src/site_symmetry.c',
           'src/spacegroup.c',
           'src/spin.c',
           'src/spg_database.c',
           'src/spglib.c',
           'src/symmetry.c']
extra_compile_args = []
extra_link_args = []

## Uncomment to activate OpenMP support for gcc
# extra_compile_args += ['-fopenmp']
# extra_link_args += ['-lgomp']

define_macros = []
# define_macros = [('SPGWARNING', None),
#                  ('SPGDEBUG', None)]

extension = Extension('spglib._spglib',
                      include_dirs=include_dirs + get_numpy_include_dirs(),
                      sources=['_spglib.c'] + sources,
                      extra_compile_args=extra_compile_args,
                      extra_link_args=extra_link_args,
                      define_macros=define_macros)

version_nums = [None, None, None]
with open("src/version.h") as w:
    for line in w:
        for i, chars in enumerate(("MAJOR", "MINOR", "MICRO")):
            if chars in line:
                version_nums[i] = int(line.split()[2])

# To deploy to pypi by travis-CI
nanoversion = 0
if os.path.isfile("__nanoversion__.txt"):
    with open('__nanoversion__.txt') as nv:
        try :
            for line in nv:
                nanoversion = int(line.strip())
                break
        except ValueError :
            nanoversion=0
        if nanoversion:
            version_nums.append(nanoversion)

if None in version_nums:
    print("Failed to get version number in setup.py.")
    raise

setup(name='spglib',
      version=(".".join(["%d" % n for n in version_nums])),
      description='This is the spglib module.',
      author='Atsushi Togo',
      author_email='atz.togo@gmail.com',
      url='http://atztogo.github.io/spglib/',
      packages=['spglib'],
      requires=['numpy'],
      provides=['spglib'],
      platforms=['all'],
      ext_modules=[extension])
