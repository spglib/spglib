/* Copyright (C) 2023 Atsushi Togo */
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

#include "niggli.h"

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

typedef struct {
    double A;
    double B;
    double C;
    double eta;
    double xi;
    double zeta;
    double eps;
    int l;
    int m;
    int n;
    double *tmat;
    double *lattice;
} NiggliParams;

static int get_num_attempts();
static NiggliParams *initialize(double const *lattice_, double const eps_);
static void finalize(double *lattice_, NiggliParams *p);
static int reset(NiggliParams *p);
static int step1(NiggliParams *p);
static int step2(NiggliParams *p);
static int step3(NiggliParams *p);
static int step4(NiggliParams *p);
static int step5(NiggliParams *p);
static int step6(NiggliParams *p);
static int step7(NiggliParams *p);
static int step8(NiggliParams *p);
static int set_parameters(NiggliParams *p);
static void set_angle_types(NiggliParams *p);
static double *get_transpose(double const *M);
static double *get_metric(double const *M);
static double *multiply_matrices(double const *A, double const *B);
static int layer_swap_axis(NiggliParams *p, int const aperiodic_axis);
static int step2_for_layer(NiggliParams *p);

#ifdef SPGDEBUG
static void debug_show(int const j, NiggliParams const *p) {
    /* int i; */

    if (j < 0) {
        printf("Finish: ");
    } else {
        printf("Step %d: ", j);
    }
    printf("%f %f %f %f %f %f\n", p->A, p->B, p->C, p->xi, p->eta, p->zeta);

    /* printf("%d %d %d\n", p->l, p->m, p->n); */
    /* for (i = 0; i < 3; i++) { */
    /*   printf("%f %f %f\n", */
    /*       p->lattice[i * 3], p->lattice[i * 3 + 1], p->lattice[i * 3 + 2]);
     */
    /* } */
}
#else
    #define debug_show(...)
#endif

int get_num_attempts() {
    char const *num_attempts_str = getenv("SPGLIB_NUM_ATTEMPTS");
    if (num_attempts_str != NULL) {
        // Try to parse the string as an integer
        char *end;
        long num_attempts = strtol(num_attempts_str, &end, 10);
        // If conversion fails end == num_attempts_str
        if (end != num_attempts_str && num_attempts > 0 &&
            num_attempts < INT_MAX)
            return (int)num_attempts;
        warning_print("Could not parse SPGLIB_NUM_ATTEMPTS=%s\n",
                      num_attempts_str);
    }
    // Otherwise return default number of attempts
    return 1000;
}

/*--------------------------------------------*/
/* Version: niggli-[major].[minor].[micro] */
/*--------------------------------------------*/
int niggli_get_major_version(void) { return NIGGLI_MAJOR_VERSION; }

int niggli_get_minor_version(void) { return NIGGLI_MINOR_VERSION; }

int niggli_get_micro_version(void) { return NIGGLI_MICRO_VERSION; }

/* return 0 if failed */
// For bulk, set `aperiodic_axis=-1`
// @note For layer group, Niggli steps are modified to maintain the non-periodic
// axis
//       - First, Move aperiodic axis to c
//       - Step 2 is skipped
//       - (Steps 5 and 6 are not modified because axis c is perpendicular to
//       the plane)
int niggli_reduce(double *lattice_, double const eps_,
                  int const aperiodic_axis) {
    int succeeded;
    NiggliParams *p;
    int (*steps[8])(NiggliParams *p) = {step1, step2, step3, step4,
                                        step5, step6, step7, step8};

    if (aperiodic_axis != -1) {
        steps[1] = step2_for_layer;
    }

    p = NULL;
    succeeded = 0;

    if ((p = initialize(lattice_, eps_)) == NULL) {
        return 0;
    }

    /* Step 0 */
    // Move aperiodic axis to c for layer
    if (!(((aperiodic_axis == 0 || aperiodic_axis == 1) &&
           layer_swap_axis(p, aperiodic_axis)) ||
          ((aperiodic_axis == -1 || aperiodic_axis == 2) &&
           set_parameters(p)))) {
        goto ret;
    }

    for (int i = 0, max_attempts = get_num_attempts(); i < max_attempts; i++) {
        int j = 0;
        for (; j < 8; j++) {
            if ((*steps[j])(p)) {
                debug_show(j + 1, p);
                if (!reset(p)) {
                    goto ret;
                }
                if (j == 1 || j == 4 || j == 5 || j == 6 || j == 7) {
                    break;
                }
            }
        }
        if (j == 8) {
            succeeded = 1;
            break;
        }
    }

    debug_show(-1, p);

ret:
    finalize(lattice_, p);
    return succeeded;
}

