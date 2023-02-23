# C example

This is the README file for spglib examples for C.

## Examples of using spglib in a cmake project

See the local [`CMakeLists.txt`](CMakeLists.txt) file for how to use
spglib in a cmake project, using either native cmake options or pkg-config.

## Examples of linking to spglib manually

To manually compile `example.c`:

```console
gcc example.c -I[include directory] -L[library directory] -lsymspg -lm
```

where the `[include directory]` and `[library directory]` can be obtained
via `pkg-config --cflags --libs spglib`.

## Concept

A crystal structure is given to a spglib function to obtain symmetry
information. `spg_get_dataset` is the function that returns most fruitful
space group information. This returns the result in the C-structure
`SpglibDataset`. We don't know the length of symmetry operations to be returned,
the memory spaces are allocated dynamically, which means we have to free them.
by `spg_free_dataset`. To tolerate tiny distortion of crystal structure,
it has to be specified when calling symmetry search function. The following
pages show the details.

- [Crystal structure format](https://spglib.github.io/spglib/variable.html)
- [Functions](https://spglib.github.io/spglib/api.html)
- [Spglib dataset](https://spglib.github.io/spglib/dataset.html)

```C
#include "spglib.h"

int main(void) {
    SpglibDataset *dataset;
    // Wurtzite structure (P6_3mc)
    double lattice[3][3] = {
        {3.111, -1.5555, 0}, {0, 2.6942050311733885, 0}, {0, 0, 4.988}};
    double position[4][3] = {
        {1.0 / 3, 2.0 / 3, 0.0},
        {2.0 / 3, 1.0 / 3, 0.5},
        {1.0 / 3, 2.0 / 3, 0.6181},
        {2.0 / 3, 1.0 / 3, 0.1181},
    };
    int types[4] = {1, 1, 2, 2};
    int num_atom = 4;
    double symprec = 1e-5;

    // SplibDataset has to be freed after use.
    dataset = spg_get_dataset(lattice, position, types, num_atom, symprec);

    // Obtain data in SpglibDataset.
    dataset->spacegroup_number;

    // Deallocate SpglibDataset, otherwise induce memory leak.
    spg_free_dataset(dataset);
}
```
