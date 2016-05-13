import os
from distutils.core import setup, Extension
from numpy.distutils.misc_util import get_numpy_include_dirs

sources = ['arithmetic.c',
           'cell.c',
           'delaunay.c',
           'hall_symbol.c',
           'kgrid.c',
           'kpoint.c',
           'mathfunc.c',
           'niggli.c',
           'pointgroup.c',
           'primitive.c',
           'refinement.c',
           'sitesym_database.c',
           'site_symmetry.c',
           'spacegroup.c',
           'spin.c',
           'spg_database.c',
           'spglib.c',
           'symmetry.c']
extra_compile_args = []
extra_link_args = []

if os.path.exists('src'):
    source_dir = "src"
else:
    source_dir = "../src"
include_dirs = [source_dir,]
for i in range(len(sources)):
    sources[i] = "%s/%s" % (source_dir, sources[i]) 

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
with open("%s/version.h" % source_dir) as w:
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
            nanoversion = 0
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
