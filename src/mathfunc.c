// Copyright (C) 2008 Atsushi Togo
// This file is part of spglib.
// SPDX-License-Identifier: BSD-3-Clause

#include "mathfunc.h"

#include <stdio.h>
#include <stdlib.h>

#include "debug.h"

#define ZERO_PREC 1e-10

double mat_get_determinant_d3(double const a[3][3]) {
    return a[0][0] * (a[1][1] * a[2][2] - a[1][2] * a[2][1]) +
           a[0][1] * (a[1][2] * a[2][0] - a[1][0] * a[2][2]) +
           a[0][2] * (a[1][0] * a[2][1] - a[1][1] * a[2][0]);
}

int mat_get_determinant_i3(int const a[3][3]) {
    return a[0][0] * (a[1][1] * a[2][2] - a[1][2] * a[2][1]) +
           a[0][1] * (a[1][2] * a[2][0] - a[1][0] * a[2][2]) +
           a[0][2] * (a[1][0] * a[2][1] - a[1][1] * a[2][0]);
}

int mat_get_trace_i3(int const a[3][3]) { return a[0][0] + a[1][1] + a[2][2]; }

void mat_copy_matrix_d3(double a[3][3], double const b[3][3]) {
    a[0][0] = b[0][0];
    a[0][1] = b[0][1];
    a[0][2] = b[0][2];
    a[1][0] = b[1][0];
    a[1][1] = b[1][1];
    a[1][2] = b[1][2];
    a[2][0] = b[2][0];
    a[2][1] = b[2][1];
    a[2][2] = b[2][2];
}

void mat_copy_matrix_i3(int a[3][3], int const b[3][3]) {
    a[0][0] = b[0][0];
    a[0][1] = b[0][1];
    a[0][2] = b[0][2];
    a[1][0] = b[1][0];
    a[1][1] = b[1][1];
    a[1][2] = b[1][2];
    a[2][0] = b[2][0];
    a[2][1] = b[2][1];
    a[2][2] = b[2][2];
}

void mat_copy_vector_d3(double a[3], double const b[3]) {
    a[0] = b[0];
    a[1] = b[1];
    a[2] = b[2];
}

void mat_copy_vector_i3(int a[3], int const b[3]) {
    a[0] = b[0];
    a[1] = b[1];
    a[2] = b[2];
}

int mat_check_identity_matrix_i3(int const a[3][3], int const b[3][3]) {
    if (a[0][0] - b[0][0] || a[0][1] - b[0][1] || a[0][2] - b[0][2] ||
        a[1][0] - b[1][0] || a[1][1] - b[1][1] || a[1][2] - b[1][2] ||
        a[2][0] - b[2][0] || a[2][1] - b[2][1] || a[2][2] - b[2][2]) {
        return 0;
    } else {
        return 1;
    }
}

int mat_check_identity_matrix_d3(double const a[3][3], double const b[3][3],
                                 double const symprec) {
    if (mat_Dabs(a[0][0] - b[0][0]) > symprec ||
        mat_Dabs(a[0][1] - b[0][1]) > symprec ||
        mat_Dabs(a[0][2] - b[0][2]) > symprec ||
        mat_Dabs(a[1][0] - b[1][0]) > symprec ||
        mat_Dabs(a[1][1] - b[1][1]) > symprec ||
        mat_Dabs(a[1][2] - b[1][2]) > symprec ||
        mat_Dabs(a[2][0] - b[2][0]) > symprec ||
        mat_Dabs(a[2][1] - b[2][1]) > symprec ||
        mat_Dabs(a[2][2] - b[2][2]) > symprec) {
        return 0;
    } else {
        return 1;
    }
}

int mat_check_identity_matrix_id3(int const a[3][3], double const b[3][3],
                                  double const symprec) {
    if (mat_Dabs(a[0][0] - b[0][0]) > symprec ||
        mat_Dabs(a[0][1] - b[0][1]) > symprec ||
        mat_Dabs(a[0][2] - b[0][2]) > symprec ||
        mat_Dabs(a[1][0] - b[1][0]) > symprec ||
        mat_Dabs(a[1][1] - b[1][1]) > symprec ||
        mat_Dabs(a[1][2] - b[1][2]) > symprec ||
        mat_Dabs(a[2][0] - b[2][0]) > symprec ||
        mat_Dabs(a[2][1] - b[2][1]) > symprec ||
        mat_Dabs(a[2][2] - b[2][2]) > symprec) {
        return 0;
    } else {
        return 1;
    }
}

/* m=axb */
void mat_multiply_matrix_d3(double m[3][3], double const a[3][3],
                            double const b[3][3]) {
    int i, j; /* a_ij */
    double c[3][3];
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            c[i][j] = a[i][0] * b[0][j] + a[i][1] * b[1][j] + a[i][2] * b[2][j];
        }
    }
    mat_copy_matrix_d3(m, c);
}

void mat_multiply_matrix_i3(int m[3][3], int const a[3][3], int const b[3][3]) {
    int i, j; /* a_ij */
    int c[3][3];
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            c[i][j] = a[i][0] * b[0][j] + a[i][1] * b[1][j] + a[i][2] * b[2][j];
        }
    }
    mat_copy_matrix_i3(m, c);
}

