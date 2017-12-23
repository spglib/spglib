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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "permutation.h"
#include "mathfunc.h"
#include "math.h"
#include "debug.h"

/* 'a++' generalized to an arbitrary increment. */
/* Performs 'a += b' and returns the old value of a. */
#ifndef SPG_POST_INCREMENT
#define SPG_POST_INCREMENT(a, b) (a += (b), a - (b))
#endif

static int argsort_by_lattice_point_distance(int * perm,
                                             SPGCONST double lattice[3][3],
                                             SPGCONST double (* positions)[3],
                                             const int * types,
                                             double * distance_temp,
                                             void *argsort_work,
                                             int size);

static PermFinder* perm_finder_alloc(int size);

static int find_perm_near_identity(int *perm,
                                   SPGCONST double lattice[3][3],
                                   SPGCONST double (*pos_original)[3],
                                   SPGCONST double (*pos_rotated)[3],
                                   int num_pos,
                                   double symprec);

/* Helper type used to get sorted indices of values. */
typedef struct {
	double value;
	int type;
	int index;
} ValueWithIndex;

static int ValueWithIndex_comparator(const void *pa, const void *pb)
{
    int cmp;
    ValueWithIndex a, b;

    a = *((ValueWithIndex*) pa);
    b = *((ValueWithIndex*) pb);

    /* order by atom type, then by value */
    cmp = (b.type < a.type) - (a.type < b.type);
    if (!cmp)
        cmp = (b.value < a.value) - (a.value < b.value);

    return cmp;
}

void* perm_argsort_work_malloc(int n)
{
    ValueWithIndex *work;

    if ((work = (ValueWithIndex*)(malloc(sizeof(ValueWithIndex) * n))) == NULL) {
        warning_print("spglib: Memory could not be allocated for argsort workspace.");
        return NULL;
    }
    return work;
}

void perm_argsort_work_free(void *work)
{
    free(work);
}

/* Compute a permutation that sorts values first by atom type, */
/* and then by value.  If types is NULL, all atoms are assumed */
/* to have the same type. */
/* */
/* Returns 0 on failure. */
int perm_argsort(int *perm,
                 const int *types,
                 const double *values,
                 void *provided_work,
                 const int n)
{
    int i;
    ValueWithIndex *work;

    if (provided_work) {
        work = (ValueWithIndex *) provided_work;
    } else if ((work = perm_argsort_work_malloc(n)) == NULL) {
        return 0;
    }

    /* Make array of all data for each value. */
    for (i = 0; i < n; i++) {
        work[i].value = values[i];
        work[i].index = i;
        work[i].type = types ? types[i] : 0;
    }

    /* Sort by type and value. */
    qsort(work, n, sizeof(ValueWithIndex), &ValueWithIndex_comparator);

    /* Retrieve indices.  This is the permutation. */
    for (i = 0; i < n; i++) {
        perm[i] = work[i].index;
    }

    if (!provided_work) {
        perm_argsort_work_free(work);
        work = NULL;
    }

    return 1;
}

/* Permute an array. */
/* data_out and data_in MUST NOT ALIAS. */
void perm_permute(void *data_out,
                  const void *data_in,
                  const int *perm,
                  const int value_size,
                  const int n)
{
	int i;
	const void *read;
  void *write;

	for (i = 0; i < n; i++) {
		read = data_in + perm[i] * value_size;
		write = data_out + i * value_size;
		memcpy(write, read, value_size);
	}
}

/* Construct the permutation that is equivalent to permuting by 'first', */
/* then permuting by 'second'. */
/* 'out' MUST NOT ALIAS with either 'first' or 'second'. */
void perm_compose(int *out,
                  const int *first,
                  const int *second,
                  const int n) {
  perm_permute_int(out, first, second, n);
}


/* Compute the inverse of 'perm'. */
/* 'perm' and 'out' MUST NOT ALIAS. */
void perm_inverse(int *out,
                  const int *perm,
                  const int n)
{
  int i;
  for (i = 0; i < n; i++)
    out[perm[i]] = i;
}

/* ***************************************** */
/*             Perm finder                   */

