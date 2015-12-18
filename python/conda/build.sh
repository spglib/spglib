#!/bin/bash

python -c "from setuptools_scm import get_version ; print(get_version())" > __conda_version__.txt

echo "Building in $SRC_DIR  $SOURCE_DIR"

cd python

$PYTHON setup.py install

# Add more build steps here, if they are necessary.

# See
# http://docs.continuum.io/conda/build.html
# for a list of environment variables that are set during the build process.