static NiggliParams *initialize(double const *lattice_, double const eps_) {
    NiggliParams *p;

    p = NULL;

    if ((p = (NiggliParams *)malloc(sizeof(NiggliParams))) == NULL) {
        warning_print("niggli: Memory could not be allocated.");
        return NULL;
    }

    p->A = 0;
    p->B = 0;
    p->C = 0;
    p->eta = 0;
    p->xi = 0;
    p->zeta = 0;
    p->eps = 0;
    p->l = 0;
    p->m = 0;
    p->n = 0;
    p->tmat = NULL;
    p->lattice = NULL;

    if ((p->tmat = (double *)malloc(sizeof(double) * 9)) == NULL) {
        warning_print("niggli: Memory could not be allocated.");
        free(p);
        p = NULL;
        return NULL;
    }

    p->eps = eps_;
    if ((p->lattice = (double *)malloc(sizeof(double) * 9)) == NULL) {
        warning_print("niggli: Memory could not be allocated.");
        free(p->tmat);
        p->tmat = NULL;
        free(p);
        p = NULL;
        return NULL;
    }

    memcpy(p->lattice, lattice_, sizeof(double) * 9);

    return p;
}

/* move aperiodic axis to c. */
static int layer_swap_axis(NiggliParams *p, int const aperiodic_axis) {
    if (aperiodic_axis == 0) {
        p->tmat[0] = 0, p->tmat[1] = 0, p->tmat[2] = -1;
        p->tmat[3] = 0, p->tmat[4] = -1, p->tmat[5] = 0;
        p->tmat[6] = -1, p->tmat[7] = 0, p->tmat[8] = 0;
    } else if (aperiodic_axis == 1) {
        p->tmat[0] = -1, p->tmat[1] = 0, p->tmat[2] = 0;
        p->tmat[3] = 0, p->tmat[4] = 0, p->tmat[5] = -1;
        p->tmat[6] = 0, p->tmat[7] = -1, p->tmat[8] = 0;
    }

    return reset(p);
}

static void finalize(double *lattice_, NiggliParams *p) {
    free(p->tmat);
    p->tmat = NULL;
    memcpy(lattice_, p->lattice, sizeof(double) * 9);
    free(p->lattice);
    p->lattice = NULL;
    free(p);
    p = NULL;
}

static int reset(NiggliParams *p) {
    double *lat_tmp;

    lat_tmp = NULL;

    if ((lat_tmp = multiply_matrices(p->lattice, p->tmat)) == NULL) {
        return 0;
    }
    memcpy(p->lattice, lat_tmp, sizeof(double) * 9);
    free(lat_tmp);
    lat_tmp = NULL;

    return set_parameters(p);
}

static int set_parameters(NiggliParams *p) {
    double *G;

    G = NULL;

    if ((G = get_metric(p->lattice)) == NULL) {
        return 0;
    }

    p->A = G[0];
    p->B = G[4];
    p->C = G[8];
    p->xi = G[5] * 2;
    p->eta = G[2] * 2;
    p->zeta = G[1] * 2;

    free(G);
    G = NULL;

    set_angle_types(p);

    return 1;
}

static void set_angle_types(NiggliParams *p) {
    p->l = 0;
    p->m = 0;
    p->n = 0;
    if (p->xi < -p->eps) {
        p->l = -1;
    }
    if (p->xi > p->eps) {
        p->l = 1;
    }
    if (p->eta < -p->eps) {
        p->m = -1;
    }
    if (p->eta > p->eps) {
        p->m = 1;
    }
    if (p->zeta < -p->eps) {
        p->n = -1;
    }
    if (p->zeta > p->eps) {
        p->n = 1;
    }
}

