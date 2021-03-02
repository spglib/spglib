/* Copyright (C) 2012 Atsushi Togo */
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

#include <stdio.h>
#include <stdlib.h>
#include "primitive.h"
#include "mathfunc.h"
#include "symmetry.h"
#include "cell.h"
#include "spin.h"
#include "debug.h"

static Symmetry * get_operations(int *spin_flips,
                                 const Symmetry *sym_nonspin,
                                 const Cell *cell,
                                 const double *tensors,
                                 const int tensor_rank,
                                 const int is_magnetic,
                                 const double symprec);
static int set_equivalent_atoms(int * equiv_atoms,
                                const Symmetry *symmetry,
                                const Cell * cell,
                                const double symprec);
static int * get_mapping_table(const Symmetry *symmetry,
                               const Cell * cell,
                               const double symprec);
static int check_spin(const double spin_j,
                      const double spin_k,
                      const int sign,
                      const double symprec);
static int check_vector(const int j,
                        const int k,
                        const double* spins,
                        SPGCONST int rot[3][3],
                        SPGCONST double lattice[3][3],
                        const int is_magnetic,
                        const double symprec);

/* Return NULL if failed */
Symmetry * spn_get_operations_with_site_tensors(int equiv_atoms[],
                                                double prim_lattice[3][3],
                                                int *spin_flips,
                                                const Symmetry *sym_nonspin,
                                                const Cell *cell,
                                                const double *tensors,
                                                const int tensor_rank,
                                                const int is_magnetic,
                                                const double symprec,
                                                const double angle_tolerance)
{
  int i, num_pure_trans, multi;
  Symmetry *symmetry;
  VecDBL *pure_trans;
  int identity[3][3] = {{ 1, 0, 0 },
                        { 0, 1, 0 },
                        { 0, 0, 1 }};

  symmetry = NULL;
  pure_trans = NULL;

  if ((symmetry = get_operations(spin_flips,
                                 sym_nonspin,
                                 cell,
                                 tensors,
                                 tensor_rank,
                                 is_magnetic,
                                 symprec)) == NULL) {
    return NULL;
  }

  if ((set_equivalent_atoms(equiv_atoms,
                            symmetry,
                            cell,
                            symprec)) == 0) {
    sym_free_symmetry(symmetry);
    symmetry = NULL;
  }

  num_pure_trans = 0;
  for (i = 0; i < symmetry->size; i++) {
    if (mat_check_identity_matrix_i3(identity, symmetry->rot[i])) {
      num_pure_trans++;
    }
  }

  if ((pure_trans = mat_alloc_VecDBL(num_pure_trans)) == NULL) {
    return NULL;
  }

  num_pure_trans = 0;
  for (i = 0; i < symmetry->size; i++) {
    if (mat_check_identity_matrix_i3(identity, symmetry->rot[i])) {
      mat_copy_vector_d3(pure_trans->vec[num_pure_trans], symmetry->trans[i]);
      num_pure_trans++;
    }
  }

  multi = prm_get_primitive_lattice_vectors(prim_lattice,
                                            cell,
                                            pure_trans,
                                            symprec,
                                            angle_tolerance);

  /* By definition, change of number of pure translations would */
  /* not be allowed. */
  if (multi != num_pure_trans) {
    return NULL;
  }

  mat_free_VecDBL(pure_trans);
  pure_trans = NULL;

  return symmetry;
}

/* Return NULL if failed */
/* spin_flips can be NULL if tensor_rank != 0. */
static Symmetry * get_operations(int *spin_flips,
                                 const Symmetry *sym_nonspin,
                                 const Cell *cell,
                                 const double *tensors,
                                 const int tensor_rank,
                                 const int is_magnetic,
                                 const double symprec)
{
  Symmetry *symmetry;
  int i, j, k, sign, num_sym, is_found;
  double pos[3];
  MatINT * rotations;
  VecDBL * trans;

  rotations = mat_alloc_MatINT(sym_nonspin->size);
  trans = mat_alloc_VecDBL(sym_nonspin->size);
  num_sym = 0;

  is_found = 0;

  for (i = 0; i < sym_nonspin->size; i++) {
    /* Set sign as undetermined (used for collinear spin case (rank = 0) */
    sign = 0;
    for (j = 0; j < cell->size; j++) {
      mat_multiply_matrix_vector_id3(pos, sym_nonspin->rot[i], cell->position[j]);
      for (k = 0; k < 3; k++) {
        pos[k] += sym_nonspin->trans[i][k];
      }
      for (k = 0; k < cell->size; k++) {
        if (cel_is_overlap_with_same_type(cell->position[k],
                                          pos,
                                          cell->types[k],
                                          cell->types[j],
                                          cell->lattice,
                                          symprec)) {
          if (tensor_rank == 0) {
            if (is_magnetic) {
              sign = check_spin(tensors[j], tensors[k], sign, symprec);
              is_found = abs(sign);
            } else {
              if (mat_Dabs(tensors[j] - tensors[k]) < symprec) {
                is_found = 1;
              } else {
                is_found = 0;
              }
            }
          }
          if (tensor_rank == 1) {
            is_found = check_vector(j, k, tensors, sym_nonspin->rot[i],
                                    cell->lattice, is_magnetic, symprec);
          }
          /* Break because cel_is_overlap_with_same_type == true */
          /* for only one atom. */
          break;
        }
      }
      if (! is_found) {
        break;
      }
    }
    if (is_found) {
      mat_copy_matrix_i3(rotations->mat[num_sym], sym_nonspin->rot[i]);
      mat_copy_vector_d3(trans->vec[num_sym], sym_nonspin->trans[i]);
      if ((tensor_rank == 0) && (is_magnetic)) {
        spin_flips[num_sym] = sign;
      }
      num_sym++;
    }
  }

  symmetry = sym_alloc_symmetry(num_sym);
  for (i = 0; i < num_sym; i++) {
    mat_copy_matrix_i3(symmetry->rot[i], rotations->mat[i]);
    mat_copy_vector_d3(symmetry->trans[i], trans->vec[i]);
  }

  mat_free_MatINT(rotations);
  rotations = NULL;
  mat_free_VecDBL(trans);
  trans = NULL;

  return symmetry;
}


