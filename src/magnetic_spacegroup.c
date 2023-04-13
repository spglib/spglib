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

#include "magnetic_spacegroup.h"

#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "msg_database.h"
#include "refinement.h"
#include "spin.h"

#define MAX_DENOMINATOR 100

static int get_reference_space_group(Spacegroup **ref_sg,
                                     MagneticSymmetry **changed_symmetry,
                                     double tmat[3][3], double shift[3],
                                     const MagneticSymmetry *magnetic_symmetry,
                                     const double symprec);
static Symmetry *get_family_space_group_with_magnetic_symmetry(
    Spacegroup **fsg, const MagneticSymmetry *magnetic_symmetry,
    const double symprec);
static Symmetry *get_maximal_subspace_group_with_magnetic_symmetry(
    Spacegroup **xsg, const MagneticSymmetry *magnetic_symmetry,
    const double symprec);
static Symmetry *get_space_group_with_magnetic_symmetry(
    Spacegroup **spacegroup, const MagneticSymmetry *magnetic_symmetry,
    const int ignore_time_reversal, const double symprec);
static int get_magnetic_space_group_type(
    MagneticSymmetry **representative,
    const MagneticSymmetry *magnetic_symmetry, const int num_sym_fsg,
    const int num_sym_xsg);
static MagneticSymmetry *get_representative(
    const MagneticSymmetry *magnetic_symmetry);
static MagneticSymmetry *get_changed_magnetic_symmetry(
    const double tmat[3][3], const double shift[3],
    const MagneticSymmetry *representative, const Symmetry *sym_xsg,
    const MagneticSymmetry *magnetic_symmetry, const double symprec);
static VecDBL *get_changed_pure_translations(const double tmat[3][3],
                                             const VecDBL *pure_trans,
                                             const double symprec);
static int is_contained_vec(const double v[3], const VecDBL *trans,
                            const int size, const double symprec);
static int is_contained_mat(const int a[3][3], const MagneticSymmetry *sym_msg,
                            const int size);
static MagneticSymmetry *get_distinct_changed_magnetic_symmetry(
    const double tmat[3][3], const double shift[3],
    const MagneticSymmetry *sym_msg);
static int is_equal(const MagneticSymmetry *sym1, const MagneticSymmetry *sym2,
                    const double symprec);
void get_rigid_rotation(double rigid_rot[3][3], const double lattice[3][3],
                        const double tmat[3][3], const Spacegroup *ref_sg);

/******************************************************************************/

