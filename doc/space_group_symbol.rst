Space-group symbol found by spglib
-----------------------------------

There are equivalent space-group symbols for a space-group type
according to the choices of axes and origin. Spglib returns one of
them without any rule except that base centring is always
set as **C-centring**.

In symmetry search, an input cell is transformed to a conventional
unit by change of basis and origin shift. The results are given as
``transformation_matrix`` and ``origin_shift`` which are obtained by
calling :ref:`api_spg_get_dataset`.

.. |sflogo| image:: http://sflogo.sourceforge.net/sflogo.php?group_id=161614&type=1
            :target: http://sourceforge.net

|sflogo|
