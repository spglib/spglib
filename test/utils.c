#include <stdio.h>

#include "spglib.h"

void show_symmetry_operations(SPGCONST int (*rotations)[3][3],
                              const double (*translations)[3], const int size) {
    int i, j;
    for (i = 0; i < size; i++) {
        printf("--- %d ---\n", i + 1);
        for (j = 0; j < 3; j++) {
            printf("%2d %2d %2d\n", rotations[i][j][0], rotations[i][j][1],
                   rotations[i][j][2]);
        }
        printf("%f %f %f\n", translations[i][0], translations[i][1],
               translations[i][2]);
    }
}

void show_cell(SPGCONST double lattice[3][3], SPGCONST double positions[][3],
               const int types[], const int num_atoms) {
    int i;

    printf("Lattice parameter:\n");
    for (i = 0; i < 3; i++) {
        printf("%f %f %f\n", lattice[0][i], lattice[1][i], lattice[2][i]);
    }
    printf("Atomic positions:\n");
    for (i = 0; i < num_atoms; i++) {
        printf("%d: %f %f %f\n", types[i], positions[i][0], positions[i][1],
               positions[i][2]);
    }
}