/// @brief Identify magnetic space-group type with database
/// If failed, return NULL.
MagneticDataset *msg_identify_magnetic_space_group_type(
    const double lattice[3][3], const MagneticSymmetry *magnetic_symmetry,
    const double symprec) {
    int i, j, s, hall_number, uni_number, type, same;
    Spacegroup *ref_sg;
    Symmetry *transformations;
    MagneticSymmetry *msg_uni, *changed_symmetry, *symmetry_cor;
    MagneticSpacegroupType msgtype, msgtype_db;
    MagneticDataset *ret;
    int uni_number_range[2];
    double rigid_rot[3][3];
    double tmat[3][3], tmat_cor[3][3];
    double shift[3], shift_cor[3];

    transformations = NULL;
    ref_sg = NULL;
    msg_uni = NULL;
    changed_symmetry = NULL;
    symmetry_cor = NULL;
    ret = NULL;

    /* TODO(shinohara): add option to specify hall_number in searching
     * space-group type */
    type = get_reference_space_group(&ref_sg, &changed_symmetry, tmat, shift,
                                     magnetic_symmetry, symprec);
    if (type == 0) goto err;
    hall_number = ref_sg->hall_number;

    msgdb_get_uni_candidates(uni_number_range, hall_number);
    debug_print("Search UNI number over between %d to %d\n",
                uni_number_range[0], uni_number_range[1]);
    for (uni_number = uni_number_range[0]; uni_number <= uni_number_range[1];
         uni_number++) {
        /* Check type and order */
        msgtype_db = msgdb_get_magnetic_spacegroup_type(uni_number);
        if (msgtype_db.type != type) continue;

        msg_uni = msgdb_get_spacegroup_operations(uni_number, hall_number);
        if (msg_uni->size != changed_symmetry->size) continue;

        /* Correction transformation */
        /* x_uni = (tmat_cor, shift_cor) x_changed */
        if ((transformations = msgdb_get_std_transformations(
                 uni_number, hall_number)) == NULL)
            goto err;

        for (i = 0; i < transformations->size; i++) {
            same = 0;

            mat_cast_matrix_3i_to_3d(tmat_cor, transformations->rot[i]);
            mat_copy_vector_d3(shift_cor, transformations->trans[i]);

            /* Since det(tmat_corr) = 1, no need to care about duplicated
             * operations */
            if ((symmetry_cor = get_distinct_changed_magnetic_symmetry(
                     tmat_cor, shift_cor, changed_symmetry)) == NULL)
                goto err;

            debug_print("\e[33mCorrection\e[0m\n");
            debug_print_matrix_d3(tmat_cor);
            debug_print_vector_d3(shift_cor);
            for (j = 0; j < symmetry_cor->size; j++) {
                debug_print("-- %d --\n", j);
                debug_print_matrix_i3(symmetry_cor->rot[j]);
                debug_print_vector_d3(symmetry_cor->trans[j]);
                debug_print("timerev=%d\n", symmetry_cor->timerev[j]);
            }

            same = is_equal(msg_uni, symmetry_cor, symprec);
            sym_free_magnetic_symmetry(symmetry_cor);
            symmetry_cor = NULL;
            if (same) break;
        }

        sym_free_symmetry(transformations);
        transformations = NULL;
        sym_free_magnetic_symmetry(msg_uni);
        msg_uni = NULL;
        if (same) break;
    }
    if (uni_number > uni_number_range[1]) {
        warning_print("Failed to match with UNI number!\n");
        goto err;
    }
    debug_print("UNI number: %d\n", uni_number);

    msgtype = msgdb_get_magnetic_spacegroup_type(uni_number);
    if (msgtype.type != type) {
        warning_print("Inconsistent MSG type:\n");
        warning_print("  From FSG and XSG: %d\n", type);
        warning_print("  From DB matching: %d\n", msgtype.type);
        goto err;
    }

    /* Update transformation */
    /* x_changed = (tmat, shift) x, x_uni = (tmat_cor, shift_cor) x_changed */
    /* => (tmat, shift) => (tmat_cor, shift_cor) (tmat, shift) */
    mat_multiply_matrix_d3(tmat, tmat_cor, tmat);
    /* tmat_cor * shift + shift_cor */
    mat_multiply_matrix_vector_d3(shift, tmat_cor, shift);
    for (s = 0; s < 3; s++) {
        shift[s] += shift_cor[s];
    }

    mat_multiply_matrix_d3(ref_sg->bravais_lattice, lattice,
                           ref_sg->bravais_lattice);
    /* Rigid rotation to standardized lattice */
    get_rigid_rotation(rigid_rot, lattice, tmat, ref_sg);

    /* Set MagneticDataset */
    if ((ret = (MagneticDataset *)(malloc(sizeof(MagneticDataset)))) == NULL)
        goto err;

    ret->uni_number = msgtype.uni_number;
    ret->msg_type = msgtype.type;
    ret->hall_number = hall_number;
    mat_copy_matrix_d3(ret->transformation_matrix, tmat);
    mat_copy_vector_d3(ret->origin_shift, shift);
    mat_copy_matrix_d3(ret->std_rotation_matrix, rigid_rot);

    free(ref_sg);
    ref_sg = NULL;
    /* msg_uni is already freed */
    /* symmetry_cor is already freed */
    sym_free_magnetic_symmetry(changed_symmetry);
    changed_symmetry = NULL;

    return ret;

err:
    if (ref_sg != NULL) {
        free(ref_sg);
        ref_sg = NULL;
    }
    if (msg_uni != NULL) {
        sym_free_magnetic_symmetry(msg_uni);
        msg_uni = NULL;
    }
    if (transformations != NULL) {
        sym_free_symmetry(transformations);
        transformations = NULL;
    }
    if (symmetry_cor != NULL) {
        sym_free_magnetic_symmetry(symmetry_cor);
        symmetry_cor = NULL;
    }
    if (changed_symmetry != NULL) {
        sym_free_magnetic_symmetry(changed_symmetry);
        changed_symmetry = NULL;
    }
    if (ret != NULL) {
        free(ret);
        ret = NULL;
    }

    return NULL;
}

/* Return transformed cell. Note that site tensors do not need to be transformed
 */
