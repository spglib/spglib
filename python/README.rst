spglib
======

Python bindings for C library for finding and handling crystal
symmetries

Installation
------------

The package is developed on GitHub. You can get the source for the
released versions from the
`repository <https://github.com/spglib/spglib/releases>`__.

Note that the Python bindings will link to the system/environment
installed Spglib (loaded from :code:`LD_LIBRARY_PATH` or :code:`DYLD_LIBRARY_PATH`),
and it will default to a bundled version otherwise. To always use the
system version of Spglib (e.g. in a lua-module environment) delete the
bundled libraries :code:`libsymspg.so*` in the python Spglib installation
directory.

Using package distribution service
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The easiest way to install python-spglib is to use the pypi package, for
which numpy is required to be installed before the installation. A
command to install spglib is:

.. code-block:: console

    $ pip install spglib

Conda is another choice:

.. code-block:: console

    $ conda install -c conda-forge spglib

These packages are made by Pawel T. Jochym.

Building from source
~~~~~~~~~~~~~~~~~~~~

To build from source, make sure you have installed the build requirements
in :code:`pyproject.toml`. Then simply run the following in the root directory.

.. code-block:: console

    $ python -m build
    $ pip install .

To include testing or documentation environments, simply include the relevant extras

.. code-block:: console

    $ pip install .[test]

Running tests
~~~~~~~~~~~~~

After installing the project with appropriate extras (:code:`testing`), either from
package manager or from source, simply run :code:`pytest` in the current directory
to test the python bindings

.. code-block:: console

    $ pytest
