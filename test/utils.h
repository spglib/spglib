#ifndef __test_utils_H__
#define __test_utils_H__

#include "spglib.h"

void show_symmetry_operations(const int (*rotations)[3][3],
                              const double (*translations)[3], const int size);
void show_matrix_3d(const double lattice[3][3]);
void show_cell(const double lattice[3][3], const double positions[][3],
               const int types[], const int num_atoms);
void show_spacegroup_type(const SpglibSpacegroupType spgtype);
void show_magnetic_symmetry_operations(const int (*rotations)[3][3],
                                       const double (*translations)[3],
                                       const int *time_reversals,
                                       const int size);
void show_magnetic_spacegroup_type(const SpglibMagneticSpacegroupType msgtype);
void show_spg_magnetic_dataset(const SpglibMagneticDataset *dataset);
int sub_spg_standardize_cell(double lattice[3][3], double position[][3],
                             int types[], const int num_atom,
                             const double symprec, const int to_primitive,
                             const int no_idealize);
int show_spg_dataset(double lattice[3][3], const double origin_shift[3],
                     double position[][3], const int num_atom,
                     const int types[]);

#ifdef _MSC_VER
// https://stackoverflow.com/a/23616164
int setenv(const char *name, const char *value, int overwrite);
#endif

#endif  // __test_utils_H__
