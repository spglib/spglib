// Copyright (C) 2008 Atsushi Togo
// This file is part of spglib.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef __mathfunc_H__
#define __mathfunc_H__

#include "base.h"

typedef struct {
    int size;
    int (*mat)[3][3];
} MatINT;

typedef struct {
    int size;
    double (*vec)[3];
} VecDBL;

double mat_get_determinant_d3(double const a[3][3]);
int mat_get_determinant_i3(int const a[3][3]);
int mat_get_trace_i3(int const a[3][3]);
SPG_API_TEST void mat_copy_matrix_d3(double a[3][3], double const b[3][3]);
void mat_copy_matrix_i3(int a[3][3], int const b[3][3]);
void mat_copy_vector_d3(double a[3], double const b[3]);
void mat_copy_vector_i3(int a[3], int const b[3]);
int mat_check_identity_matrix_i3(int const a[3][3], int const b[3][3]);
int mat_check_identity_matrix_d3(double const a[3][3], double const b[3][3],
                                 double const symprec);
int mat_check_identity_matrix_id3(int const a[3][3], double const b[3][3],
                                  double const symprec);
SPG_API_TEST void mat_multiply_matrix_d3(double m[3][3], double const a[3][3],
                                         double const b[3][3]);
void mat_multiply_matrix_i3(int m[3][3], int const a[3][3], int const b[3][3]);
void mat_multiply_matrix_di3(double m[3][3], double const a[3][3],
                             int const b[3][3]);
void mat_multiply_matrix_id3(double m[3][3], int const a[3][3],
                             double const b[3][3]);
void mat_multiply_matrix_vector_i3(int v[3], int const a[3][3], int const b[3]);
void mat_multiply_matrix_vector_d3(double v[3], double const a[3][3],
                                   double const b[3]);
void mat_multiply_matrix_vector_id3(double v[3], int const a[3][3],
                                    double const b[3]);
void mat_multiply_matrix_vector_di3(double v[3], double const a[3][3],
                                    int const b[3]);
void mat_add_matrix_i3(int m[3][3], int const a[3][3], int const b[3][3]);
void mat_cast_matrix_3i_to_3d(double m[3][3], int const a[3][3]);
SPG_API_TEST void mat_cast_matrix_3d_to_3i(int m[3][3], double const a[3][3]);
SPG_API_TEST int mat_inverse_matrix_d3(double m[3][3], double const a[3][3],
                                       double const precision);
int mat_get_similar_matrix_d3(double m[3][3], double const a[3][3],
                              double const b[3][3], double const precision);
void mat_transpose_matrix_d3(double a[3][3], double const b[3][3]);
void mat_transpose_matrix_i3(int a[3][3], int const b[3][3]);
SPG_API_TEST void mat_get_metric(double metric[3][3],
                                 double const lattice[3][3]);
double mat_norm_squared_d3(double const a[3]);
int mat_norm_squared_i3(int const a[3]);
void mat_cross_product_d3(double v[3], double const a[3], double const b[3]);
double mat_Dabs(double const a);
int mat_Nint(double const a);
double mat_Dmod1(double const a);
double mat_rem1(double const a);
MatINT *mat_alloc_MatINT(int const size);
void mat_free_MatINT(MatINT *matint);
VecDBL *mat_alloc_VecDBL(int const size);
void mat_free_VecDBL(VecDBL *vecdbl);
int mat_is_int_matrix(double const mat[3][3], double const symprec);

#endif
