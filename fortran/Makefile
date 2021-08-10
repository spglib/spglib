.SUFFIXES:

fc = gfortran -g -fcheck=all -fbacktrace
# fc = ifort -g -check all -traceback
lib = -lsymspg
ldflags ?= -L ../build/lib -L ../_build  -Wl,-rpath,'../build/lib:../_build'

test: example
	./example


example: example_f08.f90.o spglib_f08.f90.o
	$(fc) $^ $(lib) -o $@  $(ldflags)

example_f08.f90.o: spglib_f08.mod
spglib_f08.mod: spglib_f08.f90.o

%.f90.o: %.f90
	$(fc) -c $< -o $@

clean:
	rm -f example{,_f08.f90.o} defs_basis.mod spglib_f08.{f90.o,mod}
