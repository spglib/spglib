#!/bin/bash

export LBL=$1
export TKN=$2
export GIT_BRANCH=$3

conda install conda-build anaconda-client --yes
TRG=`conda build conda --no-anaconda-upload --output`
anaconda --token $TKN upload --label $LBL $TRG

