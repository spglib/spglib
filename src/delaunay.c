/* Copyright (C) 2010 Atsushi Togo */
/* All rights reserved. */

/* This file is part of spglib. */

/* Redistribution and use in source and binary forms, with or without */
/* modification, are permitted provided that the following conditions */
/* are met: */

/* * Redistributions of source code must retain the above copyright */
/*   notice, this list of conditions and the following disclaimer. */

/* * Redistributions in binary form must reproduce the above copyright */
/*   notice, this list of conditions and the following disclaimer in */
/*   the documentation and/or other materials provided with the */
/*   distribution. */

/* * Neither the name of the spglib project nor the names of its */
/*   contributors may be used to endorse or promote products derived */
/*   from this software without specific prior written permission. */

/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS */
/* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT */
/* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS */
/* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE */
/* COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, */
/* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, */
/* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; */
/* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER */
/* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT */
/* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN */
/* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE */
/* POSSIBILITY OF SUCH DAMAGE. */

#include "delaunay.h"

#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "mathfunc.h"

#define NUM_ATTEMPT 100
#define ZERO_PREC 1e-10

static int delaunay_reduce(double red_lattice[3][3], const double lattice[3][3],
                           const int aperiodic_axis, const double symprec);
static int delaunay_reduce_basis(double basis[4][3], const int lattice_rank,
                                 const double symprec);
static void get_delaunay_shortest_vectors(double basis[4][3],
                                          const int lattice_rank,
                                          const double symprec);
static int get_extended_basis(double basis[4][3], const double lattice[3][3],
                              const int aperiodic_axis);
static int delaunay_reduce_2D(double red_lattice[3][3],
                              const double lattice[3][3], const int unique_axis,
                              const int aperiodic_axis, const double symprec);
static int delaunay_reduce_basis_2D(double basis[3][3], const int lattice_rank,
                                    const double symprec);
static void get_delaunay_shortest_vectors_2D(double basis[3][3],
                                             const double unique_vec[3],
                                             const int lattice_rank,
                                             const double symprec);
static void get_extended_basis_2D(double basis[3][3],
                                  const double lattice[3][2]);

/* Return 0 if failed */
int del_delaunay_reduce(double min_lattice[3][3], const double lattice[3][3],
                        const double symprec) {
    debug_print("del_delaunay_reduce (tolerance = %f):\n", symprec);

    return delaunay_reduce(min_lattice, lattice, -1, symprec);
}

int del_delaunay_reduce_2D(double min_lattice[3][3], const double lattice[3][3],
                           const int unique_axis, const double symprec) {
    debug_print("del_delaunay_reduce_2D:\n");
    return delaunay_reduce_2D(min_lattice, lattice, unique_axis, -1, symprec);
}

int del_layer_delaunay_reduce_2D(double min_lattice[3][3],
                                 const double lattice[3][3],
                                 const int unique_axis,
                                 const int aperiodic_axis,
                                 const double symprec) {
    debug_print("del_layer_delaunay_reduce_2D:\n");
    return delaunay_reduce_2D(min_lattice, lattice, unique_axis, aperiodic_axis,
                              symprec);
}

/* Return 0 if failed */
int del_layer_delaunay_reduce(double min_lattice[3][3],
                              const double lattice[3][3],
                              const int aperiodic_axis, const double symprec) {
    debug_print("del_layer_delaunay_reduce (tolerance = %f):\n", symprec);

    return delaunay_reduce(min_lattice, lattice, aperiodic_axis, symprec);
}

