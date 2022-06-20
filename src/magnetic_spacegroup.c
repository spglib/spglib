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
#include "msg_database.h"
#include "refinement.h"
#include "spin.h"

#include "debug.h"

#include <stdlib.h>
#include <string.h>

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
    SPGCONST double tmat[3][3], SPGCONST double shift[3],
    const MagneticSymmetry *representative, const Symmetry *sym_xsg,
    const MagneticSymmetry *magnetic_symmetry, const double symprec);
static VecDBL *get_changed_pure_translations(SPGCONST double tmat[3][3],
                                             const VecDBL *pure_trans,
                                             const double symprec);
static int is_contained_vec(SPGCONST double v[3], const VecDBL *trans,
                            const int size, const double symprec);
static int is_contained_mat(SPGCONST int a[3][3],
                            const MagneticSymmetry *sym_msg, const int size);
static MagneticSymmetry *get_distinct_changed_magnetic_symmetry(
    SPGCONST double tmat[3][3], SPGCONST double shift[3],
    const MagneticSymmetry *sym_msg);
static int is_equal(const MagneticSymmetry *sym1, const MagneticSymmetry *sym2,
                    const double symprec);

/******************************************************************************/

/* If failed, return NULL. */
MagneticDataset *msg_identify_magnetic_space_group_type(
    const MagneticSymmetry *magnetic_symmetry, const double symprec) {
    int hall_number, uni_number, type, same, i;
    Spacegroup *fsg, *xsg;
    Symmetry *sym_fsg, *sym_xsg;
    MagneticSymmetry *representatives, *msg_uni, *changed_symmetry;
    MagneticSpacegroupType msgtype;
    MagneticDataset *ret;
    int uni_number_range[2];
    double tmat[3][3], tmat_bravais[3][3], rigid_rot[3][3];
    double shift[3];

    sym_fsg = NULL;
    sym_xsg = NULL;
    msg_uni = NULL;
    changed_symmetry = NULL;
    ret = NULL;

    /* Identify family space group (FSG) and maximal space group (XSG) */
    /* TODO(shinohara): add option to specify hall_number in searching
     * space-group type */
    sym_fsg = get_family_space_group_with_magnetic_symmetry(
        &fsg, magnetic_symmetry, symprec);
    sym_xsg = get_maximal_subspace_group_with_magnetic_symmetry(
        &xsg, magnetic_symmetry, symprec);
    if (sym_fsg == NULL || sym_xsg == NULL) goto err;
    debug_print("FSG: hall_number=%d, international=%s, order=%d\n",
                fsg->hall_number, fsg->international_short, sym_fsg->size);
    debug_print("XSG: hall_number=%d, international=%s, order=%d\n",
                xsg->hall_number, xsg->international_short, sym_xsg->size);

    /* Determine type of MSG and generator of factor group of MSG over XSG */
    type = get_magnetic_space_group_type(&representatives, magnetic_symmetry,
                                         sym_fsg->size, sym_xsg->size);

    /* Choose reference setting */
    /* For type-IV, use setting from Hall symbol of XSG. */
    /* For other types, use setting from Hall symbol of FSG. */
    /* transformation matrix `tmat` should be double not integer for
     * rhombohedral setting of trigonal space groups! */
    if (type == 4) {
        hall_number = xsg->hall_number;
        /* refine tmat and shift */
        ref_find_similar_bravais_lattice(xsg, symprec);
        mat_copy_matrix_d3(tmat, xsg->bravais_lattice);
        mat_copy_vector_d3(shift, xsg->origin_shift);
        ref_get_conventional_lattice(tmat_bravais, xsg);
    } else {
        hall_number = fsg->hall_number;
        /* refine tmat and shift */
        ref_find_similar_bravais_lattice(fsg, symprec);
        mat_copy_matrix_d3(tmat, fsg->bravais_lattice);
        mat_copy_vector_d3(shift, fsg->origin_shift);
        ref_get_conventional_lattice(tmat_bravais, fsg);
    }

    /* Rigid rotation to standardized lattice */
    /* tmat_bravais = rigid_rot @ tmat */
    ref_measure_rigid_rotation(rigid_rot, tmat, tmat_bravais);

    if ((changed_symmetry = get_changed_magnetic_symmetry(
             tmat, shift, representatives, sym_xsg, magnetic_symmetry,
             symprec)) == NULL)
        goto err;

    msgdb_get_uni_candidates(uni_number_range, hall_number);
    for (uni_number = uni_number_range[0]; uni_number <= uni_number_range[1];
         uni_number++) {
        msg_uni = msgdb_get_spacegroup_operations(uni_number, hall_number);
        same = is_equal(msg_uni, changed_symmetry, symprec);
        sym_free_magnetic_symmetry(msg_uni);
        msg_uni = NULL;
        if (same) break;
    }
    debug_print("UNI number: %d\n", uni_number);

    msgtype = msgdb_get_magnetic_spacegroup_type(uni_number);
    if (msgtype.type != type) {
        warning_print("Inconsistent MSG type detected: %d %d\n", msgtype.type,
                      type);
        goto err;
    }

    /* Set MagneticDataset */
    if ((ret = (MagneticDataset *)(malloc(sizeof(MagneticDataset)))) == NULL)
        goto err;

    ret->uni_number = msgtype.uni_number;
    ret->msg_type = msgtype.type;
    ret->hall_number = hall_number;
    mat_copy_matrix_d3(ret->transformation_matrix, tmat);
    mat_copy_vector_d3(ret->origin_shift, shift);
    mat_copy_matrix_d3(ret->std_rotation_matrix, rigid_rot);

    free(fsg);
    fsg = NULL;
    free(xsg);
    xsg = NULL;
    sym_free_symmetry(sym_fsg);
    sym_fsg = NULL;
    sym_free_symmetry(sym_xsg);
    sym_xsg = NULL;
    /* msg_uni is already freed. */
    sym_free_magnetic_symmetry(changed_symmetry);
    changed_symmetry = NULL;
    sym_free_magnetic_symmetry(representatives);
    representatives = NULL;

    return ret;

err:
    if (fsg != NULL) {
        free(fsg);
        fsg = NULL;
    }
    if (xsg != NULL) {
        free(xsg);
        xsg = NULL;
    }
    if (sym_fsg != NULL) {
        sym_free_symmetry(sym_fsg);
        sym_fsg = NULL;
    }
    if (sym_xsg != NULL) {
        sym_free_symmetry(sym_xsg);
        sym_xsg = NULL;
    }
    if (msg_uni != NULL) {
        sym_free_magnetic_symmetry(msg_uni);
        msg_uni = NULL;
    }
    if (changed_symmetry != NULL) {
        sym_free_magnetic_symmetry(changed_symmetry);
        changed_symmetry = NULL;
    }
    if (representatives != NULL) {
        sym_free_magnetic_symmetry(representatives);
        representatives = NULL;
    }
    if (ret != NULL) {
        free(ret);
        ret = NULL;
    }
}