void mat_multiply_matrix_di3(double m[3][3], double const a[3][3],
                             int const b[3][3]) {
    int i, j; /* a_ij */
    double c[3][3];
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            c[i][j] = a[i][0] * b[0][j] + a[i][1] * b[1][j] + a[i][2] * b[2][j];
        }
    }
    mat_copy_matrix_d3(m, c);
}

void mat_multiply_matrix_id3(double m[3][3], int const a[3][3],
                             double const b[3][3]) {
    int i, j; /* a_ij */
    double c[3][3];
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            c[i][j] = a[i][0] * b[0][j] + a[i][1] * b[1][j] + a[i][2] * b[2][j];
        }
    }
    mat_copy_matrix_d3(m, c);
}

void mat_multiply_matrix_vector_i3(int v[3], int const a[3][3],
                                   int const b[3]) {
    int i;
    int c[3];
    for (i = 0; i < 3; i++)
        c[i] = a[i][0] * b[0] + a[i][1] * b[1] + a[i][2] * b[2];
    for (i = 0; i < 3; i++) v[i] = c[i];
}

/* v[3] <- a[3][3] * b[3] */
void mat_multiply_matrix_vector_d3(double v[3], double const a[3][3],
                                   double const b[3]) {
    int i;
    double c[3];
    for (i = 0; i < 3; i++)
        c[i] = a[i][0] * b[0] + a[i][1] * b[1] + a[i][2] * b[2];
    for (i = 0; i < 3; i++) v[i] = c[i];
}

void mat_multiply_matrix_vector_id3(double v[3], int const a[3][3],
                                    double const b[3]) {
    int i;
    double c[3];
    for (i = 0; i < 3; i++)
        c[i] = a[i][0] * b[0] + a[i][1] * b[1] + a[i][2] * b[2];
    for (i = 0; i < 3; i++) v[i] = c[i];
}

void mat_multiply_matrix_vector_di3(double v[3], double const a[3][3],
                                    int const b[3]) {
    int i;
    double c[3];
    for (i = 0; i < 3; i++)
        c[i] = a[i][0] * b[0] + a[i][1] * b[1] + a[i][2] * b[2];
    for (i = 0; i < 3; i++) v[i] = c[i];
}

void mat_add_matrix_i3(int m[3][3], int const a[3][3], int const b[3][3]) {
    int i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            m[i][j] = a[i][j] + b[i][j];
        }
    }
}

void mat_cast_matrix_3i_to_3d(double m[3][3], int const a[3][3]) {
    m[0][0] = a[0][0];
    m[0][1] = a[0][1];
    m[0][2] = a[0][2];
    m[1][0] = a[1][0];
    m[1][1] = a[1][1];
    m[1][2] = a[1][2];
    m[2][0] = a[2][0];
    m[2][1] = a[2][1];
    m[2][2] = a[2][2];
}

void mat_cast_matrix_3d_to_3i(int m[3][3], double const a[3][3]) {
    m[0][0] = mat_Nint(a[0][0]);
    m[0][1] = mat_Nint(a[0][1]);
    m[0][2] = mat_Nint(a[0][2]);
    m[1][0] = mat_Nint(a[1][0]);
    m[1][1] = mat_Nint(a[1][1]);
    m[1][2] = mat_Nint(a[1][2]);
    m[2][0] = mat_Nint(a[2][0]);
    m[2][1] = mat_Nint(a[2][1]);
    m[2][2] = mat_Nint(a[2][2]);
}

/* m^-1 */
/* ruby code for auto generating */
/* 3.times {|i| 3.times {|j| */
/*       puts
 * "m[#{j}][#{i}]=(a[#{(i+1)%3}][#{(j+1)%3}]*a[#{(i+2)%3}][#{(j+2)%3}] */
/*       -a[#{(i+1)%3}][#{(j+2)%3}]*a[#{(i+2)%3}][#{(j+1)%3}])/det;" */
/* }} */
int mat_inverse_matrix_d3(double m[3][3], double const a[3][3],
                          double const precision) {
    double det;
    double c[3][3];
    det = mat_get_determinant_d3(a);
    if (mat_Dabs(det) < precision) {
        debug_print("spglib: No inverse matrix (det=%f)\n", det);
        return 0;
    }

    c[0][0] = (a[1][1] * a[2][2] - a[1][2] * a[2][1]) / det;
    c[1][0] = (a[1][2] * a[2][0] - a[1][0] * a[2][2]) / det;
    c[2][0] = (a[1][0] * a[2][1] - a[1][1] * a[2][0]) / det;
    c[0][1] = (a[2][1] * a[0][2] - a[2][2] * a[0][1]) / det;
    c[1][1] = (a[2][2] * a[0][0] - a[2][0] * a[0][2]) / det;
    c[2][1] = (a[2][0] * a[0][1] - a[2][1] * a[0][0]) / det;
    c[0][2] = (a[0][1] * a[1][2] - a[0][2] * a[1][1]) / det;
    c[1][2] = (a[0][2] * a[1][0] - a[0][0] * a[1][2]) / det;
    c[2][2] = (a[0][0] * a[1][1] - a[0][1] * a[1][0]) / det;
    mat_copy_matrix_d3(m, c);
    return 1;
}