/* Delaunay reduction */
/* Reference can be found in International table A. */
/* Return 0 if failed */
static int delaunay_reduce(double red_lattice[3][3], const double lattice[3][3],
                           const int aperiodic_axis, const double symprec) {
    int i, j, attempt, succeeded, lattice_rank;
    int tmp_mat_int[3][3];
    double volume;
    double orig_lattice[3][3], tmp_mat[3][3], basis[4][3];

    mat_copy_matrix_d3(orig_lattice, lattice);

    lattice_rank = get_extended_basis(basis, lattice, aperiodic_axis);

    for (attempt = 0; attempt < NUM_ATTEMPT; attempt++) {
        succeeded = delaunay_reduce_basis(basis, lattice_rank, symprec);
        if (succeeded) {
            break;
        }
    }

    if (!succeeded) {
        goto err;
    }

    get_delaunay_shortest_vectors(basis, lattice_rank, symprec);

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            red_lattice[i][j] = basis[j][i];
        }
    }
    /* move the aperiodic axis from b3 back to its original direction */
    if (lattice_rank == 2 && aperiodic_axis != 2) {
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                if (j == aperiodic_axis) {
                    red_lattice[i][j] = basis[2][i];
                    red_lattice[i][2] = basis[j][i];
                }
            }
        }
    }

    volume = mat_get_determinant_d3(red_lattice);
    if (mat_Dabs(volume) < symprec) {
        warning_print("spglib: Minimum lattice has no volume (line %d, %s).\n",
                      __LINE__, __FILE__);
        goto err;
    }

    if (volume < 0) {
        /* Flip axes */
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                red_lattice[i][j] = -red_lattice[i][j];
            }
        }
    }

    mat_inverse_matrix_d3(tmp_mat, red_lattice, symprec);
    mat_multiply_matrix_d3(tmp_mat, tmp_mat, orig_lattice);
    mat_cast_matrix_3d_to_3i(tmp_mat_int, tmp_mat);
    if (abs(mat_get_determinant_i3(tmp_mat_int)) != 1) {
        warning_print(
            "spglib: Determinant of Delaunay change of basis matrix "
            "has to be 1 or -1 (line %d, %s).\n",
            __LINE__, __FILE__);
        goto err;
    }

    return 1;

err:
    return 0;
}

static void get_delaunay_shortest_vectors(double basis[4][3],
                                          const int lattice_rank,
                                          const double symprec) {
    int i, j;
    double tmpmat[3][3], b[7][3], tmpvec[3];

    /* Search in the set {b1, b2, b1+b2, b3, b4, b2+b3, b3+b1} */
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++) {
            b[i][j] = basis[i][j];
        }
    }

    for (i = 0; i < 3; i++) {
        b[2][i] = basis[0][i] + basis[1][i];
    }

    for (i = 2; i < 4; i++) {
        for (j = 0; j < 3; j++) {
            b[i + 1][j] = basis[i][j];
        }
    }

    for (i = 0; i < 3; i++) {
        b[5][i] = basis[1][i] + basis[2][i];
    }
    for (i = 0; i < 3; i++) {
        b[6][i] = basis[2][i] + basis[0][i];
    }

    /* Bubble sort */
    if (lattice_rank == 3) {
        /* Uncomment to pass original POSCAR-074-ref
        for (i = 0; i < 3; i++) {
          b[2][i] = basis[2][i];
          b[3][i] = basis[3][i];
          b[4][i] = basis[0][i] + basis[1][i];
        }
        */
        for (i = 0; i < 6; i++) {
            for (j = 0; j < 6; j++) {
                if (mat_norm_squared_d3(b[j]) >
                    mat_norm_squared_d3(b[j + 1]) + ZERO_PREC) {
                    mat_copy_vector_d3(tmpvec, b[j]);
                    mat_copy_vector_d3(b[j], b[j + 1]);
                    mat_copy_vector_d3(b[j + 1], tmpvec);
                }
            }
        }
        /* find the shortest 2 vectors inside period plane and the shortest one
         * outside */
    } else {
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 2; j++) {
                if (mat_norm_squared_d3(b[j]) >
                    mat_norm_squared_d3(b[j + 1]) + ZERO_PREC) {
                    mat_copy_vector_d3(tmpvec, b[j]);
                    mat_copy_vector_d3(b[j], b[j + 1]);
                    mat_copy_vector_d3(b[j + 1], tmpvec);
                }
            }
        }
        for (i = 3; i < 6; i++) {
            for (j = 3; j < 6; j++) {
                if (mat_norm_squared_d3(b[j]) >
                    mat_norm_squared_d3(b[j + 1]) + ZERO_PREC) {
                    mat_copy_vector_d3(tmpvec, b[j]);
                    mat_copy_vector_d3(b[j], b[j + 1]);
                    mat_copy_vector_d3(b[j + 1], tmpvec);
                }
            }
        }
    }

    /* for layer, i = 2, |tmpmat| = 0 is automatically dropped */
    for (i = 2; i < 7; i++) {
        for (j = 0; j < 3; j++) {
            tmpmat[j][0] = b[0][j];
            tmpmat[j][1] = b[1][j];
            tmpmat[j][2] = b[i][j];
        }
        if (mat_Dabs(mat_get_determinant_d3(tmpmat)) > symprec) {
            for (j = 0; j < 3; j++) {
                basis[0][j] = b[0][j];
                basis[1][j] = b[1][j];
                basis[2][j] = b[i][j];
            }
            break;
        }
    }
}

