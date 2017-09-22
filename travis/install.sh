#!/bin/bash
set -e -x

aclocal
autoheader
libtoolize
touch INSTALL NEWS README AUTHORS
automake -acf
autoconf
./configure
make check # test in C
cd python
ln -s ../src .
python setup.py install 
cd ..
