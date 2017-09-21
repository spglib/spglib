#!/bin/bash
set -e -x

conda install --yes python=$TRAVIS_PYTHON_VERSION pip numpy
aclocal
autoheader
libtoolize
touch INSTALL NEWS README AUTHORS
automake -acf
autoconf
./configure
make check # test in C
cd python
pwd
git branch -v
git describe --tags --dirty
./get_nanoversion.sh
cat __nanoversion__.txt
ln -s ../src .
python setup.py install 
cd ..