static PermFinder* perm_finder_alloc(int size)
{
  int offset_pos_temp_1, offset_pos_temp_2, offset_distance_temp;
  int offset_perm_orig, offset_perm_rot, offset_perm_temp_1, offset_perm_temp_2;
  int offset_pos_sorted, offset_types_sorted, offset_lattice;
  int offset, blob_size;

  PermFinder * searcher;
  searcher = NULL;

  offset = 0;
  offset_pos_temp_1 = SPG_POST_INCREMENT(offset, size * sizeof(double[3]));
  offset_pos_temp_2 = SPG_POST_INCREMENT(offset, size * sizeof(double[3]));
  offset_distance_temp = SPG_POST_INCREMENT(offset, size * sizeof(double));
  offset_perm_orig = SPG_POST_INCREMENT(offset, size * sizeof(int));
  offset_perm_rot = SPG_POST_INCREMENT(offset, size * sizeof(int));
  offset_perm_temp_1 = SPG_POST_INCREMENT(offset, size * sizeof(int));
  offset_perm_temp_2 = SPG_POST_INCREMENT(offset, size * sizeof(int));
  offset_lattice = SPG_POST_INCREMENT(offset, 9 * sizeof(double));
  offset_pos_sorted = SPG_POST_INCREMENT(offset, size * sizeof(double[3]));
  offset_types_sorted =  SPG_POST_INCREMENT(offset, size * sizeof(int));
  blob_size = offset;

  if ((searcher = (PermFinder*)malloc(sizeof(PermFinder))) == NULL) {
    warning_print("spglib: Memory could not be allocated for searcher.");
    return NULL;
  }

  if ((searcher->blob = malloc(blob_size)) == NULL) {
    warning_print("spglib: Memory could not be allocated for searcher.");
    free(searcher);
    return NULL;
  }

  if ((searcher->argsort_work = perm_argsort_work_malloc(size)) == NULL) {
    free(searcher->blob);
    free(searcher);
    return NULL;
  }

  searcher->size = size;
  searcher->pos_temp_1 = (double (*)[3])(searcher->blob + offset_pos_temp_1);
  searcher->pos_temp_2 = (double (*)[3])(searcher->blob + offset_pos_temp_2);
  searcher->distance_temp = (double *)(searcher->blob + offset_distance_temp);
  searcher->perm_orig = (int *)(searcher->blob + offset_perm_orig);
  searcher->perm_rot = (int *)(searcher->blob + offset_perm_rot);
  searcher->perm_temp_1 = (int *)(searcher->blob + offset_perm_temp_1);
  searcher->perm_temp_2 = (int *)(searcher->blob + offset_perm_temp_2);
  searcher->lattice = (double (*)[3])(searcher->blob + offset_lattice);
  searcher->pos_sorted  = (double (*)[3])(searcher->blob + offset_pos_sorted);
  searcher->types_sorted = (int *)(searcher->blob + offset_types_sorted);

  return searcher;
}

void perm_finder_free(PermFinder *tester)
{
  if (tester != NULL) {
    if (tester->argsort_work != NULL) {
      free(tester->argsort_work);
      tester->argsort_work = NULL;
    }
    if (tester->blob != NULL) {
      free(tester->blob);
      tester->blob = NULL;
    }
    free(tester);
  }
}

PermFinder* perm_finder_init(const Cell *cell)
{
  PermFinder * searcher;
  searcher = NULL;

  /* Allocate */
  if ((searcher = perm_finder_alloc(cell->size)) == NULL) {
    return NULL;
  }

  mat_copy_matrix_d3(searcher->lattice, cell->lattice);

  /* Get the permutation that sorts the original cell. */
  /* (This is saved for as long as the PermFinder lives.) */
  if (!argsort_by_lattice_point_distance(searcher->perm_orig,
                                         cell->lattice,
                                         cell->position,
                                         cell->types,
                                         searcher->distance_temp,
                                         searcher->argsort_work,
                                         searcher->size)) {
    perm_finder_free(searcher);
    return NULL;
  }

  /* Write the sorted cell data. */
  /* (This is saved for as long as the PermFinder lives.) */
  perm_permute_double_3(searcher->pos_sorted,
                        cell->position,
                        searcher->perm_orig,
                        cell->size);

  perm_permute_int(searcher->types_sorted,
                   cell->types,
                   searcher->perm_orig,
                   cell->size);

  return searcher;
}

