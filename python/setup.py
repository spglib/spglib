import os

try:
    from setuptools import setup, Extension
    use_setuptools = True
    print("setuptools is used.")
except ImportError:
    from distutils.core import setup, Extension
    use_setuptools = False
    print("distutils is used.")

try:
    from numpy.distutils.misc_util import get_numpy_include_dirs
except ImportError:
    print("numpy.distutils.misc_util cannot be imported. Please install "
          "numpy first before installing spglib...")
    sys.exit(1)

# Workaround Python issue 21121
import sysconfig
config_var = sysconfig.get_config_var("CFLAGS")
if config_var is not None and "-Werror=declaration-after-statement" in config_var:
    os.environ['CFLAGS'] = config_var.replace(
        "-Werror=declaration-after-statement", "")    

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

if os.path.exists('src'):
    source_dir = "src"
else:
    source_dir = "../src"
include_dirs = [source_dir,]
for i,s in enumerate(sources):
    sources[i] = "%s/%s" % (source_dir, s) 

extra_compile_args = []
extra_link_args = []
define_macros = []

## Uncomment to activate OpenMP support for gcc
# extra_compile_args += ['-fopenmp']
# extra_link_args += ['-lgomp']

## For debugging
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

version = ".".join(["%d" % n for n in version_nums])
if use_setuptools:
    setup(name='spglib',
          version=version,
          description='This is the spglib module.',
          author='Atsushi Togo',
          author_email='atz.togo@gmail.com',
          url='http://atztogo.github.io/spglib/',
          packages=['spglib'],
          install_requires=['numpy'],
          provides=['spglib'],
          platforms=['all'],
          ext_modules=[extension])
else:
    setup(name='spglib',
          version=version,
          description='This is the spglib module.',
          author='Atsushi Togo',
          author_email='atz.togo@gmail.com',
          url='http://atztogo.github.io/spglib/',
          packages=['spglib'],
          requires=['numpy'],
          provides=['spglib'],
          platforms=['all'],
          ext_modules=[extension])
