/* primitive.h */
/* Copyright (C) 2008 Atsushi Togo */

#ifndef __primitive_H__
#define __primitive_H__

#include "cell.h"
#include "mathfunc.h"

typedef struct {
  Cell *cell;
  int * mapping_table;
  int size;
  double tolerance;
} Primitive;

Primitive * prm_alloc_primitive(const int size);
void prm_free_primitive(Primitive * primitive);
Primitive * prm_get_primitive(SPGCONST Cell * cell, const double symprec);
Primitive * prm_transform_to_primitive(SPGCONST Cell * cell,
				       SPGCONST double trans_mat_Bravais[3][3],
				       const Centering centering,
				       const double symprec);

#endif
