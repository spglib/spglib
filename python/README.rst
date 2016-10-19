spglib
======

|Version Badge| |Downloads Badge| |PyPi downloads| |Build Status|

Python bindings for C library for finding and handling crystal
symmetries

Installation
------------

The package is developed on github. You can get the source for the
released versions from the
`repository <https://github.com/atztogo/spglib/releases>`__.

Using package distribution service
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The easiest way to install python-spglib is to use the pypi package, for
which numpy is required to be installed before the installation. A
command to install spglib is:

::

    pip install spglib

Conda is another choice:

::

    conda install -c jochym spglib

These packages are made by Paweł T. Jochym.

Building using setup.py
~~~~~~~~~~~~~~~~~~~~~~~

To manually install python-spglib using ``setup.py``, python header
files (python-dev), C-compiler (e.g., gcc, clang), and numpy are
required before the build. The installation steps are shown as follows:

1. Go to the python directory
2. Type the command:

   ::

       python setup.py install --user

.. |Version Badge| image:: https://anaconda.org/jochym/spglib/badges/version.svg
   :target: https://anaconda.org/jochym/spglib
.. |Downloads Badge| image:: https://anaconda.org/jochym/spglib/badges/downloads.svg
   :target: https://anaconda.org/jochym/spglib
.. |PyPi downloads| image:: https://img.shields.io/pypi/dm/spglib.svg?maxAge=2592000   
   :target: https://pypi.python.org/pypi/spglib
.. |Build Status| image:: https://travis-ci.org/jochym/spglib.svg?branch=master
   :target: https://travis-ci.org/jochym/spglib