static int step1(NiggliParams *p) {
    if (p->A > p->B + p->eps || (!(fabs(p->A - p->B) > p->eps) &&
                                 fabs(p->xi) > fabs(p->eta) + p->eps)) {
        p->tmat[0] = 0, p->tmat[1] = -1, p->tmat[2] = 0;
        p->tmat[3] = -1, p->tmat[4] = 0, p->tmat[5] = 0;
        p->tmat[6] = 0, p->tmat[7] = 0, p->tmat[8] = -1;
        return 1;
    } else {
        return 0;
    }
}

static int step2(NiggliParams *p) {
    if (p->B > p->C + p->eps || (!(fabs(p->B - p->C) > p->eps) &&
                                 fabs(p->eta) > fabs(p->zeta) + p->eps)) {
        p->tmat[0] = -1, p->tmat[1] = 0, p->tmat[2] = 0;
        p->tmat[3] = 0, p->tmat[4] = 0, p->tmat[5] = -1;
        p->tmat[6] = 0, p->tmat[7] = -1, p->tmat[8] = 0;
        return 1;
    } else {
        return 0;
    }
}

/* Aperiodic axis is fixed to C, the output may not be a standard Niggli cell */
/* Nothing else should be affected, so only a warning */
static int step2_for_layer(NiggliParams *p) {
    if (p->B > p->C + p->eps || (!(fabs(p->B - p->C) > p->eps) &&
                                 fabs(p->eta) > fabs(p->zeta) + p->eps)) {
        info_print(
            "niggli: B > C or B = C and |eta| > |zeta|. Please elongate the "
            "aperiodic axis.");
    }
    return 0;
}

static int step3(NiggliParams *p) {
    int i, j, k;
    if (p->l * p->m * p->n == 1) {
        if (p->l == -1) {
            i = -1;
        } else {
            i = 1;
        }
        if (p->m == -1) {
            j = -1;
        } else {
            j = 1;
        }
        if (p->n == -1) {
            k = -1;
        } else {
            k = 1;
        }
        p->tmat[0] = i, p->tmat[1] = 0, p->tmat[2] = 0;
        p->tmat[3] = 0, p->tmat[4] = j, p->tmat[5] = 0;
        p->tmat[6] = 0, p->tmat[7] = 0, p->tmat[8] = k;
        return 1;
    } else {
        return 0;
    }
}

static int step4(NiggliParams *p) {
    int i, j, k, r;

    if (p->l == -1 && p->m == -1 && p->n == -1) {
        return 0;
    }

    if (p->l * p->m * p->n == 0 || p->l * p->m * p->n == -1) {
        i = 1;
        j = 1;
        k = 1;
        r = -1; /* 0: i, 1: j, 2: k */
        if (p->l == 1) {
            i = -1;
        }
        if (p->l == 0) {
            r = 0;
        }
        if (p->m == 1) {
            j = -1;
        }
        if (p->m == 0) {
            r = 1;
        }
        if (p->n == 1) {
            k = -1;
        }
        if (p->n == 0) {
            r = 2;
        }

        if (i * j * k == -1) {
            if (r == 0) {
                i = -1;
            }
            if (r == 1) {
                j = -1;
            }
            if (r == 2) {
                k = -1;
            }
        }

        p->tmat[0] = i, p->tmat[1] = 0, p->tmat[2] = 0;
        p->tmat[3] = 0, p->tmat[4] = j, p->tmat[5] = 0;
        p->tmat[6] = 0, p->tmat[7] = 0, p->tmat[8] = k;
        return 1;
    } else {
        return 0;
    }
}