static int argsort_by_lattice_point_distance(int * perm,
                                             SPGCONST double lattice[3][3],
                                             SPGCONST double (* positions)[3],
                                             const int * types,
                                             double * distance_temp,
                                             void *argsort_work,
                                             const int size)
{
  double diff[3];
  int i, k;
  double x;

  /* Fill distance_temp with distances. */
  for (i = 0; i < size; i++) {
    /* Fractional vector to nearest lattice point. */
    for (k = 0; k < 3; k++) {
      x = positions[i][k];
      diff[k] = x - mat_Nint(x);
    }

    /* Squared distance to lattice point. */
    mat_multiply_matrix_vector_d3(diff, lattice, diff);
    distance_temp[i] = mat_norm_squared_d3(diff);
  }

  return perm_argsort(perm,
                      types,
                      distance_temp,
                      argsort_work,
                      size);
}

/* Tests if an operator COULD be a symmetry of the cell, */
/* without the cost of sorting the rotated positions. */
/* It only inspects a few atoms. */
/* 0:  Not a symmetry.   1. Possible symmetry. */
static int perm_finder_check_possible_overlap(PermFinder *searcher,
                                              const double test_trans[3],
                                              SPGCONST int rot[3][3],
                                              const double symprec)
{
  double pos_rot[3];
  int i, i_test, k, max_search_num, search_num;
  int type_rot, is_found;

  max_search_num = 3;
  search_num = searcher->size <= max_search_num ? searcher->size
                                                : max_search_num;

  /* Check a few rotated positions. */
  /* (this could be optimized by focusing on the min_atom_type) */
  for (i_test = 0; i_test < search_num; i_test++) {

    type_rot = searcher->types_sorted[i_test];
    mat_multiply_matrix_vector_id3(pos_rot, rot, searcher->pos_sorted[i_test]);
    for (k = 0; k < 3; k++) {
      pos_rot[k] += test_trans[k];
    }

    /* Brute-force search for the rotated position. */
    /* (this could be optimized by saving the sorted ValueWithIndex data */
    /*  for the original Cell and using it to binary search for lower and */
    /*  upper bounds on 'i'. For now though, brute force is good enough) */
    is_found = 0;
    for (i = 0; i < searcher->size; i++) {
      if (cel_is_overlap_with_same_type(pos_rot,
                                        searcher->pos_sorted[i],
                                        type_rot,
                                        searcher->types_sorted[i],
                                        searcher->lattice,
                                        symprec)) {
        is_found = 1;
        break;
      }
    }

    /* The rotated position is not in the structure! */
    /* This symmetry operator is therefore clearly invalid. */
    if (!is_found) {
      return 0;
    }
  }

  return 1;
}

/* Uses a PermFinder to efficiently--but thoroughly--confirm that a given symmetry operator */
/* is a symmetry of the cell. If you need to test many symmetry operators on the same cell, */
/* you can create one PermFinder from the Cell and call this function many times. */
/* -1: Error.  0:  Not a symmetry.   1. Is a symmetry. */
int perm_finder_check_total_overlap(PermFinder *searcher,
                                    const double test_trans[3],
                                    int rot[3][3],
                                    const double symprec,
                                    const int is_identity)
{
  /* Check a few atoms by brute force before continuing. */
  /* This may allow us to avoid the sorting step for many incorrect translations. */
  if (!perm_finder_check_possible_overlap(searcher,
                                          test_trans,
                                          rot,
                                          symprec)) {
    return 0;
  }

  /* Perform the brute force search for a permutation between sorted data, */
  /* but don't bother recording it; just return success or failure. */
  return perm_finder_find_perm(NULL, /* Don't write the perm anywhere */
                               searcher,
                               test_trans,
                               rot,
                               symprec,
                               is_identity);
}

