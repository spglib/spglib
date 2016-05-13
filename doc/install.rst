How to install spglib C-API
============================

Download & install
-------------------

Major releases are found at `SourceForge
<https://sourceforge.net/project/showfiles.php?group_id=215020>`_.

1. Unzip, configure, and make::

   % tar xvfz spglib-1.9.4.tar.gz
   % cd spglib-1.9.4
   % ./configure --prefix=INSTALL_LOCATION
   % make
   % make install

2. The libraries are installed at ``INSTALL_LOCATION`` or found in
   ``src/.libs`` if you don't run ``make install``,

Full package is found at `GitHub
<https://github.com/atztogo/spglib/releases>`_. In this case, if you
need configure script, it can be created using autotools and libtool
as follows::

   % aclocal
   % autoheader
   % libtoolize # or glibtoolize with macport etc
   % touch INSTALL NEWS README AUTHORS
   % automake -acf
   % autoconf

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
