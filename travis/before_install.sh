#!/bin/bash
set -e -x

if [[ "$TRAVIS_PYTHON_VERSION" == "2.7" ]]; then
    # wget http://repo.continuum.io/miniconda/Miniconda-latest-Linux-x86_64.sh -O miniconda.sh;
    wget https://repo.continuum.io/miniconda/Miniconda2-4.3.21-Linux-x86_64.sh -O miniconda.sh;
else
    # wget http://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh -O miniconda.sh;
    wget https://repo.continuum.io/miniconda/Miniconda3-4.3.21-Linux-x86_64.sh -O miniconda.sh;
fi
echo $TRAVIS_PYTHON_VERSION
bash miniconda.sh -b -p $HOME/miniconda
export PATH="$HOME/miniconda/bin:$PATH"
# conda update --yes conda
conda install --yes python="$TRAVIS_PYTHON_VERSION" pip numpy
