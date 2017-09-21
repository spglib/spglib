#!/bin/bash
set -e -x

cd test
python -m unittest discover -b
cd ..
