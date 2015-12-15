import os
from distutils.core import setup, Extension
from numpy.distutils.misc_util import get_numpy_include_dirs

include_dirs = ['../src']
sources = ['../src/cell.c',
           '../src/hall_symbol.c',
           '../src/kgrid.c',
           '../src/kpoint.c',
           '../src/lattice.c',
           '../src/mathfunc.c',
           '../src/niggli.c',
           '../src/pointgroup.c',
           '../src/primitive.c',
           '../src/refinement.c',
           '../src/sitesym_database.c',
           '../src/site_symmetry.c',
           '../src/spacegroup.c',
           '../src/spin.c',
           '../src/spg_database.c',
           '../src/spglib.c',
           '../src/symmetry.c']
extra_compile_args = []
extra_link_args = []

## Uncomment to activate OpenMP support for gcc
# extra_compile_args += ['-fopenmp']
# extra_link_args += ['-lgomp']

extension = Extension('pyspglib._spglib',
                      include_dirs=include_dirs + get_numpy_include_dirs(),
                      sources=['_spglib.c'] + sources,
                      extra_compile_args=extra_compile_args,
                      extra_link_args=extra_link_args)

version_nums = [None, None, None]
with open(include_dirs[0] + "/version.h") as w:
    for line in w:
        for i, chars in enumerate(("MAJOR", "MINOR", "MICRO")):
            if chars in line:
                version_nums[i] = int(line.split()[2])

if None in version_nums:
    print("Failed to get version number in setup.py.")
    raise

setup(name='spglib',
      version="%d.%d.%d" % tuple(version_nums),
      description='This is the spglib module.',
      author='Atsushi Togo',
      author_email='atz.togo@gmail.com',
      url='http://spglib.sourceforge.net/',
      packages=['pyspglib'],
      requires = ['numpy'],
      provides = ['pyspglib'],
      platforms = ['all'],
      ext_modules=[extension])
