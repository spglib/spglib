# How to make ruby wrapper

Ruby wrapper is created. With this, `symPoscar.rb` can read VASP
`POSCAR` type files and analyze their symmetries.

```
% ruby extconf.rb --with-getspg-include=../build/include --with-getspg-lib=../build/lib
% make
% export LD_LIBRARY_PATH=../build/lib
% export RUBYLIB=.
% ruby symPoscar.rb ../python/test/data/hexagonal/POSCAR-168
```
