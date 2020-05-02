.. _interfaces:

Rust, Fortran, and Ruby interfaces
==================================

The Fortran, and Ruby interfaces are expected to work at a certain
level, but not supported as well as the Python interface.

Fortran interface
------------------

Fortran interface `spglib_f08.f90
<https://github.com/spglib/spglib/blob/master/example/example_f08.f90>`_
is found in `example
<https://github.com/spglib/spglib/tree/master/example>`_
directory. This fortran interface is originally contributed by Dimitar
Pashov and has been updated by some other people. `spglib_f.c
<https://github.com/spglib/spglib/blob/master/src/spglib_f.c>`_ is
obsolete and is not recommended to use.

Rust interface
--------------

Rust interface contributed by Seaton Ullberg. The documentation is
found at https://docs.rs/crate/spglib/.

Ruby interface
---------------

Note that this ruby interface was written in ancient times, so may not
work with the recent ruby.

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
`ruby/README <https://github.com/spglib/spglib/blob/master/ruby/README>`_.
