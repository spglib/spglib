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

#include "debug.h"

#include <stdlib.h>

static int get_family_space_group_with_magnetic_symmetry(
    Spacegroup **fsg, const MagneticSymmetry *magnetic_symmetry,
    const double symprec);
static int get_maximal_subspace_group_with_magnetic_symmetry(
    Spacegroup **xsg, const MagneticSymmetry *magnetic_symmetry,
    const double symprec);
static int get_space_group_with_magnetic_symmetry(
    Spacegroup **spacegroup, const MagneticSymmetry *magnetic_symmetry,
    const int ignore_time_reversal, const double symprec);
static MagneticSymmetry *get_representative(
    const MagneticSymmetry *magnetic_symmetry, const double symprec);

/* Get family space group (FSG) and return its order. */
/* FSG is a non-magnetic space group obtained by ignoring primes in operations.
 */
/* If failed, return 0. */
int get_family_space_group_with_magnetic_symmetry(
    Spacegroup **fsg, const MagneticSymmetry *magnetic_symmetry,
    const double symprec) {
    return get_space_group_with_magnetic_symmetry(fsg, magnetic_symmetry, 1,
                                                  symprec);
}

/* Get maximal subspace group (XSG) and return its order. */
/* XSG is a space group obtained by removing primed operations. */
/* If failed, return 0. */
int get_maximal_subspace_group_with_magnetic_symmetry(
    Spacegroup **xsg, const MagneticSymmetry *magnetic_symmetry,
    const double symprec) {
    return get_space_group_with_magnetic_symmetry(xsg, magnetic_symmetry, 0,
                                                  symprec);
}

/* Return type of MSG. If failed, return 0. */
int msg_get_magnetic_space_group_type(Spacegroup **fsg, Spacegroup **xsg,
                                      MagneticSymmetry **representative,
                                      const MagneticSymmetry *magnetic_symmetry,
                                      const double symprec) {
    int num_sym_fsg, num_sym_xsg, num_sym_msg, flag;
    int msg_type;
    int identity[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

    num_sym_msg = magnetic_symmetry->size;

    /* Identify family space group (FSG) and maximal space group (XSG) */
    /* TODO(shinohara): add option to specify hall_number in searching
     * space-group type */
    num_sym_fsg = get_family_space_group_with_magnetic_symmetry(
        fsg, magnetic_symmetry, symprec);
    num_sym_xsg = get_maximal_subspace_group_with_magnetic_symmetry(
        xsg, magnetic_symmetry, symprec);
    if (num_sym_fsg == 0 || num_sym_xsg == 0) return 0;

    debug_print("MSG: order=%d\n", num_sym_msg);
    debug_print("FSG: hall_number=%d, international=%s, order=%d\n",
                (*fsg)->hall_number, (*fsg)->international_short, num_sym_fsg);
    debug_print("XSG: hall_number=%d, international=%s, order=%d\n",
                (*xsg)->hall_number, (*xsg)->international_short, num_sym_xsg);

    if (num_sym_fsg == num_sym_xsg) {
        if ((*representative = sym_alloc_magnetic_symmetry(1)) == NULL)
            return 0;

        mat_copy_matrix_i3((*representative)->rot[0], identity);
        (*representative)->trans[0][0] = 0;
        (*representative)->trans[0][1] = 0;
        (*representative)->trans[0][2] = 0;

        if (num_sym_msg == num_sym_fsg) {
            (*representative)->timerev[0] = 0;
            msg_type = 1; /* Type-I */
        } else if (num_sym_msg == 2 * num_sym_fsg) {
            (*representative)->timerev[0] = 1;
            msg_type = 2; /* Type-II */
        } else {
            /* Unreachable */
            return 0;
        }
    } else if (num_sym_fsg == 2 * num_sym_xsg) {
        *representative = get_representative(magnetic_symmetry, symprec);
        if (*representative == NULL) return 0;

        /* If primed operation is translation, type-IV. Otherwise, type-III. */
        if (mat_check_identity_matrix_i3(identity, (*representative)->rot[0])) {
            msg_type = 4; /* Type-IV */
        } else {
            msg_type = 3; /* Type-III */
        }
    } else {
        /* Unreachable */
        return 0;
    }

    return msg_type;
}

/******************************************************************************/
/* Local functions                                                            */
/******************************************************************************/

/* ignore_time_reversal=true  -> FSG */
/* ignore_time_reversal=false -> XSG */
static int get_space_group_with_magnetic_symmetry(
    Spacegroup **spacegroup, const MagneticSymmetry *magnetic_symmetry,
    const int ignore_time_reversal, const double symprec) {
    int i, num_sym_msg, num_sym, is_type2;
    Symmetry *sym, *prim_sym;
    int identity[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

    sym = NULL;
    prim_sym = NULL;
    num_sym_msg = magnetic_symmetry->size;

    if ((sym = sym_alloc_symmetry(num_sym_msg)) == NULL) {
        return 0;
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
    sym_free_symmetry(sym);
    sym = NULL;

    *spacegroup = spa_search_spacegroup_with_symmetry(prim_sym, symprec);

    sym_free_symmetry(prim_sym);
    prim_sym = NULL;

    if (spacegroup == NULL) {
        return 0;
    }

    return num_sym;
}

/* Get coset representative of XSG in MSG. */
/* Assume MSG is type-III or type-IV. */
/* If failed, return NULL. */
static MagneticSymmetry *get_representative(
    const MagneticSymmetry *magnetic_symmetry, const double symprec) {
    int i, j, k, num_pure_trans, contained;
    MagneticSymmetry *representative;
    int identity[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

    representative = NULL;

    if ((representative = sym_alloc_magnetic_symmetry(1)) == NULL) {
        return NULL;
    }

    /* If primed operation with identity linear part has translations other */
    /* than centering vectors, MSG is type-IV. */
    for (i = 0; i < magnetic_symmetry->size; i++) {
        if (!magnetic_symmetry->timerev[i]) {
            continue;
        }

        mat_copy_matrix_i3(representative->rot[0], magnetic_symmetry->rot[i]);
        mat_copy_vector_d3(representative->trans[0],
                           magnetic_symmetry->trans[i]);
        representative->timerev[0] = 1;
        break;
    }
    return representative;
}
