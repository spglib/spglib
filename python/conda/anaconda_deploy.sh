#!/bin/bash

export LBL=$1
export TKN=$2
export GIT_BRANCH=$3

conda install conda-build anaconda-client --yes
conda build conda --no-anaconda-upload
anaconda --token $TKN upload --label $LBL /home/travis/miniconda/conda-bld/linux-64/spglib*py*.tar.bz2

