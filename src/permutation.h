/* Copyright (C) 2017 Atsushi Togo */
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

/* * Neither the name of the phonopy project nor the names of its */
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

#include "mathfunc.h"
#include "cell.h"

void * perm_argsort_work_malloc(int n);
void perm_argsort_work_free(void * work);

int perm_argsort(int * perm,
                 const int * types,
                 const double * data,
                 void * work,
                 int n);

void perm_permute(void * data_out,
                  const void * data_in,
                  const int * perm,
                  int value_size,
                  int n);

void perm_compose(int * out,
                  const int * first,
                  const int * second,
                  int n);

void perm_inverse(int *out,
                  const int *perm,
                  int n);

/* Note: data_out and data_in MUST NOT ALIAS. */
static inline void perm_permute_int(int *data_out,
                                    const int *data_in,
                                    const int *perm,
                                    const int n) {
    perm_permute(data_out, data_in, perm, sizeof(int), n);
}

/* Note: data_out and data_in MUST NOT ALIAS. */
static inline void perm_permute_double(double *data_out,
                                       const double *data_in,
                                       const int *perm,
                                       const int n) {
    perm_permute(data_out, data_in, perm, sizeof(double), n);
}

/* Note: data_out and data_in MUST NOT ALIAS. */
static inline void perm_permute_double_3(double (*data_out)[3],
                                         SPGCONST double (*data_in)[3],
                                         const int *perm,
                                         const int n) {
    perm_permute(data_out, data_in, perm, sizeof(double[3]), n);
}

/* Contains pre-allocated memory and precomputed data for perm_finder_check_total_overlap. */
typedef struct {
    int size;

    /* Pre-allocated memory for various things. */
    void * argsort_work;
    void * blob;

    /* Temp areas for writing positions. (points into blob) */
    double (*pos_temp_1)[3];
    double (*pos_temp_2)[3];

    /* Temp area for writing lattice point distances. (points into blob) */
    double * distance_temp;

    /* Areas for permutations. (points into blob) */
    int * perm_orig; /* perm that sorted the original cell */
    int * perm_rot;  /* temporary used mainly to hold the perm that sorts the rotated cell */
    int * perm_temp_1; /* temporary */
    int * perm_temp_2; /* temporary */

    /* Sorted data of original cell. (points into blob)*/
    double (*lattice)[3];
    double (*pos_sorted)[3];
    int * types_sorted;
} PermFinder;

void perm_finder_free(PermFinder * searcher);

PermFinder* perm_finder_init(const Cell * cell);

int perm_finder_check_total_overlap(PermFinder * searcher,
                                    const double test_trans[3],
                                    SPGCONST int rot[3][3],
                                    double symprec,
                                    int is_identity);

int perm_finder_find_perm(int * perm,
                          PermFinder * searcher,
                          const double test_trans[3],
                          SPGCONST int rot[3][3],
                          double symprec,
                          int is_identity);
