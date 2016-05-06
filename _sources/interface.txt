Python, Fortran and Ruby interfaces
===================================

Only Python interface is well maintained. But It is expected Fortran
and Ruby interfaces work at certain level.

Python interface
-----------------

Python module is prepared in the ``python`` directory.
See the details at :ref:`python_spglib`.

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
