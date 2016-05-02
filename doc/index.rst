Spglib
======

Spglib is a library for finding and handling crystal symmetries
written in C.

Spglib for python is found at :ref:`python_spglib`.

Version history is summarized in `ChangeLog
<https://github.com/atztogo/spglib/blob/master/ChangeLog>`_.

News
-----

**Python module pyspglib is renamed to spglib in version 1.9.0.**

In versions 1.8.x or before, the python wrapper (pyspglib) was
considered as a special extention of spglib and separately maintained,
but in version 1.9.x and later, it starts to be a part of spglib as a
usual extension, and this python wrapper will be maintained to work
following every update of c-spglib. The package name is changed from
pyspglib to spglib. Following this change, the way to import python
spglib module is changed and it is written in :ref:`python_spglib`.

Features
----------

* Find symmetry operations
* Identify space-group type
* Wyckoff position assignment
* Refine crystal structure
* Find a primitive cell
* Search irreducible k-points
* Liner tetrahedron method (the document is to be prepared.)

Algorithms
------------

* Space-group type finding algorithm: R. W. Grosse-Kunstleve, Acta
  Cryst., A55, 383-395 (1999)

* Crystal refinement algorithm: R. W. Grosse-Kunstleve and
  P. D. Adams, Acta Cryst., A58, 60-65 (2002)

Install
----------

1. `Download spglib
   <https://sourceforge.net/project/showfiles.php?group_id=215020>`_.

2. Unzip, configure, and make::

   % tar xvfz spglib-1.9.4.tar.gz
   % cd spglib-1.9.4
   % ./configure --prefix=INSTALL_LOCATION
   % make
   % make install

3. The libraries are installed at ``INSTALL_LOCATION`` or found in
   ``src/.libs`` if you don't run ``make install``,

OpenMP
^^^^^^^

Bottle neck of symmetry operation search may be eased using the OpenMP
threading. In the case of gcc (> 4.2), set the following environment
variables before running configure script::

   % export LIBS='-lgomp'
   % export CFLAGS='-fopenmp'

Overhead of threading is relatively large when the number of atoms is
small. Therefore the threading is activated when the number of atoms
>= 1000.

Usage
------

1. Include ``spglib.h``
2. Link ``libsymspg.a`` or ``libsymspg.so``
3. A compilation example is shown in  `example/README
   <https://github.com/atztogo/spglib/blob/master/example/README>`_.

Example
--------

A few examples are found in `example
<https://github.com/atztogo/spglib/tree/master/example>`_ directory.

Python extension
-----------------

Python module is prepared in the ``python`` directory.
See the details at :ref:`python_spglib`.

Ruby extension
---------------

The ruby script ``symPoscar.rb`` in ``ruby`` directory reads a VASP
POSCAR formatted file and finds the space-group type. The ``-l``
option refines a slightly distorted structure within tolerance
spacified with the ``-s`` option. The ``-o`` option gives the symmetry
operations of the input structure.

::

   ruby symPoscar.rb POSCAR -s 0.1 -o -l


Some more options are shown with ``--help`` option::

   ruby symPoscar.rb --help

The way to compile the ruby module is explained in 
`ruby/README <https://github.com/atztogo/spglib/blob/master/ruby/README>`_.

Fortran interface
------------------

Fortran interface `spglib_f08.f90
<https://github.com/atztogo/spglib/blob/master/example/example_f08.f90>`_
is found in `example
<https://github.com/atztogo/spglib/tree/master/example>`_
directory. This fortran interface is the contribution from Dimitar
Pashov. `spglib_f.c
<https://github.com/atztogo/spglib/blob/master/src/spglib_f.c>`_ is
obsolete and is not recommended to use.

API & variables
----------------

.. toctree::
   :maxdepth: 2

   definition
   api
   variable
   python-spglib

Mailing list
-------------

For questions, bug reports, and comments, please visit following
mailing list:

https://lists.sourceforge.net/lists/listinfo/spglib-users

For more information
---------------------

* Repository: https://github.com/atztogo/spglib 
* License: New BSD after version 1.0.beta-1. The older versions are under GPLv2.
* Contact: atz.togo@gmail.com
* Authour: Atsushi Togo

Acknowledgments
----------------

Spglib project acknowledges Yusuke Seto for the Crystallographic
database, Dimitar Pashov for the fortran interface, Paweł T. Jochym
for python packages.
