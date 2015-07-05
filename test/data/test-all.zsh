#!/bin/zsh

echo "Triclinic"
./test-tricli.zsh $1

echo "Monoclinic"
./test-monocli.zsh $1

echo "Orthorhombic"
./test-ortho.zsh $1

echo "Tetragonal"
./test-tetra.zsh $1

echo "Trigonal"
./test-trigo.zsh $1

echo "Hexagonal"
./test-hexa.zsh $1

echo "Cubic"
./test-cubic.zsh $1

echo "Virtual"
cd virtual_structure
./test-virtual.zsh|tee log
diff log spg_reference.txt
rm log
cd ..

echo "Other"
cd others
./test-others.zsh
cd ..
