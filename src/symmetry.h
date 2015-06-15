/* symmetry.h */
/* Copyright (C) 2008 Atsushi Togo */

#ifndef __symmetry_H__
#define __symmetry_H__

#include "cell.h"
#include "mathfunc.h"
#include "primitive.h"

typedef struct {
  int size;
  int (*rot)[3][3];
  double (*trans)[3];
} Symmetry;

typedef struct {
  int rot[48][3][3];
  int size;
} PointSymmetry;

Symmetry * sym_alloc_symmetry(const int size);
void sym_free_symmetry(Symmetry * symmetry);
Symmetry * sym_get_operation(SPGCONST Primitive * primitive);
Symmetry * sym_reduce_operation(SPGCONST Cell * primitive,
				SPGCONST Symmetry * symmetry,
				const double symprec);
VecDBL * sym_get_pure_translation(SPGCONST Cell *cell,
				   const double symprec);
VecDBL * sym_reduce_pure_translation(SPGCONST Cell * cell,
				      const VecDBL * pure_trans,
				      const double symprec);
void sym_set_angle_tolerance(double tolerance);
double sym_get_angle_tolerance(void);

#endif
