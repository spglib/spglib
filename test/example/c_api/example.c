#include <stdio.h>

#include "spglib.h"

int main(int argc, char *argv[]) {
    SpglibDataset *dataset;
    int i, j;
    char const wl[26] = "abcdefghijklmnopqrstuvwxyz";

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

    // SpglibDataset has to be freed after use.
    dataset = spg_get_dataset(lattice, position, types, num_atom, symprec);

    printf("International symbol: %s (%d)\n", dataset->international_symbol,
           dataset->spacegroup_number);
    printf("Hall symbol:   %s\n", dataset->hall_symbol);
    printf("Wyckoff letters:\n");
    for (i = 0; i < dataset->n_atoms; i++) {
        printf("%c ", wl[dataset->wyckoffs[i]]);
    }
    printf("\n");
    printf("Equivalent atoms:\n");
    for (i = 0; i < dataset->n_atoms; i++) {
        printf("%d -> %d\n", i, dataset->equivalent_atoms[i]);
    }
    printf("Space group operations:\n");
    for (i = 0; i < dataset->n_operations; i++) {
        printf("--- %d ---\n", i + 1);
        for (j = 0; j < 3; j++) {
            printf("%2d %2d %2d\n", dataset->rotations[i][j][0],
                   dataset->rotations[i][j][1], dataset->rotations[i][j][2]);
        }
        printf("%f %f %f\n", dataset->translations[i][0],
               dataset->translations[i][1], dataset->translations[i][2]);
    }

    // Deallocate SpglibDataset, otherwise induce memory leak.
    spg_free_dataset(dataset);
}