/* Common implementation for check_total_overlap and find_perm. */
/* -1: Error */
/* 0: The rotation is not a symmetry operator. (perm is not written) */
/* 1: The rotation is a symmetry operator. (perm was written if not NULL) */
int perm_finder_find_perm(int * perm, /* can be NULL to write nothing */
                          PermFinder *tester,
                          const double test_trans[3],
                          SPGCONST int rot[3][3],
                          const double symprec,
                          const int is_identity)
{
  double pos_rot[3], *ppos_rot;
  int i, k;

  /* Write rotated positions to 'pos_temp_1' */
  for (i = 0; i < tester->size; i++) {
    if (is_identity) {
      ppos_rot = tester->pos_sorted[i];
    } else {
      mat_multiply_matrix_vector_id3(pos_rot, rot, tester->pos_sorted[i]);
      ppos_rot = (double*)&pos_rot;
    }

    for (k = 0; k < 3; k++) {
      tester->pos_temp_1[i][k] = ppos_rot[k] + test_trans[k];
    }
  }

  /* Get permutation that sorts these positions. */
  /* (Stored in 'perm_rot' until the end of this function call) */
  if (!argsort_by_lattice_point_distance(tester->perm_rot,
                                         tester->lattice,
                                         tester->pos_temp_1,
                                         tester->types_sorted,
                                         tester->distance_temp,
                                         tester->argsort_work,
                                         tester->size)) {
    return -1;
  }

  /* Write sorted positions to 'pos_temp_2'. */
  perm_permute_double_3(tester->pos_temp_2,
                        tester->pos_temp_1,
                        tester->perm_rot,
                        tester->size);

  /* Compute perm between sorted coordinates. */
  if (!find_perm_near_identity(tester->perm_temp_1,
                               tester->lattice,
                               tester->pos_sorted, /* pos_original */
                               tester->pos_temp_2, /* pos_rotated */
                               tester->size,
                               symprec)) {
    return 0;
  }

  if (perm != NULL) {
    /* Compute total permutation. */
    /* It is the result of applying the following four permutations in order: */
    /*  - 'perm_orig' (the perm that sorted the original cell) */
    /*  - 'perm_temp_1' (the perm between the sorted coords) */
    /*  - 'inv(perm_rot)' (the perm that undoes the sorting of the rotated cell) */
    /*  - 'inv(perm_orig)' */
    /* The last inverse perm is because the rotated positions that we wrote to 'pos_temp_1' */
    /* were constructed from the sorted original positions rather than their original order. */
    perm_compose(tester->perm_temp_2, tester->perm_orig, tester->perm_temp_1, tester->size);

    /* NOTE: perm_temp_1 is reused for (perm_orig . perm_rot) ... */
    perm_compose(tester->perm_temp_1, tester->perm_orig, tester->perm_rot, tester->size);

    /* NOTE: perm_rot is reused for inv(perm_orig . perm_rot) ... */
    perm_inverse(tester->perm_rot, tester->perm_temp_1, tester->size);

    perm_compose(perm, tester->perm_temp_2, tester->perm_rot, tester->size);
  }

  return 1;
}

/* Optimized for the case where the max difference in index */
/* between pos_original and pos_rotated is small. */
static int find_perm_near_identity(int *perm,
                                   SPGCONST double lattice[3][3],
                                   SPGCONST double (*pos_original)[3],
                                   SPGCONST double (*pos_rotated)[3],
                                   const int num_pos,
                                   const double symprec)
{
  int i, k, i_orig, i_rot;
  int search_start;
  double diff_frac[3], diff_cart[3];

  /* perm[i] = index of position in pos_original that overlaps with pos_rotated[i] */
  for (i = 0; i < num_pos; i++) {
    perm[i] = -1;
  }

  search_start = 0;
  for (i_orig = 0; i_orig < num_pos; i_orig++) {

    /* Permanently skip positions filled near the beginning. */
    while (perm[search_start] >= 0) {
      search_start++;
    }

    for (i_rot = search_start; i_rot < num_pos; i_rot++) {

      /* Skip any filled positions that aren't near the beginning. */
      if (perm[i_rot] >= 0) {
        continue;
      }

      for (k = 0; k < 3; k++) {
        diff_frac[k] = pos_original[i_orig][k] - pos_rotated[i_rot][k];
        diff_frac[k] -= mat_Nint(diff_frac[k]);
      }
      mat_multiply_matrix_vector_d3(diff_cart, lattice, diff_frac);

      if (sqrt(mat_norm_squared_d3(diff_cart)) < symprec) {
        perm[i_rot] = i_orig;
        break;
      }
    }

    if (i_rot == num_pos) {
      /* We never hit the 'break'. */
      /* Failure; a position in pos_rotated does not */
      /* overlap with any position in pos_original. */
      return 0;
    }
  }

  /* Success */
  return 1;
}
