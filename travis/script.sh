#!/bin/bash
set -e -x

export PATH="$HOME/miniconda/bin:$PATH"
cd python/test
python -m unittest discover -b
cd ../..