static int delaunay_reduce_basis(double basis[4][3], const int lattice_rank,
                                 const double symprec) {
    int i, j, k, l;
    double dot_product;

    for (i = 0; i < 3; i++) {
        for (j = i + 1; j < 4; j++) {
            dot_product = 0.0;
            for (k = 0; k < 3; k++) {
                dot_product += basis[i][k] * basis[j][k];
            }
            if (dot_product > symprec) {
                if (i < lattice_rank) {
                    for (k = 0; k < 4; k++) {
                        if (!(k == i || k == j)) {
                            for (l = 0; l < 3; l++) {
                                basis[k][l] += basis[i][l];
                            }
                        }
                    }
                    for (k = 0; k < 3; k++) {
                        basis[i][k] = -basis[i][k];
                    }
                    return 0;
                } else {
                    /* For layer, if b3 lies in a sphere with center -(b1+b2)/2
                     * and radius |b1+b2|/2, */
                    /* dot(b3, b4) <= 0 will be impossible. This should not
                     * affect the final results */
                    /* except the primitive cell is not a standard Delaunay
                     * cell, so just a warning. */
                    warning_print(
                        "spglib: Dot product between basis %d, %d larger than "
                        "0 (line %d, %s).\n",
                        i + 1, j + 1, __LINE__, __FILE__);
                    debug_print_vectors_d3(basis, 4);
                }
            }
        }
    }

    return 1;
}

/* aperiodic_axis (if exists) is temporarily moved to b3 */
static int get_extended_basis(double basis[4][3], const double lattice[3][3],
                              const int aperiodic_axis) {
    int i, j, lattice_rank;

    if (aperiodic_axis == -1) {
        lattice_rank = 3;
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                basis[i][j] = lattice[j][i];
            }
        }
    } else {
        lattice_rank = 0;
        for (i = 0; i < 3; i++) {
            if (i != aperiodic_axis) {
                for (j = 0; j < 3; j++) {
                    basis[lattice_rank][j] = lattice[j][i];
                }
                lattice_rank++;
            }
        }

        for (j = 0; j < 3; j++) {
            basis[lattice_rank][j] = lattice[j][aperiodic_axis];
        }
    }

    for (i = 0; i < 3; i++) {
        basis[3][i] = -lattice[i][0] - lattice[i][1] - lattice[i][2];
    }

    return lattice_rank;
}

/* For Monocli/Oblique, unique axis is aperiodic axis, j and k are periodic */
/* For Monocli/Rectang, k is aperiodic axis, unique axis and j are periodic */
/* j and k are delaunay reduced, which can be incomplete for Monocli/Rectang */
static int delaunay_reduce_2D(double red_lattice[3][3],
                              const double lattice[3][3], const int unique_axis,
                              const int aperiodic_axis, const double symprec) {
    int i, j, k, attempt, succeeded, lattice_rank;
    double volume;
    double basis[3][3], lattice_2D[3][2], unique_vec[3];

    if (aperiodic_axis == -1 || unique_axis == aperiodic_axis) {
        j = -1;
        k = -1;
        for (i = 0; i < 3; i++) {
            if (i != unique_axis) {
                if (j == -1) {
                    j = i;
                } else {
                    k = i;
                }
            }
        }
        lattice_rank = 2;
    } else {
        for (i = 0; i < 3; i++) {
            if (i != unique_axis && i != aperiodic_axis) {
                j = i;
            }
        }
        k = aperiodic_axis;
        lattice_rank = 1;
    }

    for (i = 0; i < 3; i++) {
        unique_vec[i] = lattice[i][unique_axis];
        lattice_2D[i][0] = lattice[i][j];
        lattice_2D[i][1] = lattice[i][k];
    }

    get_extended_basis_2D(basis, lattice_2D);

    for (attempt = 0; attempt < NUM_ATTEMPT; attempt++) {
        succeeded = delaunay_reduce_basis_2D(basis, lattice_rank, symprec);
        if (succeeded) {
            break;
        }
    }

    if (!succeeded) {
        goto err;
    }

    get_delaunay_shortest_vectors_2D(basis, unique_vec, lattice_rank, symprec);

    for (i = 0; i < 3; i++) {
        red_lattice[i][unique_axis] = lattice[i][unique_axis];
        ;
        red_lattice[i][j] = basis[0][i];
        red_lattice[i][k] = basis[1][i];
    }

    volume = mat_get_determinant_d3(red_lattice);
    if (mat_Dabs(volume) < symprec) {
        warning_print("spglib: Minimum lattice has no volume (line %d, %s).\n",
                      __LINE__, __FILE__);
        goto err;
    }

    if (volume < 0) {
        for (i = 0; i < 3; i++) {
            red_lattice[i][unique_axis] = -red_lattice[i][unique_axis];
        }
    }

    return 1;

err:
    return 0;
}

