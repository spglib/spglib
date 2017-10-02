#!/bin/bash
set -e -x

# UnsatisfiableError: The following specifications were found to be in conflict:
#   - pyopenssl 17.0.0 py36_0 -> python 3.6*
#   - python 3.5*
if [[ "$TRAVIS_PYTHON_VERSION" == "3.6" ]]; then
    wget http://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh -O miniconda.sh;
else
    wget http://repo.continuum.io/miniconda/Miniconda-latest-Linux-x86_64.sh -O miniconda.sh;
fi
echo $TRAVIS_PYTHON_VERSION
bash miniconda.sh -b -p $HOME/miniconda
export PATH="$HOME/miniconda/bin:$PATH"
conda update --yes conda
conda install --no-update-deps --yes python="$TRAVIS_PYTHON_VERSION" pip numpy
