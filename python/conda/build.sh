#!/bin/bash

git checkout $GIT_BRANCH

cd python
./get_nanoversion.sh
ln -s ../src .

$PYTHON setup.py install

# Add more build steps here, if they are necessary.

# See
# http://docs.continuum.io/conda/build.html
# for a list of environment variables that are set during the build process.
