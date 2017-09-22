#!/bin/bash
set -e -x

# Install a system package required by our library
# yum install -y numpy

ls /opt/python

# Compile wheels
for PYBIN in /opt/python/cp27*/bin; do
    "${PYBIN}/pip" install numpy
    "${PYBIN}/pip" wheel /io/ -w wheelhouse/
done

for PYBIN in /opt/python/cp35*/bin; do
    "${PYBIN}/pip" install numpy
    "${PYBIN}/pip" wheel /io/ -w wheelhouse/
done

for PYBIN in /opt/python/cp36*/bin; do
    "${PYBIN}/pip" install numpy
    "${PYBIN}/pip" wheel /io/ -w wheelhouse/
done

# Bundle external shared libraries into the wheels
for whl in wheelhouse/*.whl; do
    auditwheel repair "$whl" -w /io/wheelhouse/
done

# Install packages and test
for PYBIN in /opt/python/cp27*/bin/; do
    "${PYBIN}/pip" install spglib --no-index -f /io/wheelhouse
done

for PYBIN in /opt/python/cp35*/bin/; do
    "${PYBIN}/pip" install spglib --no-index -f /io/wheelhouse
done

for PYBIN in /opt/python/cp36*/bin/; do
    "${PYBIN}/pip" install spglib --no-index -f /io/wheelhouse
done
