#!/bin/bash

git describe --tags --dirty | sed -e 's/-/_/g' -e 's/^v//g' > __conda_version__.txt

cd python

$PYTHON setup.py install

# Add more build steps here, if they are necessary.

# See
# http://docs.continuum.io/conda/build.html
# for a list of environment variables that are set during the build process.