/* since they are in cartesian coordiantes. Return NULL if failed. */
/* To transform cell and site tensors with cell-size changing cases, */
/*   1. transform `cell` to primitive */
/*   2. compute pure translations after given transformation */
/*   3. apply the pure translations to the primitive */
Cell *msg_get_transformed_cell(const Cell *cell, const double tmat[3][3],
                               const double origin_shift[3],
                               const double rigid_rot[3][3],
                               const MagneticSymmetry *magnetic_symmetry,
                               const double symprec,
                               const double angle_tolerance) {
    int i, p, ip, s, changed_num_atoms;
    VecDBL *pure_trans, *prm_pure_trans, *changed_pure_trans;
    Primitive *primitive;
    Cell *changed_cell;
    int *remapping;
    double tmat_prm[3][3], inv_latt[3][3], tmat_inv[3][3];
    double pos_tmp[3];

    pure_trans = NULL;
    prm_pure_trans = NULL;
    changed_pure_trans = NULL;
    primitive = NULL;
    changed_cell = NULL;
    remapping = NULL;

    /* 1. transform `cell` to primitive */
    if ((pure_trans = spn_collect_pure_translations_from_magnetic_symmetry(
             magnetic_symmetry)) == NULL)
        goto err;
    /* Supposedly, primitive->mapping_table maps cell to primitive->cell */
    if ((primitive = prm_alloc_primitive(cell->size)) == NULL) {
        goto err;
    }
    if (prm_get_primitive_with_pure_trans(primitive, cell, pure_trans, symprec,
                                          angle_tolerance) == 0)
        goto err;
    /* cell->lattice = (a_std, b_std, c_std) @ tmat */
    /* primitive->cell->lattice = (a_std, b_std, c_std) @ tmat_prm */
    /* => tmat_prm = tmat @ cell->lattice^{-1} @ primitive->cell->lattice */
    mat_inverse_matrix_d3(inv_latt, cell->lattice, 0);
    mat_multiply_matrix_d3(tmat_prm, tmat, inv_latt);
    mat_multiply_matrix_d3(tmat_prm, tmat_prm, primitive->cell->lattice);

    /* mapping from original cell to primitive is one-to-many */
    /* Thus, its inverse mapping does not exist. */
    /* However, because site tensors do not change by pure translations, */
    /* it is safe to pick out one site from preimage in original cell. */
    if ((remapping = (int *)malloc(sizeof(int) * primitive->cell->size)) ==
        NULL)
        goto err;
    for (i = 0; i < primitive->cell->size; i++) {
        remapping[i] = -1;
    }
    for (i = 0; i < cell->size; i++) {
        if (remapping[primitive->mapping_table[i]] != -1) continue;
        remapping[primitive->mapping_table[i]] = i;
    }

    /* 2. compute pure translations in transformed cell */
    if ((prm_pure_trans = mat_alloc_VecDBL(1)) == NULL) goto err;
    for (s = 0; s < 3; s++) {
        prm_pure_trans->vec[0][s] = 0;
    }
    if ((changed_pure_trans = get_changed_pure_translations(
             tmat_prm, prm_pure_trans, symprec)) == NULL)
        goto err;

    /* 3. apply the pure translations to the primitive */
    changed_num_atoms = primitive->cell->size * changed_pure_trans->size;
    if ((changed_cell = cel_alloc_cell(changed_num_atoms, cell->tensor_rank)) ==
        NULL)
        goto err;

    for (i = 0; i < primitive->cell->size; i++) {
        /* x_std = (tmat_prm, origin_shift) x_prm  */
        mat_multiply_matrix_vector_d3(pos_tmp, tmat_prm,
                                      primitive->cell->position[i]);
        for (s = 0; s < 3; s++) {
            pos_tmp[s] += origin_shift[s];
        }

        for (p = 0; p < changed_pure_trans->size; p++) {
            ip = i * changed_pure_trans->size + p;
            changed_cell->types[ip] = primitive->cell->types[i];
            for (s = 0; s < 3; s++) {
                changed_cell->position[ip][s] =
                    mat_Dmod1(pos_tmp[s] + changed_pure_trans->vec[p][s]);
            }
            debug_print_vector_d3(changed_cell->position[ip]);

            /* No need to apply transformation for cartesian coordinates. */
            /* On the other hand, need to apply rigid rotation to site tensors.
             */
            if (cell->tensor_rank == COLLINEAR) {
                changed_cell->tensors[ip] = cell->tensors[remapping[i]];
            } else if (cell->tensor_rank == NONCOLLINEAR) {
                /* Assume rigid_rot is proper. */
                mat_multiply_matrix_vector_d3(changed_cell->tensors + 3 * ip,
                                              rigid_rot,
                                              cell->tensors + 3 * remapping[i]);
            }
        }
    }

    /* Rotate cell->lattice. Note that rigid_rot is in cartesian. */
    mat_multiply_matrix_d3(changed_cell->lattice, rigid_rot, cell->lattice);
    mat_inverse_matrix_d3(tmat_inv, tmat, 0);
    mat_multiply_matrix_d3(changed_cell->lattice, changed_cell->lattice,
                           tmat_inv);

    free(remapping);
    remapping = NULL;
    mat_free_VecDBL(pure_trans);
    pure_trans = NULL;
    mat_free_VecDBL(prm_pure_trans);
    prm_pure_trans = NULL;
    mat_free_VecDBL(changed_pure_trans);
    changed_pure_trans = NULL;
    prm_free_primitive(primitive);
    primitive = NULL;

    return changed_cell;

err:
    if (remapping != NULL) {
        free(remapping);
        remapping = NULL;
    }
    if (pure_trans != NULL) {
        mat_free_VecDBL(pure_trans);
        pure_trans = NULL;
    }
    if (prm_pure_trans != NULL) {
        mat_free_VecDBL(prm_pure_trans);
        prm_pure_trans = NULL;
    }
    if (changed_pure_trans != NULL) {
        mat_free_VecDBL(changed_pure_trans);
        changed_pure_trans = NULL;
    }
    if (primitive != NULL) {
        prm_free_primitive(primitive);
        primitive = NULL;
    }
    if (changed_cell != NULL) {
        cel_free_cell(changed_cell);
        changed_cell = NULL;
    }
    return NULL;
}

/******************************************************************************/
/* Local functions                                                            */
/******************************************************************************/

