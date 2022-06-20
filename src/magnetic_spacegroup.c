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

static int get_space_group_with_magnetic_symmetry(
    Spacegroup **spacegroup, const MagneticSymmetry *magnetic_symmetry,
    const int ignore_time_reversal, const double symprec);

/* Get family space group (FSG) and return its order. */
/* FSG is a non-magnetic space group obtained by ignoring primes in operations.
 */
/* If failed, return 0. */
int msg_get_family_space_group_with_magnetic_symmetry(
    Spacegroup **fsg, const MagneticSymmetry *magnetic_symmetry,
    const double symprec) {
    return get_space_group_with_magnetic_symmetry(fsg, magnetic_symmetry, 1,
                                                  symprec);
}

/* Get maximal subspace group (XSG) and return its order. */
/* XSG is a space group obtained by removing primed operations. */
/* If failed, return 0. */
int msg_get_maximal_subspace_group_with_magnetic_symmetry(
    Spacegroup **xsg, const MagneticSymmetry *magnetic_symmetry,
    const double symprec) {
    return get_space_group_with_magnetic_symmetry(xsg, magnetic_symmetry, 0,
                                                  symprec);
}

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

    /* If and only if MSG is type-II, it has pure time-reversal operation (I,
     * 0)1' */
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
