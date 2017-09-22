#!/bin/bash
set -e -x

cd python/test
python -m unittest discover -b
cd ../..