/* Return type of MSG. Return 0 if failed. */
static int get_reference_space_group(Spacegroup **ref_sg,
                                     MagneticSymmetry **changed_symmetry,
                                     double tmat[3][3], double shift[3],
                                     const MagneticSymmetry *magnetic_symmetry,
                                     const double symprec) {
    int type;
    Symmetry *sym_fsg, *sym_xsg;
    Spacegroup *fsg, *xsg;
    MagneticSymmetry *representatives;

    sym_fsg = NULL;
    sym_xsg = NULL;
    fsg = NULL;
    xsg = NULL;
    representatives = NULL;

    /* Identify family space group (FSG) and maximal space group (XSG) */
    if ((sym_fsg = get_family_space_group_with_magnetic_symmetry(
             &fsg, magnetic_symmetry, symprec)) == NULL)
        goto err;
    debug_print("FSG: hall_number=%d, international=%s, order=%d\n",
                fsg->hall_number, fsg->international_short, sym_fsg->size);
    if ((sym_xsg = get_maximal_subspace_group_with_magnetic_symmetry(
             &xsg, magnetic_symmetry, symprec)) == NULL)
        goto err;
    debug_print("XSG: hall_number=%d, international=%s, order=%d\n",
                xsg->hall_number, xsg->international_short, sym_xsg->size);

    /* Determine type of MSG and generator of factor group of MSG over XSG */
    type = get_magnetic_space_group_type(&representatives, magnetic_symmetry,
                                         sym_fsg->size, sym_xsg->size);
    debug_print("type=%d\n", type);
    if (type == 0) {
        goto err;
    }

    /* Choose reference setting */
    /* For type-IV, use setting from Hall symbol of XSG. */
    /* For other types, use setting from Hall symbol of FSG. */
    if ((*ref_sg = (Spacegroup *)malloc(sizeof(Spacegroup))) == NULL) goto err;
    if (type == 4) {
        spa_copy_spacegroup(*ref_sg, xsg);
    } else {
        spa_copy_spacegroup(*ref_sg, fsg);
    }
    mat_inverse_matrix_d3(tmat, (*ref_sg)->bravais_lattice, 0);
    mat_copy_vector_d3(shift, (*ref_sg)->origin_shift);

    debug_print("Transformation\n");
    debug_print_matrix_d3(tmat);
    debug_print_vector_d3(shift);
    debug_print("det = %f\n", mat_get_determinant_d3(tmat));

    if ((*changed_symmetry = get_changed_magnetic_symmetry(
             tmat, shift, representatives, sym_xsg, magnetic_symmetry,
             symprec)) == NULL)
        goto err;

    sym_free_symmetry(sym_fsg);
    sym_fsg = NULL;
    sym_free_symmetry(sym_xsg);
    sym_xsg = NULL;
    free(fsg);
    fsg = NULL;
    free(xsg);
    xsg = NULL;
    sym_free_magnetic_symmetry(representatives);
    representatives = NULL;

    return type;
err:
    if (sym_fsg != NULL) {
        sym_free_symmetry(sym_fsg);
        sym_fsg = NULL;
    }
    if (sym_xsg != NULL) {
        sym_free_symmetry(sym_xsg);
        sym_xsg = NULL;
    }
    if (fsg != NULL) {
        free(fsg);
        fsg = NULL;
    }
    if (xsg != NULL) {
        free(xsg);
        xsg = NULL;
    }
    if (representatives != NULL) {
        sym_free_magnetic_symmetry(representatives);
        representatives = NULL;
    }

    return 0;
}
/* Get family space group (FSG) and return its order. */
/* FSG is a non-magnetic space group obtained by ignoring primes in operations.
 */
/* If failed, return NULL. */
static Symmetry *get_family_space_group_with_magnetic_symmetry(
    Spacegroup **fsg, const MagneticSymmetry *magnetic_symmetry,
    const double symprec) {
    return get_space_group_with_magnetic_symmetry(fsg, magnetic_symmetry, 1,
                                                  symprec);
}

/* Get maximal subspace group (XSG) and return its order. */
/* XSG is a space group obtained by removing primed operations. */
/* If failed, return NULL. */
static Symmetry *get_maximal_subspace_group_with_magnetic_symmetry(
    Spacegroup **xsg, const MagneticSymmetry *magnetic_symmetry,
    const double symprec) {
    return get_space_group_with_magnetic_symmetry(xsg, magnetic_symmetry, 0,
                                                  symprec);
}

