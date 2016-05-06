Spglib
======

Spglib is a library for finding and handling crystal symmetries
written in C.

Spglib for python is found at :ref:`python_spglib`.

Version history is summarized in `ChangeLog
<https://github.com/atztogo/spglib/blob/master/ChangeLog>`_.

News
-----

* SpglibDataset is slightly modified at version 1.9.4. This may hit
  some users.

* Python module pyspglib is renamed to spglib in version 1.9.0.

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

Documentation
--------------

.. toctree::
   :maxdepth: 1

   install
   api
   variable
   Python interface <python-spglib>
   interface
   definition

References
----------

General materials

* International tables for crystallography volumes A, B, C
* Online dictionary of crystallography:
  http://reference.iucr.org/dictionary/
* Symmetry relationships between crystal structures by Ulrich Muller,
  Oxford university press (2013)

Specific algorithms

* Space-group type determination: R. W. Grosse-Kunstleve, Acta Cryst.,
  A55, 383-395 (1999)
* Crystal refinement: R. W. Grosse-Kunstleve and P. D. Adams, Acta
  Cryst., A58, 60-65 (2002)

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
for Python packages.
