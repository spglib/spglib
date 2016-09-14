spglib
======

|Version Badge| |Downloads Badge| |Build Status|

Python bindings for C library for finding and handling crystal symmetries

Installation
-------------

Source codes, examples, and the test are downloaded `SourceForge
<https://sourceforge.net/project/showfiles.php?group_id=215020>`_ or
`GitHub
<https://github.com/atztogo/spglib/releases>`_.

Using package distribution service
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The easiest way to install python-spglib is to use the pypi package,
for which numpy is required to be installed before the
installation. A command to install spglib is::

   % pip install spglib

Conda is another choice::

   % conda install -c jochym spglib

These packages are made by Paweł T. Jochym.

Building using setup.py
^^^^^^^^^^^^^^^^^^^^^^^^

To manually install python-spglib using ``setup.py``, python header
files (python-dev), C-compiler (e.g., gcc, clang), and numpy are
required before the build. The installation steps are shown as
follows:

1. Go to the :file:`python` directory
2. Type the command::

      % python setup.py install --user

   Document about where spglib is installed is found at the
   links below:
   
   - https://docs.python.org/2/install/#alternate-installation-the-user-scheme
   - https://docs.python.org/3/install/#alternate-installation-the-user-scheme


.. |Version Badge| image:: https://anaconda.org/jochym/spglib/badges/version.svg
   :target: https://anaconda.org/jochym/spglib
.. |Downloads Badge| image:: https://anaconda.org/jochym/spglib/badges/downloads.svg
   :target: https://anaconda.org/jochym/spglib
.. |Build Status| image:: https://travis-ci.org/jochym/spglib.svg?branch=master
   :target: https://travis-ci.org/jochym/spglib