/* ignore_time_reversal=true  -> FSG */
/* ignore_time_reversal=false -> XSG */
/* If failed, return NULL. */
/* Output's `spacegroup` is w.r.t. input setting: */
/*     x_std = (P^-1, p) x */
/*    (a_std, b_std, c_std) = (a, b, c) @ P */
/* where P := spacegroup->bravais_lattice, p := spacegroup->origin_shift. */
static Symmetry *get_space_group_with_magnetic_symmetry(
    Spacegroup **spacegroup, const MagneticSymmetry *magnetic_symmetry,
    const int ignore_time_reversal, const double symprec) {
    int i, num_sym_msg, num_sym, is_type2;
    Symmetry *sym, *prim_sym;
    int identity[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    double unit_lat[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    double tmat[3][3], inv_tmat[3][3];

    sym = NULL;
    prim_sym = NULL;
    num_sym_msg = magnetic_symmetry->size;

    if ((sym = sym_alloc_symmetry(num_sym_msg)) == NULL) {
        return NULL;
    }

    /* If MSG is type-II, it has pure time-reversal operation (I, 0)1' */
    is_type2 = 0;
    for (i = 0; i < num_sym_msg; i++) {
        if (mat_check_identity_matrix_i3(identity, magnetic_symmetry->rot[i]) &&
            mat_Dabs(magnetic_symmetry->trans[i][0]) < symprec &&
            mat_Dabs(magnetic_symmetry->trans[i][1]) < symprec &&
            mat_Dabs(magnetic_symmetry->trans[i][2]) < symprec &&
            magnetic_symmetry->timerev[i]) {
            is_type2 = 1;
        }
    }

    /* XSG: Accept only ordinary operations */
    num_sym = 0;
    for (i = 0; i < num_sym_msg; i++) {
        if (!ignore_time_reversal && magnetic_symmetry->timerev[i] == 1) {
            continue;
        }

        /* For type-II MSG, letting `g` be some symmetry operation, both g1 and
         * g1' */
        /* belong to MSG. To eliminate duplicated symmetry, we only take the
         * other. */
        if (is_type2 && magnetic_symmetry->timerev[i] == 1) {
            continue;
        }

        mat_copy_matrix_i3(sym->rot[num_sym], magnetic_symmetry->rot[i]);
        mat_copy_vector_d3(sym->trans[num_sym], magnetic_symmetry->trans[i]);
        num_sym++;
    }
    sym->size = num_sym;

    /* (a, b, c) = (a_prim, b_prim, c_prim) @ tmat */
    prim_sym = prm_get_primitive_symmetry(tmat, sym, symprec);
    *spacegroup =
        spa_search_spacegroup_with_symmetry(prim_sym, unit_lat, symprec);
    /* refine bravais_lattice and origin_shift */
    ref_find_similar_bravais_lattice(*spacegroup, symprec);
    /* At this point, let P = spacegroup->bravais_lattice, p =
     * spacegroup->origin_shift: */
    /*     x_std = (P^-1, p) x_prim */
    /*     (a_std, b_std, c_std) = (a_prim, b_prim, c_prim) @ P */

    /* Change basis from primitive to original */
    /* x = (tmat, 0)^-1 x_prim */
    /* => x_std = (P^-1, p) (tmat, 0) x */
    /*          = ( P^-1 @ tmat, p) x */
    /*          = ( (tmat^-1 @ P)^-1, p) x */
    /*    (a_std, b_std, c_std) = (a, b, c) @ tmat^-1 @ P */
    mat_inverse_matrix_d3(inv_tmat, tmat, 0);
    mat_multiply_matrix_d3((*spacegroup)->bravais_lattice, inv_tmat,
                           (*spacegroup)->bravais_lattice);

    sym_free_symmetry(prim_sym);
    prim_sym = NULL;

    if (spacegroup == NULL) {
        return NULL;
    }

    return sym;
}

/* Return type of MSG. If failed, return 0. */
static int get_magnetic_space_group_type(
    MagneticSymmetry **representative,
    const MagneticSymmetry *magnetic_symmetry, const int num_sym_fsg,
    const int num_sym_xsg) {
    int num_sym_msg;
    int identity[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

    num_sym_msg = magnetic_symmetry->size;

    if (num_sym_fsg == num_sym_xsg) {
        if ((*representative = sym_alloc_magnetic_symmetry(2)) == NULL)
            return 0;

        /* Set the first representative as identity */
        mat_copy_matrix_i3((*representative)->rot[0], identity);
        (*representative)->trans[0][0] = 0;
        (*representative)->trans[0][1] = 0;
        (*representative)->trans[0][2] = 0;
        (*representative)->timerev[0] = 0;

        if (num_sym_msg == num_sym_fsg) {
            (*representative)->size = 1; /* shrink size */
            return 1;                    /* Type-I */
        } else if (num_sym_msg == 2 * num_sym_fsg) {
            /* Pure anti operation */
            mat_copy_matrix_i3((*representative)->rot[1], identity);
            (*representative)->trans[1][0] = 0;
            (*representative)->trans[1][1] = 0;
            (*representative)->trans[1][2] = 0;
            (*representative)->timerev[1] = 1;
            return 2; /* Type-II */
        } else {
            return 0; /* Unreachable */
        }
    } else if (num_sym_fsg == 2 * num_sym_xsg) {
        *representative = get_representative(magnetic_symmetry);
        if (*representative == NULL) return 0;

        /* If primed operation is translation, type-IV. Otherwise, type-III. */
        if (mat_check_identity_matrix_i3(identity, (*representative)->rot[1])) {
            return 4; /* Type-IV */
        } else {
            return 3; /* Type-III */
        }
    }

    return 0; /* Unreachable */
}

/* Get coset representative of XSG in MSG. */
/* Assume magnetic_symmetry is type-III or type-IV. */
/* If failed, return NULL. */
static MagneticSymmetry *get_representative(
    const MagneticSymmetry *magnetic_symmetry) {
    int i;
    MagneticSymmetry *representative;
    int identity[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

    representative = NULL;
    if ((representative = sym_alloc_magnetic_symmetry(2)) == NULL) return NULL;

    /* Set the first representative as identity */
    mat_copy_matrix_i3(representative->rot[0], identity);
    representative->trans[0][0] = 0;
    representative->trans[0][1] = 0;
    representative->trans[0][2] = 0;
    representative->timerev[0] = 0;

    /* If primed operation with identity linear part has translations other */
    /* than centering vectors, MSG is type-IV. */
    for (i = 0; i < magnetic_symmetry->size; i++) {
        if (mat_check_identity_matrix_i3(magnetic_symmetry->rot[i], identity) &&
            magnetic_symmetry->timerev[i]) {
            /* Type-IV: anti translation */
            mat_copy_matrix_i3(representative->rot[1],
                               magnetic_symmetry->rot[i]);
            mat_copy_vector_d3(representative->trans[1],
                               magnetic_symmetry->trans[i]);
            representative->timerev[1] = 1;
            return representative;
        }
    }

    /* Now, this MSG should be type-III. */
    for (i = 0; i < magnetic_symmetry->size; i++) {
        if (!magnetic_symmetry->timerev[i]) continue;

        mat_copy_matrix_i3(representative->rot[1], magnetic_symmetry->rot[i]);
        mat_copy_vector_d3(representative->trans[1],
                           magnetic_symmetry->trans[i]);
        representative->timerev[1] = 1;
        return representative;
    }
    return NULL;
}

/* Transformation (tmat, shift) converts Hall symbol's setting to user input. */
/* (a, b, c) = (a_std, b_std, c_std) tmat */
/* x = (tmat, shift)^-1 x_std */
/* (W, w) = (tmat, shift)^-1 (W_std, w_std) (tmat, shift) */
/* If failed, return NULL. */
/* Be careful the correspondence: tmat = spacegroup->bravais_lattice^-1 */
static MagneticSymmetry *get_changed_magnetic_symmetry(
    const double tmat[3][3], const double shift[3],
    const MagneticSymmetry *representatives, const Symmetry *sym_xsg,
    const MagneticSymmetry *magnetic_symmetry, const double symprec) {
    int size, num_factors, num_sym, i, j, k;
    VecDBL *pure_trans, *changed_pure_trans;
    MagneticSymmetry *changed, *factors, *changed_factors,
        *changed_representatives;
    double trans_tmp[3];

    pure_trans = NULL;
    factors = NULL;
    changed = NULL;
    changed_pure_trans = NULL;
    changed_representatives = NULL;
    changed_factors = NULL;
    num_factors = 0;
    num_sym = 0;

    /* Additional operation in MSG */
    if ((changed_representatives = get_distinct_changed_magnetic_symmetry(
             tmat, shift, representatives)) == NULL)
        goto err;

    /* centerings in conventional lattice */
    if ((pure_trans = spn_collect_pure_translations_from_magnetic_symmetry(
             magnetic_symmetry)) == NULL)
        goto err;
    if ((changed_pure_trans =
             get_changed_pure_translations(tmat, pure_trans, symprec)) == NULL)
        goto err;

    /* Collect factor group in conventional lattice */
    if ((factors = sym_alloc_magnetic_symmetry(sym_xsg->size)) == NULL)
        goto err;
    for (i = 0; i < sym_xsg->size; i++) {
        if (is_contained_mat(sym_xsg->rot[i], factors, num_factors)) continue;

        mat_copy_matrix_i3(factors->rot[num_factors], sym_xsg->rot[i]);
        mat_copy_vector_d3(factors->trans[num_factors], sym_xsg->trans[i]);
        factors->timerev[num_factors] = 0;
        num_factors++;
    }
    factors->size = num_factors;
    if ((changed_factors = get_distinct_changed_magnetic_symmetry(
             tmat, shift, factors)) == NULL)
        goto err;

    /* Number of coset may change in conversion between hR and hP! */
    size = representatives->size * changed_pure_trans->size * num_factors;
    if ((changed = sym_alloc_magnetic_symmetry(size)) == NULL) goto err;

    for (i = 0; i < changed_pure_trans->size; i++) {
        for (j = 0; j < changed_representatives->size; j++) {
            for (k = 0; k < num_factors; k++) {
                /* (I, ti)(Pj, tj)(Pk, tk) = (Pj * Pk, Pj * tk + tj + ti) */
                mat_multiply_matrix_i3(changed->rot[num_sym],
                                       changed_representatives->rot[j],
                                       changed_factors->rot[k]);

                mat_multiply_matrix_vector_id3(trans_tmp,
                                               changed_representatives->rot[j],
                                               changed_factors->trans[k]);
                trans_tmp[0] += changed_representatives->trans[j][0] +
                                changed_pure_trans->vec[i][0];
                trans_tmp[1] += changed_representatives->trans[j][1] +
                                changed_pure_trans->vec[i][1];
                trans_tmp[2] += changed_representatives->trans[j][2] +
                                changed_pure_trans->vec[i][2];
                trans_tmp[0] = mat_Dmod1(trans_tmp[0]);
                trans_tmp[1] = mat_Dmod1(trans_tmp[1]);
                trans_tmp[2] = mat_Dmod1(trans_tmp[2]);
                mat_copy_vector_d3(changed->trans[num_sym], trans_tmp);

                changed->timerev[num_sym] =
                    (changed_representatives->timerev[j] !=
                     changed_factors->timerev[k]);
                num_sym++;
            }
        }
    }

    mat_free_VecDBL(pure_trans);
    pure_trans = NULL;
    mat_free_VecDBL(changed_pure_trans);
    changed_pure_trans = NULL;
    sym_free_magnetic_symmetry(factors);
    factors = NULL;
    sym_free_magnetic_symmetry(changed_factors);
    changed_factors = NULL;
    sym_free_magnetic_symmetry(changed_representatives);
    changed_representatives = NULL;
    return changed;
err:
    if (pure_trans != NULL) {
        mat_free_VecDBL(pure_trans);
        pure_trans = NULL;
    }
    if (changed_pure_trans != NULL) {
        mat_free_VecDBL(changed_pure_trans);
        changed_pure_trans = NULL;
    }
    if (factors != NULL) {
        sym_free_magnetic_symmetry(factors);
        factors = NULL;
    }
    if (changed_factors != NULL) {
        sym_free_magnetic_symmetry(changed_factors);
        changed_factors = NULL;
    }
    if (changed_representatives != NULL) {
        sym_free_magnetic_symmetry(changed_representatives);
        changed_representatives = NULL;
    }
    return NULL;
}

/* (I, w) = (tmat, shift)^-1 (I, w_std) (tmat, shift) */
/* w_std = tmat @ w */
static VecDBL *get_changed_pure_translations(const double tmat[3][3],
                                             const VecDBL *pure_trans,
                                             const double symprec) {
    int size, count, denominator, i, n0, n1, n2, s, t, ok;
    double det;
    double trans_tmp[3], trans_shifted[3];
    VecDBL *changed_pure_trans;

    changed_pure_trans = NULL;
    count = 0;

    det = mat_get_determinant_d3(tmat);
    size = mat_Nint(pure_trans->size / det);

    if ((changed_pure_trans = mat_alloc_VecDBL(size)) == NULL) goto err;

    if (mat_Dabs(det - 1) <= symprec) {
        for (i = 0; i < pure_trans->size; i++) {
            mat_multiply_matrix_vector_d3(trans_tmp, tmat, pure_trans->vec[i]);
            for (s = 0; s < 3; s++) {
                trans_tmp[s] = mat_Dmod1(trans_tmp[s]);
            }
            mat_copy_vector_d3(changed_pure_trans->vec[count++], trans_tmp);
        }
    } else {
        /* Find least common denominator of elements in tmat */
        /* determinant of tmat may not be integer! */
        for (denominator = 1; denominator <= MAX_DENOMINATOR; denominator++) {
            ok = 1;
            for (s = 0; s < 3; s++) {
                for (t = 0; t < 3; t++) {
                    if (mat_Dabs(tmat[s][t] * denominator -
                                 mat_Nint(tmat[s][t] * denominator)) >
                        symprec) {
                        ok = 0;
                        break;
                    }
                }
            }
            if (ok) break;
        }
        debug_print("denominator=%d\n", denominator);

        /* Find unique translations after transformation */
        for (n0 = 0; n0 <= denominator; n0++) {
            for (n1 = 0; n1 <= denominator; n1++) {
                for (n2 = 0; n2 <= denominator; n2++) {
                    for (i = 0; i < pure_trans->size; i++) {
                        /* Try additional lattice points to recover translations
                         * in conventional cell */
                        trans_shifted[0] = pure_trans->vec[i][0] + n0;
                        trans_shifted[1] = pure_trans->vec[i][1] + n1;
                        trans_shifted[2] = pure_trans->vec[i][2] + n2;
                        mat_multiply_matrix_vector_d3(trans_tmp, tmat,
                                                      trans_shifted);
                        for (s = 0; s < 3; s++) {
                            trans_tmp[s] = mat_Dmod1(trans_tmp[s]);
                        }

                        if (is_contained_vec(trans_tmp, changed_pure_trans,
                                             count, symprec))
                            continue;
                        mat_copy_vector_d3(changed_pure_trans->vec[count++],
                                           trans_tmp);
                    }
                }
            }
        }
    }

    /* Sanity check */
    if (count != size) {
        warning_print(
            "spglib: Failed to find pure translations after transformation.\n");
        warning_print("Expect=%d, Actual=%d\n", size, count);
        goto err;
    }

    return changed_pure_trans;

err:
    if (changed_pure_trans != NULL) {
        mat_free_VecDBL(changed_pure_trans);
        changed_pure_trans = NULL;
    }
    return NULL;
}

/* Return 1 iff `v` is contained in `trans`. */
static int is_contained_vec(const double v[3], const VecDBL *trans,
                            const int size, const double symprec) {
    int i, s, equivalent;

    for (i = 0; i < size; i++) {
        equivalent = 1;
        for (s = 0; s < 3; s++) {
            if (mat_Dabs(v[s] - trans->vec[i][s]) >= symprec) {
                equivalent = 0;
                break;
            }
        }
        if (equivalent) return 1;
    }

    return 0;
}

/* Return 1 iff `a` is contained in `sym_msg->rot`. */
static int is_contained_mat(const int a[3][3], const MagneticSymmetry *sym_msg,
                            const int size) {
    int i;
    for (i = 0; i < size; i++) {
        if (mat_check_identity_matrix_i3(a, sym_msg->rot[i])) {
            return 1;
        }
    }
    return 0;
}

/* Transform magnetic symmetry operations by (tmat, shift) */
/* This function does not check duplicated operations after transformation. */
/* x_std = (tmat, shift) x */
/* (W, w) -> (tmat, shift) (W, w) (tmat, shift)^-1 */
static MagneticSymmetry *get_distinct_changed_magnetic_symmetry(
    const double tmat[3][3], const double shift[3],
    const MagneticSymmetry *sym_msg) {
    int i;
    MagneticSymmetry *changed;
    double tmatinv[3][3];
    double rot_tmp[3][3];
    double trans_tmp[3];

    changed = NULL;
    mat_inverse_matrix_d3(tmatinv, tmat, 0);

    if ((changed = sym_alloc_magnetic_symmetry(sym_msg->size)) == NULL)
        return NULL;

    for (i = 0; i < sym_msg->size; i++) {
        changed->timerev[i] = sym_msg->timerev[i];

        /* W_std = tmat @ W @ tmat^-1 */
        mat_multiply_matrix_di3(rot_tmp, tmat, sym_msg->rot[i]);
        mat_multiply_matrix_d3(rot_tmp, rot_tmp, tmatinv);
        mat_cast_matrix_3d_to_3i(changed->rot[i], rot_tmp);

        /* w_std = shift - W_std @ shift + tmat @ w */
        mat_copy_vector_d3(changed->trans[i], shift);
        mat_multiply_matrix_vector_id3(trans_tmp, changed->rot[i], shift);
        changed->trans[i][0] -= trans_tmp[0];
        changed->trans[i][1] -= trans_tmp[1];
        changed->trans[i][2] -= trans_tmp[2];
        mat_multiply_matrix_vector_d3(trans_tmp, tmat, sym_msg->trans[i]);
        changed->trans[i][0] += trans_tmp[0];
        changed->trans[i][1] += trans_tmp[1];
        changed->trans[i][2] += trans_tmp[2];
        changed->trans[i][0] = mat_Dmod1(changed->trans[i][0]);
        changed->trans[i][1] = mat_Dmod1(changed->trans[i][1]);
        changed->trans[i][2] = mat_Dmod1(changed->trans[i][2]);
    }

    return changed;
}

/* Return 1 if sym1 is isomorphic to sym2 */
static int is_equal(const MagneticSymmetry *sym1, const MagneticSymmetry *sym2,
                    const double symprec) {
    int i, j, found;

    if (sym1->size != sym2->size) return 0;

    for (i = 0; i < sym1->size; i++) {
        found = 0;
        for (j = 0; j < sym2->size; j++) {
            if (mat_check_identity_matrix_i3(sym1->rot[i], sym2->rot[j]) &&
                mat_Dmod1(sym1->trans[i][0] - sym2->trans[j][0]) < symprec &&
                mat_Dmod1(sym1->trans[i][1] - sym2->trans[j][1]) < symprec &&
                mat_Dmod1(sym1->trans[i][2] - sym2->trans[j][2]) < symprec &&
                sym1->timerev[i] == sym2->timerev[j]) {
                found = 1;
                debug_print("sym1[%d] -> sym2[%d]\n", i, j);
                break;
            }
        }

        if (!found) return 0;
    }

    return 1;
}

void get_rigid_rotation(double rigid_rot[3][3], const double lattice[3][3],
                        const double tmat[3][3], const Spacegroup *ref_sg) {
    double ideal_latt[3][3], inv_ideal_latt[3][3], inv_latt[3][3];
    double tmat_bravais[3][3], inv_tmat_bravais[3][3];

    /* Refine lattice with metric tensor */
    ref_get_conventional_lattice(ideal_latt, ref_sg);
    mat_inverse_matrix_d3(inv_ideal_latt, ideal_latt, 0);
    mat_multiply_matrix_d3(tmat_bravais, lattice, inv_ideal_latt);

    /* Now, x_std = (tmat, -shift) x */
    /*      (a_std, b_std, c_std) = (a, b, c) @ tmat^-1 */
    /*      x_std' = (tmat_bravais, -shift) x */
    /*      (a_std', b_std', c_std') = (a, b, c) @ tmat_bravais^-1 */
    debug_print("Transformation (ref)\n");
    debug_print_matrix_d3(tmat_bravais);
    debug_print("det = %f\n", mat_get_determinant_d3(tmat_bravais));

    /* (a_std', b_std', c_std') = rigid_rot @ (a_std, b_std, c_std) */
    /* => (a, b, c) @ tmat_bravais^-1 = rigid_rot @ (a, b, c) @ tmat^-1 */
    /* => rigid_rot = (a, b, c) @ tmat_bravais^-1 @ tmat @ (a, b, c)^-1 */
    mat_inverse_matrix_d3(inv_latt, lattice, 0);
    mat_inverse_matrix_d3(inv_tmat_bravais, tmat, 0);
    mat_multiply_matrix_d3(rigid_rot, lattice, inv_tmat_bravais);
    mat_multiply_matrix_d3(rigid_rot, rigid_rot, tmat);
    mat_multiply_matrix_d3(rigid_rot, rigid_rot, inv_latt);

    debug_print("Rigid rotation\n");
    debug_print_matrix_d3(rigid_rot);
    debug_print("det = %f\n", mat_get_determinant_d3(rigid_rot));
}
