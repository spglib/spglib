#!/bin/bash

VER=`(cd ../../ ; python -c "from setuptools_scm import get_version ; print(get_version())")`

echo "Building version: $VER in $SRC_DIR  $SOURCE_DIR"

echo "$VER" > __conda_version__.txt

