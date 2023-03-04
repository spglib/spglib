#ifndef __test_utils_H__
#define __test_utils_H__

#include "spglib.h"

void show_symmetry_operations(const int (*rotations)[3][3],
                              const double (*translations)[3], const int size);
void show_cell(const double lattice[3][3], const double positions[][3],
               const int types[], const int num_atoms);

#endif  // __test_utils_H__
