#ifndef __test_utils_H__
#define __test_utils_H__

#include "spglib.h"

void show_symmetry_operations(int const (*rotations)[3][3],
                              double const (*translations)[3], int const size);
void show_matrix_3d(double const lattice[3][3]);
void show_cell(double const lattice[3][3], double const positions[][3],
               int const types[], int const num_atoms);
void show_spacegroup_type(SpglibSpacegroupType const spgtype);
void show_magnetic_symmetry_operations(int const (*rotations)[3][3],
                                       double const (*translations)[3],
                                       int const *time_reversals,
                                       int const size);
void show_magnetic_spacegroup_type(SpglibMagneticSpacegroupType const msgtype);
void show_spg_magnetic_dataset(SpglibMagneticDataset const *dataset);
int sub_spg_standardize_cell(double lattice[3][3], double position[][3],
                             int types[], int const num_atom,
                             double const symprec, int const to_primitive,
                             int const no_idealize);
int show_spg_dataset(double lattice[3][3], double const origin_shift[3],
                     double position[][3], int const num_atom,
                     int const types[]);

#ifdef _MSC_VER
// https://stackoverflow.com/a/23616164
int setenv(char const *name, char const *value, int overwrite);
#endif

#endif  // __test_utils_H__