/******************************************************************************/
/* Local functions                                                            */
/******************************************************************************/

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
static Symmetry *get_space_group_with_magnetic_symmetry(
    Spacegroup **spacegroup, const MagneticSymmetry *magnetic_symmetry,
    const int ignore_time_reversal, const double symprec) {
    int i, num_sym_msg, num_sym, is_type2;
    Symmetry *sym, *prim_sym;
    int identity[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

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

    prim_sym = prm_get_primitive_symmetry(sym, symprec);

    *spacegroup = spa_search_spacegroup_with_symmetry(prim_sym, symprec);

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
static MagneticSymmetry *get_changed_magnetic_symmetry(
    SPGCONST double tmat[3][3], SPGCONST double shift[3],
    const MagneticSymmetry *representatives, const Symmetry *sym_xsg,
    const MagneticSymmetry *magnetic_symmetry, const double symprec) {
    int size, num_factors, num_sym, i, j, k;
    VecDBL *pure_trans, *changed_pure_trans;
    MagneticSymmetry *changed, *factors, *changed_factors,
        *changed_representatives;
    double trans_tmp[3];

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

static VecDBL *get_changed_pure_translations(SPGCONST double tmat[3][3],
                                             const VecDBL *pure_trans,
                                             const double symprec) {
    int size, count, denominator, i, n0, n1, n2;
    double det;
    double trans_tmp[3], trans_shifted[3];
    VecDBL *changed_pure_trans;

    changed_pure_trans = NULL;
    count = 0;

    det = mat_get_determinant_d3(tmat);
    size = pure_trans->size * mat_Nint(1 / det);

    if ((changed_pure_trans = mat_alloc_VecDBL(size)) == NULL) return NULL;

    if (mat_Dabs(det - 1) <= symprec) {
        for (i = 0; i < pure_trans->size; i++) {
            mat_copy_vector_d3(changed_pure_trans->vec[count++],
                               pure_trans->vec[i]);
        }
    } else if (mat_Dabs(det) > 1 + symprec) {
        /* Find unique translations after transformation */
        for (i = 0; i < pure_trans->size; i++) {
            mat_multiply_matrix_vector_d3(trans_tmp, tmat, pure_trans->vec[i]);
            trans_tmp[0] = mat_Dmod1(trans_tmp[0]);
            trans_tmp[1] = mat_Dmod1(trans_tmp[1]);
            trans_tmp[2] = mat_Dmod1(trans_tmp[2]);

            if (is_contained_vec(trans_tmp, changed_pure_trans, count, symprec))
                continue;
            mat_copy_vector_d3(changed_pure_trans->vec[count++],
                               pure_trans->vec[i]);
        }
    } else {
        /* Unit cell is enlarged */
        denominator = (int)(1 / det + 1);
        for (n0 = 0; n0 < denominator; n0++) {
            for (n1 = 0; n1 < denominator; n1++) {
                for (n2 = 0; n2 < denominator; n2++) {
                    for (i = 0; i < pure_trans->size; i++) {
                        /* Try additional lattice points to recover translations
                         * in conventional cell */
                        trans_shifted[0] = pure_trans->vec[i][0] + n0;
                        trans_shifted[1] = pure_trans->vec[i][1] + n1;
                        trans_shifted[2] = pure_trans->vec[i][2] + n2;
                        mat_multiply_matrix_vector_d3(trans_tmp, tmat,
                                                      trans_shifted);
                        trans_tmp[0] = mat_Dmod1(trans_tmp[0]);
                        trans_tmp[1] = mat_Dmod1(trans_tmp[1]);
                        trans_tmp[2] = mat_Dmod1(trans_tmp[2]);

                        if (is_contained_vec(trans_tmp, changed_pure_trans,
                                             count, symprec))
                            continue;
                        mat_copy_vector_d3(changed_pure_trans->vec[count++],
                                           pure_trans->vec[i]);
                    }
                }
            }
        }
    }

    /* Sanity check */
    if (count != size) {
        warning_print(
            "spglib: Failed to find pure translations after transformation.");
        return NULL;
    }

    return changed_pure_trans;
}

/* Return 1 iff `v` is contained in `trans`. */
static int is_contained_vec(SPGCONST double v[3], const VecDBL *trans,
                            const int size, const double symprec) {
    int i;

    for (i = 0; i < size; i++) {
        if (mat_Dabs(v[0] - trans->vec[i][0]) < symprec &&
            mat_Dabs(v[1] - trans->vec[i][1]) < symprec &&
            mat_Dabs(v[2] - trans->vec[i][2]) < symprec) {
            return 1;
        }
    }

    return 0;
}

/* Return 1 iff `a` is contained in `sym_msg->rot`. */
static int is_contained_mat(SPGCONST int a[3][3],
                            const MagneticSymmetry *sym_msg, const int size) {
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
static MagneticSymmetry *get_distinct_changed_magnetic_symmetry(
    SPGCONST double tmat[3][3], SPGCONST double shift[3],
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
                break;
            }
        }

        if (!found) return 0;
    }

    return 1;
}
