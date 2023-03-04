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

#ifndef __msg_H__
#define __msg_H__

#include "msg_database.h"
#include "spacegroup.h"
#include "symmetry.h"

typedef struct {
    /* Magnetic space-group type */
    int uni_number;
    int msg_type;
    int hall_number;
    /* Transformation to standardized setting */
    double transformation_matrix[3][3];
    double origin_shift[3];
    /* Rigid rotation to standardized lattice */
    double std_rotation_matrix[3][3];
} MagneticDataset;

MagneticDataset *msg_identify_magnetic_space_group_type(
    const double lattice[3][3], const MagneticSymmetry *magnetic_symmetry,
    const double symprec);
Cell *msg_get_transformed_cell(const Cell *cell, const double tmat[3][3],
                               const double origin_shift[3],
                               const double rigid_rot[3][3],
                               const MagneticSymmetry *magnetic_symmetry,
                               const double symprec,
                               const double angle_tolerance);

#endif /*__msg_H__ */