/* Return 0 if failed */
static int set_equivalent_atoms(int * equiv_atoms,
                                const Symmetry *symmetry,
                                const Cell * cell,
                                const double symprec)
{
  int i, j, k, is_found;
  double pos[3];
  int *mapping_table;

  mapping_table = NULL;

  if ((mapping_table = get_mapping_table(symmetry, cell, symprec)) == NULL) {
    return 0;
  }

  for (i = 0; i < cell->size; i++) {
    if (mapping_table[i] != i) {
      continue;
    }
    is_found = 0;
    for (j = 0; j < symmetry->size; j++) {
      mat_multiply_matrix_vector_id3(pos,
                                     symmetry->rot[j],
                                     cell->position[i]);
      for (k = 0; k < 3; k++) {
        pos[k] += symmetry->trans[j][k];
      }
      for (k = 0; k < cell->size; k++) {
        if (cel_is_overlap_with_same_type(pos,
                                          cell->position[k],
                                          cell->types[i],
                                          cell->types[k],
                                          cell->lattice,
                                          symprec)) {
          if (mapping_table[k] < i) {
            equiv_atoms[i] = equiv_atoms[mapping_table[k]];
            is_found = 1;
            break;
          }
        }
      }
      if (is_found) {
        break;
      }
    }
    if (!is_found) {
      equiv_atoms[i] = i;
    }
  }

  for (i = 0; i < cell->size; i++) {
    if (mapping_table[i] == i) {
      continue;
    }
    equiv_atoms[i] = equiv_atoms[mapping_table[i]];
  }

  free(mapping_table);
  mapping_table = NULL;

  return 1;
}

/* Return NULL if failed */
static int * get_mapping_table(const Symmetry *symmetry,
                               const Cell * cell,
                               const double symprec)
{
  int i, j, k, is_found;
  double pos[3];
  int *mapping_table;
  SPGCONST int I[3][3] = {{ 1, 0, 0},
                          { 0, 1, 0},
                          { 0, 0, 1}};

  mapping_table = NULL;

  if ((mapping_table = (int*) malloc(sizeof(int) * cell->size)) == NULL) {
    warning_print("spglib: Memory could not be allocated.");
    return NULL;
  }

  for (i = 0; i < cell->size; i++) {
    is_found = 0;
    for (j = 0; j < symmetry->size; j++) {
      if (mat_check_identity_matrix_i3(symmetry->rot[j], I)) {
        for (k = 0; k < 3; k++) {
          pos[k] = cell->position[i][k] + symmetry->trans[j][k];
        }
        for (k = 0; k < cell->size; k++) {
          if (cel_is_overlap_with_same_type(pos,
                                            cell->position[k],
                                            cell->types[i],
                                            cell->types[k],
                                            cell->lattice,
                                            symprec)) {
            if (k < i) {
              mapping_table[i] = mapping_table[k];
              is_found = 1;
              break;
            }
          }
        }
      }
      if (is_found) {
        break;
      }
    }
    if (!is_found) {
      mapping_table[i] = i;
    }
  }

  return mapping_table;
}

static int check_spin(const double spin_j,
                      const double spin_k,
                      const int sign,
                      const double symprec)
{
  if (sign == 0) {
    if (mat_Dabs(spin_j - spin_k) < symprec) {
      return 1;
    }
    if (mat_Dabs(spin_j + spin_k) < symprec) {
      return -1;
    }
    return 0;
  } else {
    if (mat_Dabs(spin_j - spin_k * sign) < symprec) {
      return sign;
    } else {
      return 0;
    }
  }
}

/* Work in Cartesian coordinates. */
static int check_vector(const int j,
                        const int k,
                        const double* vectors,
                        SPGCONST int rot[3][3],
                        SPGCONST double lattice[3][3],
                        const int is_magnetic,
                        const double symprec)
{
  int i, detR;
  double vec_j[3], vec_jp[3], diff[3];
  double inv_lat[3][3], rot_cart[3][3];

  /* is_magnetic: Non-collinear magnetic moment m' = |detR|Rm */
  /* !is_magnetic: Usual vector: v' = Rv */

  mat_inverse_matrix_d3(inv_lat, lattice, 0);
  mat_multiply_matrix_id3(rot_cart, rot, inv_lat);
  mat_multiply_matrix_d3(rot_cart, lattice, rot_cart);

  for (i = 0; i < 3; i++) {
    vec_j[i] = vectors[j * 3 + i];
  }

  /* v_j' = R v_j */
  mat_multiply_matrix_vector_d3(vec_jp, rot_cart, vec_j);

  if (is_magnetic) {
    detR = mat_get_determinant_i3(rot);
    for (i = 0; i < 3; i++) {
      diff[i] = mat_Dabs(detR * vec_jp[i] - vectors[k * 3 + i]);
    }
  } else {
    for (i = 0; i < 3; i++) {
      diff[i] = mat_Dabs(vec_jp[i] - vectors[k * 3 + i]);
    }
  }

  if (diff[0] < symprec && diff[1] < symprec && diff[2] < symprec) {
    return 1;
  } else {
    return 0;
  }
}