static int delaunay_reduce_basis_2D(double basis[3][3], const int lattice_rank,
                                    const double symprec) {
    int i, j, k, l;
    double dot_product;

    for (i = 0; i < 2; i++) {
        for (j = i + 1; j < 3; j++) {
            dot_product = 0.0;
            for (k = 0; k < 3; k++) {
                dot_product += basis[i][k] * basis[j][k];
            }
            if (dot_product > symprec) {
                if (i < lattice_rank) {
                    for (k = 0; k < 3; k++) {
                        if (!(k == i || k == j)) {
                            for (l = 0; l < 3; l++) {
                                basis[k][l] += 2 * basis[i][l];
                            }
                            break;
                        }
                    }
                    for (k = 0; k < 3; k++) {
                        basis[i][k] = -basis[i][k];
                    }
                    return 0;
                } else {
                    warning_print(
                        "spglib: Dot product between basis %d, %d larger than "
                        "0 (line %d, %s).\n",
                        i + 1, j + 1, __LINE__, __FILE__);
                    debug_print_vectors_d3(basis, 3);
                }
            }
        }
    }

    return 1;
}

static void get_delaunay_shortest_vectors_2D(double basis[3][3],
                                             const double unique_vec[3],
                                             const int lattice_rank,
                                             const double symprec) {
    int i, j;
    double b[4][3], tmpmat[3][3];
    double tmpvec[3];

    /* Search in the set {b1, b2, b3, b1+b2} */
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            b[i][j] = basis[i][j];
        }
    }

    for (i = 0; i < 3; i++) {
        b[3][i] = basis[0][i] + basis[1][i];
    }

    /* Bubble sort */
    for (i = lattice_rank % 2; i < 3; i++) {
        for (j = lattice_rank % 2; j < 3; j++) {
            if (mat_norm_squared_d3(b[j]) >
                mat_norm_squared_d3(b[j + 1]) + ZERO_PREC) {
                mat_copy_vector_d3(tmpvec, b[j]);
                mat_copy_vector_d3(b[j], b[j + 1]);
                mat_copy_vector_d3(b[j + 1], tmpvec);
            }
        }
    }

    for (i = 0; i < 3; i++) {
        tmpmat[i][0] = b[0][i];
        tmpmat[i][1] = unique_vec[i];
    }

    for (i = 1; i < 4; i++) {
        for (j = 0; j < 3; j++) {
            tmpmat[j][2] = b[i][j];
        }
        if (mat_Dabs(mat_get_determinant_d3(tmpmat)) > symprec) {
            for (j = 0; j < 3; j++) {
                basis[0][j] = b[0][j];
                basis[1][j] = b[i][j];
            }
            break;
        }
    }
}

static void get_extended_basis_2D(double basis[3][3],
                                  const double lattice[3][2]) {
    int i, j;

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++) {
            basis[i][j] = lattice[j][i];
        }
    }

    for (i = 0; i < 3; i++) {
        basis[2][i] = -lattice[i][0] - lattice[i][1];
    }
}
