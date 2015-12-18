#!/bin/bash

VER=`(cd ../../ ; python -c "from setuptools_scm import get_version ; print(get_version())")`

echo "Building version: $VER in $SRC_DIR"

echo $VER > $SRC_DIR/__conda_version__.txt