static int step5(NiggliParams *p) {
    if (fabs(p->xi) > p->B + p->eps ||
        (!(fabs(p->B - p->xi) > p->eps) && 2 * p->eta < p->zeta - p->eps) ||
        (!(fabs(p->B + p->xi) > p->eps) && p->zeta < -p->eps)) {
        p->tmat[0] = 1, p->tmat[1] = 0, p->tmat[2] = 0;
        p->tmat[3] = 0, p->tmat[4] = 1, p->tmat[5] = 0;
        p->tmat[6] = 0, p->tmat[7] = 0, p->tmat[8] = 1;
        if (p->xi > 0) {
            p->tmat[5] = -1;
        }
        if (p->xi < 0) {
            p->tmat[5] = 1;
        }
        return 1;
    } else {
        return 0;
    }
}

static int step6(NiggliParams *p) {
    if (fabs(p->eta) > p->A + p->eps ||
        (!(fabs(p->A - p->eta) > p->eps) && 2 * p->xi < p->zeta - p->eps) ||
        (!(fabs(p->A + p->eta) > p->eps) && p->zeta < -p->eps)) {
        p->tmat[0] = 1, p->tmat[1] = 0, p->tmat[2] = 0;
        p->tmat[3] = 0, p->tmat[4] = 1, p->tmat[5] = 0;
        p->tmat[6] = 0, p->tmat[7] = 0, p->tmat[8] = 1;
        if (p->eta > 0) {
            p->tmat[2] = -1;
        }
        if (p->eta < 0) {
            p->tmat[2] = 1;
        }
        return 1;
    } else {
        return 0;
    }
}

static int step7(NiggliParams *p) {
    if (fabs(p->zeta) > p->A + p->eps ||
        (!(fabs(p->A - p->zeta) > p->eps) && 2 * p->xi < p->eta - p->eps) ||
        (!(fabs(p->A + p->zeta) > p->eps) && p->eta < -p->eps)) {
        p->tmat[0] = 1, p->tmat[1] = 0, p->tmat[2] = 0;
        p->tmat[3] = 0, p->tmat[4] = 1, p->tmat[5] = 0;
        p->tmat[6] = 0, p->tmat[7] = 0, p->tmat[8] = 1;
        if (p->zeta > 0) {
            p->tmat[1] = -1;
        }
        if (p->zeta < 0) {
            p->tmat[1] = 1;
        }
        return 1;
    } else {
        return 0;
    }
}

static int step8(NiggliParams *p) {
    if (p->xi + p->eta + p->zeta + p->A + p->B < -p->eps ||
        (!(fabs(p->xi + p->eta + p->zeta + p->A + p->B) > p->eps) &&
         2 * (p->A + p->eta) + p->zeta > p->eps)) {
        p->tmat[0] = 1, p->tmat[1] = 0, p->tmat[2] = 1;
        p->tmat[3] = 0, p->tmat[4] = 1, p->tmat[5] = 1;
        p->tmat[6] = 0, p->tmat[7] = 0, p->tmat[8] = 1;
        return 1;
    } else {
        return 0;
    }
}

static double *get_transpose(double const *M) {
    int i, j;
    double *M_T;

    M_T = NULL;

    if ((M_T = (double *)malloc(sizeof(double) * 9)) == NULL) {
        warning_print("niggli: Memory could not be allocated.");
        return NULL;
    }

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            M_T[i * 3 + j] = M[j * 3 + i];
        }
    }

    return M_T;
}

static double *get_metric(double const *M) {
    double *G, *M_T;

    G = NULL;
    M_T = NULL;

    if ((M_T = get_transpose(M)) == NULL) {
        return NULL;
    }
    if ((G = multiply_matrices(M_T, M)) == NULL) {
        return NULL;
    }

    free(M_T);
    M_T = NULL;

    return G;
}

static double *multiply_matrices(double const *L, double const *R) {
    int i, j, k;
    double *M;

    M = NULL;

    if ((M = (double *)malloc(sizeof(double) * 9)) == NULL) {
        warning_print("niggli: Memory could not be allocated.");
        return NULL;
    }

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            M[i * 3 + j] = 0;
            for (k = 0; k < 3; k++) {
                M[i * 3 + j] += L[i * 3 + k] * R[k * 3 + j];
            }
        }
    }

    return M;
}