/* m = b^-1 a b */
int mat_get_similar_matrix_d3(double m[3][3], double const a[3][3],
                              double const b[3][3], double const precision) {
    double c[3][3];
    if (!mat_inverse_matrix_d3(c, b, precision)) {
        debug_print("spglib: No similar matrix due to 0 determinant.\n");
        return 0;
    }
    mat_multiply_matrix_d3(m, a, b);
    mat_multiply_matrix_d3(m, c, m);
    return 1;
}

void mat_transpose_matrix_d3(double a[3][3], double const b[3][3]) {
    double c[3][3];
    c[0][0] = b[0][0];
    c[0][1] = b[1][0];
    c[0][2] = b[2][0];
    c[1][0] = b[0][1];
    c[1][1] = b[1][1];
    c[1][2] = b[2][1];
    c[2][0] = b[0][2];
    c[2][1] = b[1][2];
    c[2][2] = b[2][2];
    mat_copy_matrix_d3(a, c);
}

void mat_transpose_matrix_i3(int a[3][3], int const b[3][3]) {
    int c[3][3];
    c[0][0] = b[0][0];
    c[0][1] = b[1][0];
    c[0][2] = b[2][0];
    c[1][0] = b[0][1];
    c[1][1] = b[1][1];
    c[1][2] = b[2][1];
    c[2][0] = b[0][2];
    c[2][1] = b[1][2];
    c[2][2] = b[2][2];
    mat_copy_matrix_i3(a, c);
}

void mat_get_metric(double metric[3][3], double const lattice[3][3]) {
    double lattice_t[3][3];
    mat_transpose_matrix_d3(lattice_t, lattice);
    mat_multiply_matrix_d3(metric, lattice_t, lattice);
}

double mat_norm_squared_d3(double const a[3]) {
    return a[0] * a[0] + a[1] * a[1] + a[2] * a[2];
}

int mat_norm_squared_i3(int const a[3]) {
    return a[0] * a[0] + a[1] * a[1] + a[2] * a[2];
}

void mat_cross_product_d3(double v[3], double const a[3], double const b[3]) {
    v[0] = a[1] * b[2] - a[2] * b[1];
    v[1] = a[2] * b[0] - a[0] * b[2];
    v[2] = a[0] * b[1] - a[1] * b[0];
}

double mat_Dabs(double const a) {
    if (a < 0.0)
        return -a;
    else
        return a;
}

/* Round to a nearest integer */
int mat_Nint(double const a) {
    if (a < 0.0)
        return (int)(a - 0.5);
    else
        return (int)(a + 0.5);
}

double mat_Dmod1(double const a) {
    double b;
    b = a - mat_Nint(a);

    if (b < 0.0 - ZERO_PREC)
        return b + 1.0;
    else
        return b;
}

/// @brief Return the remainder of a divided by 1.
/// @param a
/// @return Remainder between -0.5 and 0.5
double mat_rem1(double const a) { return a - mat_Nint(a); }

MatINT *mat_alloc_MatINT(int const size) {
    MatINT *matint;

    matint = NULL;

    if ((matint = (MatINT *)malloc(sizeof(MatINT))) == NULL) {
        warning_memory("matint");
        return NULL;
    }

    matint->size = size;
    if (size > 0) {
        if ((matint->mat = (int (*)[3][3])malloc(sizeof(int[3][3]) * size)) ==
            NULL) {
            warning_memory("matint->mat");
            free(matint);
            matint = NULL;
            return NULL;
        }
    }
    return matint;
}

void mat_free_MatINT(MatINT *matint) {
    if (matint->size > 0) {
        free(matint->mat);
        matint->mat = NULL;
    }
    free(matint);
}

VecDBL *mat_alloc_VecDBL(int const size) {
    VecDBL *vecdbl;

    vecdbl = NULL;

    if ((vecdbl = (VecDBL *)malloc(sizeof(VecDBL))) == NULL) {
        warning_memory("vecdbl");
        return NULL;
    }

    vecdbl->size = size;
    if (size > 0) {
        if ((vecdbl->vec = (double (*)[3])malloc(sizeof(double[3]) * size)) ==
            NULL) {
            warning_memory("vecdbl->vec");
            free(vecdbl);
            vecdbl = NULL;
            return NULL;
        }
    }
    return vecdbl;
}

void mat_free_VecDBL(VecDBL *vecdbl) {
    if (vecdbl->size > 0) {
        free(vecdbl->vec);
        vecdbl->vec = NULL;
    }
    free(vecdbl);
}

int mat_is_int_matrix(double const mat[3][3], double const symprec) {
    int i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (mat_Dabs(mat_Nint(mat[i][j]) - mat[i][j]) > symprec) {
                return 0;
            }
        }
    }
    return 1;
}
