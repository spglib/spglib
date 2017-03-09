How to install spglib C-API
============================

Download
---------

The source code is downloaded at
https://github.com/atztogo/spglib/releases . 
But minor updates are not included in this package. If you want the
latest version, you can git-clone the spglib repository::

   % git clone https://github.com/atztogo/spglib.git

Install
--------

Compiling using cmake
^^^^^^^^^^^^^^^^^^^^^^

1. After expanding source code, go into the source code directory::

     % tar xvfz spglib-1.9.8.tar.gz
     % cd spglib-1.9.8

   The current directory is ``PROJECT_SOURCE_DIR``.

2. Create a directory for the build::

     % mkdir _build; cd _build
     % cmake ..
     % make
     % make install

3. The libraries are installed at ``PROJECT_SOURCE_DIR/lib`` and the
   header file is installed at ``PROJECT_SOURCE_DIR/include``.

Compiling using configure script
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. The configure script is prepared using
   autotools and libtool as follows::

     % aclocal
     % autoheader
     % libtoolize # or glibtoolize with macport etc
     % touch INSTALL NEWS README AUTHORS
     % automake -acf
     % autoconf


2. Run configure script::

     % tar xvfz spglib-1.9.8.tar.gz
     % cd spglib-1.9.8
     % ./configure --prefix=INSTALLATION_LOCATION
     % make
     % make install

3. The libraries are installed at ``INSTALLATION_LOCATION/lib`` or found in
   ``src/.libs`` if you don't run ``make install``.

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
