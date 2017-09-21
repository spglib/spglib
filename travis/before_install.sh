#!/bin/bash
set -e -x

if [[ "$TRAVIS_PYTHON_VERSION" == "2.7" ]]; then
  wget http://repo.continuum.io/miniconda/Miniconda-latest-Linux-x86_64.sh -O miniconda.sh;
else
  wget http://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh -O miniconda.sh;
fi
echo $TRAVIS_PYTHON_VERSION
bash miniconda.sh -b -p $HOME/miniconda
export PATH="$HOME/miniconda/bin:$PATH"
conda update --yes conda
