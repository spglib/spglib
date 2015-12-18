#!/bin/bash

VER=`(cd ../../ ; python -c "from setuptools_scm import get_version ; print(get_version())")`

echo "Building version: $VER"

echo $VER > __conda_version__.txt

