import os
import re
import subprocess
import sys
import sysconfig

setup_type = sys.argv[1]

# Command line flags forwarded to CMake (for debug purpose)
cmake_cmd_args = []
for f in sys.argv:
    if f.startswith('-D'):
        cmake_cmd_args.append(f)

for f in cmake_cmd_args:
    sys.argv.remove(f)

try:
    from setuptools import Extension, setup
    from setuptools.command.build_ext import build_ext

    use_setuptools = True
    print("setuptools is used.")

    # Trick to pip install numpy when it's not installed.
    # Reference: https://stackoverflow.com/questions/2379898/
    class CustomBuildExtCommand(build_ext):
        def run(self):
            import numpy

            self.include_dirs.append(numpy.get_include())
            build_ext.run(self)

except ImportError:
    from distutils.core import Extension, setup

    use_setuptools = False
    print("distutils is used.")

    try:
        from numpy.distutils.misc_util import get_numpy_include_dirs
    except ImportError:
        print(
            "numpy.distutils.misc_util cannot be imported. Please install "
            "numpy first before installing spglib..."
        )
        sys.exit(1)


# Use the cmake build
# https://martinopilia.com/posts/2018/09/15/building-python-extension.html
# https://stackoverflow.com/questions/42585210/extending-setuptools-extension-to-use-cmake-in-setup-py
class CMakeExtension(Extension):
    def __init__(self, name, cmake_lists_dir='..', **kwa):
        Extension.__init__(self, name, sources=[], **kwa)
        self.cmake_lists_dir = os.path.abspath(cmake_lists_dir)


class CMakeBuildExt(build_ext):
    def build_extensions(self) -> None:
        # Ensure that CMake is present and working
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError('Cannot find CMake executable')
        for ext in self.extensions:
            extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
            # cfg = 'Debug' if options['--debug'] == 'ON' else 'Release'
            cfg = 'Release'
            cmake_args = [
                f"-DCMAKE_BUILD_TYPE={cfg}",
                f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{cfg.upper()}={extdir}",
                f"-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_{cfg.upper()}={self.build_temp}",
                f"-DPython_EXECUTABLE={sys.executable}",
                '-DWITH_Python=ON',
                '-DWITH_TESTS=OFF'
            ]

            cmake_args += cmake_cmd_args

            if not os.path.exists(self.build_temp):
                os.makedirs(self.build_temp)

            # Config
            subprocess.check_call(['cmake', ext.cmake_lists_dir] + cmake_args,
                                  cwd=self.build_temp)

            # Build
            subprocess.check_call(['cmake', '--build', '.', '--config', cfg],
                                  cwd=self.build_temp)

# Workaround Python issue 21121
config_var = sysconfig.get_config_var("CFLAGS")
if config_var is not None and "-Werror=declaration-after-statement" in config_var:
    os.environ["CFLAGS"] = config_var.replace("-Werror=declaration-after-statement", "")

if os.path.exists("src"):
    source_dir = "src"
else:
    source_dir = os.path.join(os.pardir, "src")

include_dirs = [
    source_dir,
]
if not use_setuptools:
    include_dirs += get_numpy_include_dirs()


extra_compile_args = []
if setup_type == "test":
    extra_compile_args.append("-UNDEBUG")
extra_link_args = []
define_macros = []

# Uncomment to activate OpenMP support for gcc
# extra_compile_args += ['-fopenmp']
# extra_link_args += ['-lgomp']

# For debugging
# define_macros = [('SPGWARNING', None),
#                  ('SPGDEBUG', None)]

extension = CMakeExtension("spglib._spglib")

with open(os.path.join('..', 'CMakeLists.txt')) as fl:
    # regex blackmagic: try out at regex101.com
    # Search for first "project(spglib VERSION ...)"
    # including whitespace, newlines, reordering and case-insensitive
    match = re.search('(?i)\s*project\s*\(\s*spglib[\s\S]*?VERSION ([0-9.]*)[\s\S]*?\)', fl.read())
    if not match:
        raise RuntimeError("Failed to find spglib version from cmake file")
    version = match.group(1)

# To deploy to pypi by travis-CI
nanoversion = 0
if os.path.isfile("__nanoversion__.txt"):
    with open("__nanoversion__.txt") as nv:
        try:
            for line in nv:
                nanoversion = int(line.strip())
                break
        except ValueError:
            nanoversion = 0
if nanoversion != 0:
    version += f"-{nanoversion}"

extras_require = {
    "testing": [
        "codecov",
        "pytest",
        "pytest-cov",
        "pytest-benchmark",
    ],
    "doc": [
        "Sphinx==4.5.0",
        "sphinx-autobuild==2021.3.14",
        "sphinxcontrib-bibtex==2.4.2",
        "sphinx-book-theme==0.3.3",
        "myst-parser==0.18.0",
        "linkify-it-py==2.0.0",
    ],
}

if use_setuptools:
    setup(
        name="spglib",
        version=version,
        cmdclass={"build_ext": CMakeBuildExt},
        setup_requires=["numpy", "setuptools>=18.0"],
        license="BSD-3-Clause",
        description="This is the spglib module.",
        long_description=open("README.rst", "rb").read().decode("utf-8"),
        long_description_content_type="text/x-rst",
        author="Atsushi Togo",
        author_email="atz.togo@gmail.com",
        url="http://spglib.github.io/spglib/",
        packages=["spglib"],
        install_requires=[
            "numpy",
        ],
        provides=["spglib"],
        platforms=["all"],
        ext_modules=[extension],
        tests_require=[
            "pyyaml",
        ],
        extras_require=extras_require,
    )
else:
    setup(
        name="spglib",
        version=version,
        license="BSD-3-Clause",
        description="This is the spglib module.",
        long_description=open("README.rst", "rb").read().decode("utf-8"),
        long_description_content_type="text/x-rst",
        author="Atsushi Togo",
        author_email="atz.togo@gmail.com",
        url="http://spglib.github.io/spglib/",
        packages=["spglib"],
        requires=["numpy"],
        provides=["spglib"],
        platforms=["all"],
        ext_modules=[extension],
        cmdclass={"build_ext": CMakeBuildExt},
        tests_require=[
            "pyyaml",
        ],
        extras_require=extras_require,
    )
