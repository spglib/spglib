#include <stdio.h>
#include <stdlib.h>

#include "spglib.h"

void show_symmetry_operations(int const (*rotations)[3][3],
                              double const (*translations)[3], int const size) {
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

void show_matrix_3d(double const lattice[3][3]) {
    for (int i = 0; i < 3; i++) {
        printf("%f %f %f\n", lattice[0][i], lattice[1][i], lattice[2][i]);
    }
}

void show_cell(double const lattice[3][3], double const positions[][3],
               int const types[], int const num_atoms) {
    printf("Lattice parameter:\n");
    show_matrix_3d(lattice);
    printf("Atomic positions:\n");
    for (int i = 0; i < num_atoms; i++) {
        printf("%d: %f %f %f\n", types[i], positions[i][0], positions[i][1],
               positions[i][2]);
    }
}

void show_spacegroup_type(SpglibSpacegroupType const spgtype) {
    printf("Number:            %d\n", spgtype.number);
    printf("International:     %s\n", spgtype.international_short);
    printf("International:     %s\n", spgtype.international_full);
    printf("International:     %s\n", spgtype.international);
    printf("Schoenflies:       %s\n", spgtype.schoenflies);
    printf("Hall symbol:       %s\n", spgtype.hall_symbol);
    printf("Point group intl:  %s\n", spgtype.pointgroup_international);
    printf("Point group Schoe: %s\n", spgtype.pointgroup_schoenflies);
    printf("Arithmetic cc num. %d\n", spgtype.arithmetic_crystal_class_number);
    printf("Arithmetic cc sym. %s\n", spgtype.arithmetic_crystal_class_symbol);
}

void show_magnetic_symmetry_operations(int const (*rotations)[3][3],
                                       double const (*translations)[3],
                                       int const *time_reversals,
                                       int const size) {
    int i, j;
    for (i = 0; i < size; i++) {
        printf("--- %d ---\n", i + 1);
        for (j = 0; j < 3; j++) {
            printf("%2d %2d %2d\n", rotations[i][j][0], rotations[i][j][1],
                   rotations[i][j][2]);
        }
        printf("%f %f %f\n", translations[i][0], translations[i][1],
               translations[i][2]);
        printf("%2d\n", time_reversals[i]);
    }
}

void show_magnetic_spacegroup_type(SpglibMagneticSpacegroupType const msgtype) {
    printf("UNI Number:    %d\n", msgtype.uni_number);
    printf("Litvin Number: %d\n", msgtype.litvin_number);
    printf("BNS Number:    %s\n", msgtype.bns_number);
    printf("OG Number:     %s\n", msgtype.og_number);
    printf("Number:        %d\n", msgtype.number);
    printf("Type:          %d\n", msgtype.type);
}

void show_spg_magnetic_dataset(SpglibMagneticDataset const *dataset) {
    int i, p, s;
    printf("UNI number: %d\n", dataset->uni_number);
    printf("Type: %d\n", dataset->msg_type);
    printf("Hall number: %d\n", dataset->hall_number);

    printf("\nSymmetry operations\n");
    for (p = 0; p < dataset->n_operations; p++) {
        printf("--- %d ---\n", p + 1);
        for (s = 0; s < 3; s++) {
            printf("%2d %2d %2d\n", dataset->rotations[p][s][0],
                   dataset->rotations[p][s][1], dataset->rotations[p][s][2]);
        }
        printf("%f %f %f\n", dataset->translations[p][0],
               dataset->translations[p][1], dataset->translations[p][2]);
        printf("%d\n", dataset->time_reversals[p]);
    }

    printf("\nEquivalent atoms:\n");
    for (i = 0; i < dataset->n_atoms; i++) {
        printf(" %d", dataset->equivalent_atoms[i]);
    }
    printf("\n");

    printf("\nTransformation matrix:\n");
    for (s = 0; s < 3; s++) {
        printf("%f %f %f\n", dataset->transformation_matrix[s][0],
               dataset->transformation_matrix[s][1],
               dataset->transformation_matrix[s][2]);
    }
    printf("Origin shift:\n");
    printf("%f %f %f\n", dataset->origin_shift[0], dataset->origin_shift[1],
           dataset->origin_shift[2]);

    printf("\nStandardization\n");
    printf("Rigid rotation\n");
    for (s = 0; s < 3; s++) {
        printf("%f %f %f\n", dataset->std_rotation_matrix[s][0],
               dataset->std_rotation_matrix[s][1],
               dataset->std_rotation_matrix[s][2]);
    }
    printf("Lattice\n");
    for (s = 0; s < 3; s++) {
        printf("%f %f %f\n", dataset->std_lattice[s][0],
               dataset->std_lattice[s][1], dataset->std_lattice[s][2]);
    }
    printf("Positions, types, site tensors \n");
    for (i = 0; i < dataset->n_std_atoms; i++) {
        printf("[%f %f %f], types=%d, ", dataset->std_positions[i][0],
               dataset->std_positions[i][1], dataset->std_positions[i][2],
               dataset->std_types[i]);
        if (dataset->tensor_rank == 0) {
            printf("%f\n", dataset->std_tensors[i]);
        } else if (dataset->tensor_rank == 1) {
            printf("%f %f %f \n", dataset->std_tensors[i * 3],
                   dataset->std_tensors[i * 3 + 1],
                   dataset->std_tensors[i * 3 + 2]);
        }
    }
}

int sub_spg_standardize_cell(double lattice[3][3], double position[][3],
                             int types[], int const num_atom,
                             double const symprec, int const to_primitive,
                             int const no_idealize) {
    int i, num_primitive_atom, retval;
    double lat[3][3];
    double (*pos)[3];
    int *typ;

    pos = (double (*)[3])malloc(sizeof(double[3]) * num_atom);
    typ = (int *)malloc(sizeof(int) * num_atom);

    for (i = 0; i < 3; i++) {
        lat[i][0] = lattice[i][0];
        lat[i][1] = lattice[i][1];
        lat[i][2] = lattice[i][2];
    }

    for (i = 0; i < num_atom; i++) {
        pos[i][0] = position[i][0];
        pos[i][1] = position[i][1];
        pos[i][2] = position[i][2];
        typ[i] = types[i];
    }

    /* lattice, position, and types are overwritten. */
    num_primitive_atom = spg_standardize_cell(
        lat, pos, typ, num_atom, to_primitive, no_idealize, symprec);

    if (num_primitive_atom) {
        printf("VASP POSCAR format: ");
        if (to_primitive == 0) {
            printf("to_primitive=0 and ");
        } else {
            printf("to_primitive=1 and ");
        }

        if (no_idealize == 0) {
            printf("no_idealize=0\n");
        } else {
            printf("no_idealize=1\n");
        }
        printf("1.0\n");
        for (i = 0; i < 3; i++) {
            printf("%f %f %f\n", lat[0][i], lat[1][i], lat[2][i]);
        }
        printf("%d\n", num_primitive_atom);
        printf("Direct\n");
        for (i = 0; i < num_primitive_atom; i++) {
            printf("%f %f %f\n", pos[i][0], pos[i][1], pos[i][2]);
        }

        retval = 0;
    } else {
        retval = 1;
    }

    free(typ);
    typ = NULL;
    free(pos);
    pos = NULL;

    return retval;
}

int show_spg_dataset(double lattice[3][3], double const origin_shift[3],
                     double position[][3], int const num_atom,
                     int const types[]) {
    SpglibDataset *dataset;
    char ptsymbol[6];
    int pt_trans_mat[3][3];
    int i, j;
    int retval = 0;
    char const *wl = "abcdefghijklmnopqrstuvwxyz";

    for (i = 0; i < num_atom; i++) {
        for (j = 0; j < 3; j++) {
            position[i][j] += origin_shift[j];
        }
    }

    dataset = spg_get_dataset(lattice, position, types, num_atom, 1e-5);

    if (dataset == NULL) {
        retval = 1;
        goto end;
    }

    printf("International: %s (%d)\n", dataset->international_symbol,
           dataset->spacegroup_number);
    printf("Hall symbol:   %s\n", dataset->hall_symbol);
    if (spg_get_pointgroup(ptsymbol, pt_trans_mat, dataset->rotations,
                           dataset->n_operations)) {
        printf("Point group:   %s\n", ptsymbol);
        printf("Transformation matrix:\n");
        for (i = 0; i < 3; i++) {
            printf("%f %f %f\n", dataset->transformation_matrix[i][0],
                   dataset->transformation_matrix[i][1],
                   dataset->transformation_matrix[i][2]);
        }
        printf("Wyckoff letters:\n");
        for (i = 0; i < dataset->n_atoms; i++) {
            printf("%c ", wl[dataset->wyckoffs[i]]);
        }
        printf("\n");
        printf("Equivalent atoms:\n");
        for (i = 0; i < dataset->n_atoms; i++) {
            printf("%d ", dataset->equivalent_atoms[i]);
        }
        printf("\n");

        for (i = 0; i < dataset->n_operations; i++) {
            printf("--- %d ---\n", i + 1);
            for (j = 0; j < 3; j++) {
                printf("%2d %2d %2d\n", dataset->rotations[i][j][0],
                       dataset->rotations[i][j][1],
                       dataset->rotations[i][j][2]);
            }
            printf("%f %f %f\n", dataset->translations[i][0],
                   dataset->translations[i][1], dataset->translations[i][2]);
        }
    } else {
        retval = 1;
    }

    if (dataset) {
        spg_free_dataset(dataset);
    }

end:
    return retval;
}

#ifdef _MSC_VER
// https://stackoverflow.com/a/23616164
int setenv(char const *name, char const *value, int overwrite) {
    int errcode = 0;
    if (!overwrite) {
        size_t envsize = 0;
        errcode = getenv_s(&envsize, NULL, 0, name);
        if (errcode || envsize) return errcode;
    }
    return _putenv_s(name, value);
}
#endif
