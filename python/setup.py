import os
import re
from skbuild import setup

# TODO: Temporary fix for #212
#  Remove and update dependency when scikit-build/scikit-build#717 is resolved
import platform
if platform.system() == "Darwin":
    mac_version, _, mac_arch = platform.mac_ver()
else:
    mac_version = None

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

cmake_args = ['-DWITH_Python=ON', '-DWITH_TESTS=OFF', '-DUSE_OMP=OFF', '-DBUNDLE_Python_SharedLib=ON']
# TODO: Temporary fix for #212
if mac_version:
    cmake_args.append(f"-DCMAKE_OSX_DEPLOYMENT_TARGET={mac_version}")

setup(
    name="spglib",
    version=version,
    setup_requires=["numpy", "scikit-build"],
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
    tests_require=[
        "pyyaml",
    ],
    cmake_args=cmake_args,
    cmake_source_dir="..",
    extras_require=extras_require,
)
