Fortran and Ruby interfaces
============================

It is expected that Fortran and Ruby interfaces work at a certain
level but not as goog as Python interface.

Ruby interface
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
