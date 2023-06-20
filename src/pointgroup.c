/* Copyright (C) 2008 Atsushi Togo */
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

#include "pointgroup.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "mathfunc.h"
#include "symmetry.h"

#define NUM_ROT_AXES 73
#define ZERO_PREC 1e-10

typedef struct {
    int table[10];
    char symbol[6];
    char schoenflies[4];
    Holohedry holohedry;
    Laue laue;
} __attribute__((aligned(64))) __attribute__((packed)) PointgroupType;

static const PointgroupType pointgroup_data[33] = {
    {
        /* 0 */
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        "     ",
        "   ",
        HOLOHEDRY_NONE,
        LAUE_NONE,
    },
    {
        /* 1 */
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        "1    ",
        "C1 ",
        TRICLI,
        LAUE1,
    },
    {
        /* 2 */
        {0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
        "-1   ",
        "Ci ",
        TRICLI,
        LAUE1,
    },
    {
        /* 3 */
        {0, 0, 0, 0, 0, 1, 1, 0, 0, 0},
        "2    ",
        "C2 ",
        MONOCLI,
        LAUE2M,
    },
    {
        /* 4 */
        {0, 0, 0, 1, 0, 1, 0, 0, 0, 0},
        "m    ",
        "Cs ",
        MONOCLI,
        LAUE2M,
    },
    {
        /* 5 */
        {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
        "2/m  ",
        "C2h",
        MONOCLI,
        LAUE2M,
    },
    {
        /* 6 */
        {0, 0, 0, 0, 0, 1, 3, 0, 0, 0},
        "222  ",
        "D2 ",
        ORTHO,
        LAUEMMM,
    },
    {
        /* 7 */
        {0, 0, 0, 2, 0, 1, 1, 0, 0, 0},
        "mm2  ",
        "C2v",
        ORTHO,
        LAUEMMM,
    },
    {
        /* 8 */
        {0, 0, 0, 3, 1, 1, 3, 0, 0, 0},
        "mmm  ",
        "D2h",
        ORTHO,
        LAUEMMM,
    },
    {
        /* 9 */
        {0, 0, 0, 0, 0, 1, 1, 0, 2, 0},
        "4    ",
        "C4 ",
        TETRA,
        LAUE4M,
    },
    {
        /* 10 */
        {0, 2, 0, 0, 0, 1, 1, 0, 0, 0},
        "-4   ",
        "S4 ",
        TETRA,
        LAUE4M,
    },
    {
        /* 11 */
        {0, 2, 0, 1, 1, 1, 1, 0, 2, 0},
        "4/m  ",
        "C4h",
        TETRA,
        LAUE4M,
    },
    {
        /* 12 */
        {0, 0, 0, 0, 0, 1, 5, 0, 2, 0},
        "422  ",
        "D4 ",
        TETRA,
        LAUE4MMM,
    },
    {
        /* 13 */
        {0, 0, 0, 4, 0, 1, 1, 0, 2, 0},
        "4mm  ",
        "C4v",
        TETRA,
        LAUE4MMM,
    },
    {
        /* 14 */
        {0, 2, 0, 2, 0, 1, 3, 0, 0, 0},
        "-42m ",
        "D2d",
        TETRA,
        LAUE4MMM,
    },
    {
        /* 15 */
        {0, 2, 0, 5, 1, 1, 5, 0, 2, 0},
        "4/mmm",
        "D4h",
        TETRA,
        LAUE4MMM,
    },
    {
        /* 16 */
        {0, 0, 0, 0, 0, 1, 0, 2, 0, 0},
        "3    ",
        "C3 ",
        TRIGO,
        LAUE3,
    },
    {
        /* 17 */
        {0, 0, 2, 0, 1, 1, 0, 2, 0, 0},
        "-3   ",
        "C3i",
        TRIGO,
        LAUE3,
    },
    {
        /* 18 */
        {0, 0, 0, 0, 0, 1, 3, 2, 0, 0},
        "32   ",
        "D3 ",
        TRIGO,
        LAUE3M,
    },
    {
        /* 19 */
        {0, 0, 0, 3, 0, 1, 0, 2, 0, 0},
        "3m   ",
        "C3v",
        TRIGO,
        LAUE3M,
    },
    {
        /* 20 */
        {0, 0, 2, 3, 1, 1, 3, 2, 0, 0},
        "-3m  ",
        "D3d",
        TRIGO,
        LAUE3M,
    },
    {
        /* 21 */
        {0, 0, 0, 0, 0, 1, 1, 2, 0, 2},
        "6    ",
        "C6 ",
        HEXA,
        LAUE6M,
    },
    {
        /* 22 */
        {2, 0, 0, 1, 0, 1, 0, 2, 0, 0},
        "-6   ",
        "C3h",
        HEXA,
        LAUE6M,
    },
    {
        /* 23 */
        {2, 0, 2, 1, 1, 1, 1, 2, 0, 2},
        "6/m  ",
        "C6h",
        HEXA,
        LAUE6M,
    },
    {
        /* 24 */
        {0, 0, 0, 0, 0, 1, 7, 2, 0, 2},
        "622  ",
        "D6 ",
        HEXA,
        LAUE6MMM,
    },
    {
        /* 25 */
        {0, 0, 0, 6, 0, 1, 1, 2, 0, 2},
        "6mm  ",
        "C6v",
        HEXA,
        LAUE6MMM,
    },
    {
        /* 26 */
        {2, 0, 0, 4, 0, 1, 3, 2, 0, 0},
        "-6m2 ",
        "D3h",
        HEXA,
        LAUE6MMM,
    },
    {
        /* 27 */
        {2, 0, 2, 7, 1, 1, 7, 2, 0, 2},
        "6/mmm",
        "D6h",
        HEXA,
        LAUE6MMM,
    },
    {
        /* 28 */
        {0, 0, 0, 0, 0, 1, 3, 8, 0, 0},
        "23   ",
        "T  ",
        CUBIC,
        LAUEM3,
    },
    {
        /* 29 */
        {0, 0, 8, 3, 1, 1, 3, 8, 0, 0},
        "m-3  ",
        "Th ",
        CUBIC,
        LAUEM3,
    },
    {
        /* 30 */
        {0, 0, 0, 0, 0, 1, 9, 8, 6, 0},
        "432  ",
        "O  ",
        CUBIC,
        LAUEM3M,
    },
    {
        /* 31 */
        {0, 6, 0, 6, 0, 1, 3, 8, 0, 0},
        "-43m ",
        "Td ",
        CUBIC,
        LAUEM3M,
    },
    {
        /* 32 */
        {0, 6, 8, 9, 1, 1, 9, 8, 6, 0},
        "m-3m ",
        "Oh ",
        CUBIC,
        LAUEM3M,
    }};

static const int identity[3][3] = {
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1},
};

static const int inversion[3][3] = {
    {-1, 0, 0},
    {0, -1, 0},
    {0, 0, -1},
};

// length of `rot_axes` should be equal to `NUM_ROT_AXES`
static const int rot_axes[][3] = {
    {1, 0, 0},   {0, 1, 0},   {0, 0, 1},   {0, 1, 1},   {1, 0, 1},
    {1, 1, 0},   {0, 1, -1},  {-1, 0, 1},  {1, -1, 0},  {1, 1, 1}, /* 10 */
    {-1, 1, 1},  {1, -1, 1},  {1, 1, -1},  {0, 1, 2},   {2, 0, 1},
    {1, 2, 0},   {0, 2, 1},   {1, 0, 2},   {2, 1, 0},   {0, -1, 2}, /* 20 */
    {2, 0, -1},  {-1, 2, 0},  {0, -2, 1},  {1, 0, -2},  {-2, 1, 0},
    {2, 1, 1},   {1, 2, 1},   {1, 1, 2},   {2, -1, -1}, {-1, 2, -1}, /* 30 */
    {-1, -1, 2}, {2, 1, -1},  {-1, 2, 1},  {1, -1, 2},  {2, -1, 1},  /* 35 */
    {1, 2, -1},  {-1, 1, 2},  {3, 1, 2},   {2, 3, 1},   {1, 2, 3},   /* 40 */
    {3, 2, 1},   {1, 3, 2},   {2, 1, 3},   {3, -1, 2},  {2, 3, -1},  /* 45 */
    {-1, 2, 3},  {3, -2, 1},  {1, 3, -2},  {-2, 1, 3},  {3, -1, -2}, /* 50 */
    {-2, 3, -1}, {-1, -2, 3}, {3, -2, -1}, {-1, 3, -2}, {-2, -1, 3}, /* 55 */
    {3, 1, -2},  {-2, 3, 1},  {1, -2, 3},  {3, 2, -1},  {-1, 3, 2},  /* 60 */
    {2, -1, 3},  {1, 1, 3},   {-1, 1, 3},  {1, -1, 3},  {-1, -1, 3}, /* 65 */
    {1, 3, 1},   {-1, 3, 1},  {1, 3, -1},  {-1, 3, -1}, {3, 1, 1},   /* 70 */
    {3, 1, -1},  {3, -1, 1},  {3, -1, -1},
};

static int get_pointgroup_number_by_rotations(const int rotations[][3][3],
                                              int num_rotations);
static int get_pointgroup_number(const PointSymmetry* pointsym);
static int get_pointgroup_class_table(int table[10],
                                      const PointSymmetry* pointsym);
static int get_rotation_type(const int rot[3][3]);
static int get_rotation_axis(const int rot[3][3]);
static int get_orthogonal_axis(int ortho_axes[], const int proper_rot[3][3],
                               int rot_order);
static int laue2m(int axes[3], const PointSymmetry* pointsym);
static int layer_laue2m(int axes[3], const PointSymmetry* pointsym,
                        int aperiodic_axis);

static int laue_one_axis(int axes[3], const PointSymmetry* pointsym,
                         int rot_order);
static int lauennn(int axes[3], const PointSymmetry* pointsym, int rot_order,
                   int aperiodic_axis);
static int get_axes(int axes[3], Laue laue, const PointSymmetry* pointsym,
                    int aperiodic_axis);
static void get_proper_rotation(int prop_rot[3][3], const int rot[3][3]);
static void set_transformation_matrix(int tmat[3][3], const int axes[3]);
static int is_exist_axis(const int axis_vec[3], int axis_index);
static void sort_axes(int axes[3]);
static void layer_check_and_sort_axes(int axes[3], int aperiodic_axis);

Pointgroup ptg_get_transformation_matrix(int transform_mat[3][3],
                                         const int rotations[][3][3],
                                         const int num_rotations,
                                         const int aperiodic_axis) {
    int axes[3];
    PointSymmetry pointsym;
    Pointgroup pointgroup;

    debug_print("ptg_get_transformation_matrix:\n");

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            transform_mat[i][j] = 0;
        }
    }

    int pg_num = get_pointgroup_number_by_rotations(rotations, num_rotations);

    // Layer group does not have cubic point groups (28 <= pg_num < 32)
    if (pg_num > 0 && (aperiodic_axis == -1 || pg_num < 28)) {
        // Based on step (c) of R. W. Grosse-Kunstleve (1999)
        pointgroup = ptg_get_pointgroup(pg_num);
        pointsym = ptg_get_pointsymmetry(rotations, num_rotations);
        get_axes(axes, pointgroup.laue, &pointsym, aperiodic_axis);
        set_transformation_matrix(transform_mat, axes);
    } else {
        warning_print("spglib: No point group was found ");
        warning_print("(line %d, %s).\n", __LINE__, __FILE__);
        pointgroup.number = 0;
    }

    return pointgroup;
}

Pointgroup ptg_get_pointgroup(const int pointgroup_number) {
    Pointgroup pointgroup;
    PointgroupType pointgroup_type;

    pointgroup.number = pointgroup_number;
    pointgroup_type = pointgroup_data[pointgroup_number];
    memcpy(pointgroup.symbol, pointgroup_type.symbol, 6);
    memcpy(pointgroup.schoenflies, pointgroup_type.schoenflies, 4);
    for (int i = 0; i < 5; i++) {
        if (pointgroup.symbol[i] == ' ') {
            pointgroup.symbol[i] = '\0';
        }
    }
    for (int i = 0; i < 3; i++) {
        if (pointgroup.schoenflies[i] == ' ') {
            pointgroup.schoenflies[i] = '\0';
        }
    }
    pointgroup.holohedry = pointgroup_type.holohedry;
    pointgroup.laue = pointgroup_type.laue;

    debug_print("ptg_get_pointgroup: %s\n", pointgroup_type.symbol);

    return pointgroup;
}

// @brief Construct PointSymmetry from given `rotations`
PointSymmetry ptg_get_pointsymmetry(const int rotations[][3][3],
                                    const int num_rotations) {
    PointSymmetry pointsym;

    pointsym.size = 0;
    for (int i = 0; i < num_rotations; i++) {
        for (int j = 0; j < pointsym.size; j++) {
            if (mat_check_identity_matrix_i3(rotations[i], pointsym.rot[j])) {
                goto escape;
            }
        }
        mat_copy_matrix_i3(pointsym.rot[pointsym.size], rotations[i]);
        pointsym.size++;
    escape:;
    }

    return pointsym;
}

static int get_pointgroup_number_by_rotations(const int rotations[][3][3],
                                              const int num_rotations) {
    PointSymmetry pointsym = ptg_get_pointsymmetry(rotations, num_rotations);
    return get_pointgroup_number(&pointsym);
}

static int get_pointgroup_number(const PointSymmetry* pointsym) {
    int table[10];
    PointgroupType pointgroup_type;

    debug_print("get_pointgroup_number: ");

    /* Get list of point symmetry operations */
    if (!get_pointgroup_class_table(table, pointsym)) {
        debug_print("get_pointgroup_class_table returned 0\n");
        return 0;
    }

    for (int i = 1; i < 33; i++) {
        int counter = 0;
        pointgroup_type = pointgroup_data[i];
        for (int j = 0; j < 10; j++) {
            if (pointgroup_type.table[j] == table[j]) {
                counter++;
            }
        }
        if (counter == 10) {
            debug_print("%d\n", i);
            return i;
        }
    }
    debug_print("0\n");
    return 0;
}

// @brief Return counts of types of rotations
// @param[out] table counts of {-6, -4, -3, -2, 1, 1, 2, 3, 4, 6}-fold rotations
// or roto-rotations
// @param[in] pointsym
static int get_pointgroup_class_table(int table[10],
                                      const PointSymmetry* pointsym) {
    /* Look-up table */
    /* Operation   -6 -4 -3 -2 -1  1  2  3  4  6 */
    /* Trace     -  2 -1  0  1 -3  3 -1  0  1  2 */
    /* Determinant -1 -1 -1 -1 -1  1  1  1  1  1 */

    /* table[0] = -6 axis */
    /* table[1] = -4 axis */
    /* table[2] = -3 axis */
    /* table[3] = -2 axis */
    /* table[4] = -1 axis */
    /* table[5] =  1 axis */
    /* table[6] =  2 axis */
    /* table[7] =  3 axis */
    /* table[8] =  4 axis */
    /* table[9] =  6 axis */

    for (int i = 0; i < 10; i++) {
        table[i] = 0;
    }
    for (int i = 0; i < pointsym->size; i++) {
        int rot_type = get_rotation_type(pointsym->rot[i]);
        if (rot_type == -1) {
            warning_print("spglib: No point group symbol found ");
            warning_print("(line %d, %s).\n", __LINE__, __FILE__);
            return 0;
        }
        table[rot_type]++;
    }
    return 1;
}

static int get_rotation_type(const int rot[3][3]) {
    if (mat_get_determinant_i3(rot) == -1) {
        switch (mat_get_trace_i3(rot)) {
            case -2: /* -6 */
                return 0;
            case -1: /* -4 */
                return 1;
            case 0: /* -3 */
                return 2;
            case 1: /* -2 */
                return 3;
            case -3: /* -1 */
                return 4;
            default:
                return -1;
        }
    } else {
        switch (mat_get_trace_i3(rot)) {
            case 3: /* 1 */
                return 5;
            case -1: /* 2 */
                return 6;
            case 0: /* 3 */
                return 7;
            case 1: /* 4 */
                return 8;
            case 2: /* 6 */
                return 9;
            default:
                return -1;
        }
    }
}

// @brief Get conventional axes for point group `pointsym`
// @note For layer group, Laue class 2/m and mmm need a special care
// @param[out] axes three indices of `rot_axes`. If they are larger than
//             NUM_ROT_AXES, it represents the negative direction.
// @param[in] laue
// @param[in] pointsym
// @param[in] aperiodic_axis
static int get_axes(int axes[3], const Laue laue, const PointSymmetry* pointsym,
                    const int aperiodic_axis) {
    switch (laue) {
        case LAUE1:
            axes[0] = 0;  // {1, 0, 0}
            axes[1] = 1;  // {0, 1, 0}
            axes[2] = 2;  // {0, 0, 1}
            break;
        case LAUE2M:
            if (aperiodic_axis == -1) {
                laue2m(axes, pointsym);
            } else {
                layer_laue2m(axes, pointsym, aperiodic_axis);
            }
            break;
        case LAUEMMM:
            lauennn(axes, pointsym, 2, aperiodic_axis);
            break;
        case LAUE4M:
        case LAUE4MMM:
            laue_one_axis(axes, pointsym, 4);
            break;
        case LAUE3:
        case LAUE3M:
        case LAUE6M:
        case LAUE6MMM:
            laue_one_axis(axes, pointsym, 3);
            break;
        case LAUEM3:
            lauennn(axes, pointsym, 2, -1);
            break;
        case LAUEM3M:
            lauennn(axes, pointsym, 4, -1);
            break;
        default:
            break;
    }

    return 1;
}

static int laue2m(int axes[3], const PointSymmetry* pointsym) {
    int prop_rot[3][3];
    int ortho_axes[NUM_ROT_AXES];
    /* Uncomment to pass original POSCAR-14-227-47-ref,
     * POSCAR-15-230-prim-18-ref, POSCAR-15-bcc-18-ref */
    /* This has been done in 2D delaunay reduce
    int tmpval, t_mat[3][3];
    */

    for (int i = 0; i < pointsym->size; i++) {
        get_proper_rotation(prop_rot, pointsym->rot[i]);

        /* Search two-fold rotation */
        if (!(mat_get_trace_i3(prop_rot) == -1)) {
            continue;
        }

        /* The first axis (unique axis b) */
        axes[1] = get_rotation_axis(prop_rot);
        break;
    }

    /* The second axis */
    int num_ortho_axis = get_orthogonal_axis(ortho_axes, prop_rot, 2);
    if (!num_ortho_axis) {
        // Error could not get orthogonal axis
        return 0;
    }

    int min_norm = 8;
    int is_found = 0;
    for (int i = 0; i < num_ortho_axis; i++) {
        int norm = mat_norm_squared_i3(rot_axes[ortho_axes[i]]);
        if (norm < min_norm) {
            min_norm = norm;
            axes[0] = ortho_axes[i];
            is_found = 1;
        }
    }
    if (!is_found) {
        // Error not found
        return 0;
    }

    /* The third axis */
    min_norm = 8;
    is_found = 0;
    for (int i = 0; i < num_ortho_axis; i++) {
        int norm = mat_norm_squared_i3(rot_axes[ortho_axes[i]]);
        if ((norm < min_norm) && (ortho_axes[i] != axes[0])) {
            min_norm = norm;
            axes[2] = ortho_axes[i];
            is_found = 1;
        }
    }
    if (!is_found) {
        // Error not found
        return 0;
    }

    /* Uncomment to pass original POSCAR-14-227-47-ref,
     * POSCAR-15-230-prim-18-ref, POSCAR-15-bcc-18-ref */
    /* This has been done in 2D delaunay reduce
    set_transformation_matrix(t_mat, axes);
    if (mat_get_determinant_i3(t_mat) < 0) {
      tmpval = axes[0];
      axes[0] = axes[2];
      axes[2] = tmpval;
    }
    */

    return 1;
}

// @note The two-fold axis is set to axis-a. Axes b and c forms the periodic
// plane.
static int layer_laue2m(int axes[3], const PointSymmetry* pointsym,
                        const int aperiodic_axis) {
    int prop_rot[3][3];
    int ortho_axes[NUM_ROT_AXES];

    for (int i = 0; i < pointsym->size; i++) {
        get_proper_rotation(prop_rot, pointsym->rot[i]);

        /* Search two-fold rotation */
        if (!(mat_get_trace_i3(prop_rot) == -1)) {
            continue;
        }

        /* Monoclinic/Rectangular: The first axis is axis a according to ITE */
        /* Monoclinic/Oblique: The first axis is moved to c in
         * change_basis_monocli */
        axes[0] = get_rotation_axis(prop_rot);
        break;
    }

    int num_ortho_axis = get_orthogonal_axis(ortho_axes, prop_rot, 2);
    if (!num_ortho_axis) {
        return 0;
    }

    if (rot_axes[axes[0]][aperiodic_axis] == 1 ||
        rot_axes[axes[0]][aperiodic_axis] == -1) {
        /* For Monoclinic/Oblique. */
        /* The second axis */
        int min_norm = 8;
        int is_found = 0;
        for (int i = 0; i < num_ortho_axis; i++) {
            int norm = mat_norm_squared_i3(rot_axes[ortho_axes[i]]);
            if (norm < min_norm) {
                min_norm = norm;
                axes[1] = ortho_axes[i];
                is_found = 1;
            }
        }
        if (!is_found) {
            return 0;
        }

        /* The third axis */
        min_norm = 8;
        is_found = 0;
        for (int i = 0; i < num_ortho_axis; i++) {
            int norm = mat_norm_squared_i3(rot_axes[ortho_axes[i]]);
            if ((norm < min_norm) && (ortho_axes[i] != axes[1])) {
                min_norm = norm;
                axes[2] = ortho_axes[i];
                is_found = 1;
            }
        }
        if (!is_found) {
            return 0;
        }
    } else if (rot_axes[axes[0]][aperiodic_axis] == 0) {
        /* For Monoclinic/Rectangular. */
        /* The second axis in the periodic plane */
        int min_norm = 8;
        int is_found = 0;
        for (int i = 0; i < num_ortho_axis; i++) {
            if (rot_axes[ortho_axes[i]][aperiodic_axis] == 0) {
                int norm = mat_norm_squared_i3(rot_axes[ortho_axes[i]]);
                if (norm < min_norm) {
                    min_norm = norm;
                    axes[1] = ortho_axes[i];
                    is_found = 1;
                }
            }
        }
        if (!is_found) {
            return 0;
        }

        /* The third axis outside the periodic plane */
        min_norm = 8;
        is_found = 0;
        for (int i = 0; i < num_ortho_axis; i++) {
            if (rot_axes[ortho_axes[i]][aperiodic_axis] == 1 ||
                rot_axes[ortho_axes[i]][aperiodic_axis] == -1) {
                int norm = mat_norm_squared_i3(rot_axes[ortho_axes[i]]);
                if (norm < min_norm) {
                    min_norm = norm;
                    axes[2] = ortho_axes[i];
                    is_found = 1;
                }
            }
        }
        if (!is_found) {
            return 0;
        }
    } else
        return 0;

    /* det(t_mat) > 0 in 2D delaunay reduce */

    return 1;
}

// For Laue classes 4/m, 4/mmm, -3, -3/m, 6/m, 6/mmm
static int laue_one_axis(int axes[3], const PointSymmetry* pointsym,
                         const int rot_order) {
    int axis_vec[3], tmp_axes[3];
    int prop_rot[3][3], t_mat[3][3];
    int ortho_axes[NUM_ROT_AXES];

    debug_print("laue_one_axis with rot_order %d\n", rot_order);

    for (int i = 0; i < pointsym->size; i++) {
        get_proper_rotation(prop_rot, pointsym->rot[i]);

        /* Search four-fold rotation */
        if (rot_order == 4) {
            if (mat_get_trace_i3(prop_rot) == 1) {
                /* The first axis (c axis) */
                axes[2] = get_rotation_axis(prop_rot);
                break;
            }
        }

        /* Search three-fold rotation */
        if (rot_order == 3) {
            if (mat_get_trace_i3(prop_rot) == 0) {
                /* The first axis (c axis) */
                axes[2] = get_rotation_axis(prop_rot);
                break;
            }
        }
    }

    /* Candidates of the second axis */
    int num_ortho_axis = get_orthogonal_axis(ortho_axes, prop_rot, rot_order);
    if (!num_ortho_axis) {
        warning_print("spglib: Secondary axis is not found.");
        warning_print("(line %d, %s).\n", __LINE__, __FILE__);
        return 0;
    }

    tmp_axes[1] = -1;
    tmp_axes[2] = axes[2];
    for (int i = 0; i < num_ortho_axis; i++) {
        int is_found = 0;
        tmp_axes[0] = ortho_axes[i];
        mat_multiply_matrix_vector_i3(axis_vec, prop_rot,
                                      rot_axes[tmp_axes[0]]);
        for (int j = 0; j < num_ortho_axis; j++) {
            is_found = is_exist_axis(axis_vec, ortho_axes[j]);
            if (is_found == 1) {
                tmp_axes[1] = ortho_axes[j];
                break;
            }
            if (is_found == -1) {
                tmp_axes[1] = ortho_axes[j] + NUM_ROT_AXES;
                break;
            }
        }

        if (!is_found) {
            continue;
        }

        set_transformation_matrix(t_mat, tmp_axes);
        int det = abs(mat_get_determinant_i3(t_mat));
        if (det < 4) { /* to avoid F-center choice det=4 */
            axes[0] = tmp_axes[0];
            axes[1] = tmp_axes[1];
            break;
        }
    }

    set_transformation_matrix(t_mat, axes);
    if (mat_get_determinant_i3(t_mat) < 0) {
        int tmpval = axes[0];
        axes[0] = axes[1];
        axes[1] = tmpval;
    }

    debug_print("axes[0] = %d\n", axes[0]);
    debug_print("axes[1] = %d\n", axes[1]);
    debug_print("axes[2] = %d\n", axes[2]);

    return 1;
}

// For Laue classes mmm, m-3, m-3m
static int lauennn(int axes[3], const PointSymmetry* pointsym,
                   const int rot_order, const int aperiodic_axis) {
    int prop_rot[3][3];

    for (int i = 0; i < 3; i++) {
        axes[i] = -1;
    }

    for (int i = 0, count = 0; i < pointsym->size; i++) {
        get_proper_rotation(prop_rot, pointsym->rot[i]);

        /* Search two- or four-fold rotation */
        if ((mat_get_trace_i3(prop_rot) == -1 && rot_order == 2) ||
            (mat_get_trace_i3(prop_rot) == 1 && rot_order == 4)) {
            int axis = get_rotation_axis(prop_rot);
            if (!((axis == axes[0]) || (axis == axes[1]) ||
                  (axis == axes[2]))) {
                axes[count] = axis;
                count++;
            }
        }
    }

    if (aperiodic_axis == -1) {
        sort_axes(axes);
    } else {
        layer_check_and_sort_axes(axes, aperiodic_axis);
    }

    return 1;
}

static int get_rotation_axis(const int proper_rot[3][3]) {
    int vec[3];

    /* No specific axis for I and -I */
    if (mat_check_identity_matrix_i3(proper_rot, identity)) {
        warning_print("No specific axis for I and -I.\n");
        return -1;
    }

    /* Look for eigenvector = rotation axis */
    for (int i = 0; i < NUM_ROT_AXES; i++) {
        mat_multiply_matrix_vector_i3(vec, proper_rot, rot_axes[i]);
        if (vec[0] == rot_axes[i][0] && vec[1] == rot_axes[i][1] &&
            vec[2] == rot_axes[i][2]) {
            return i;
        }
    }

    debug_print("rotation axis could not be found.\n");
    return -1;
}

// Output indices of `rot_axes` orthogonal to the axis of `proper_rot`
// @param[out] ortho_axes indices of `rot_axes`
// @param[in] proper_rot Proper rotation
// @param[in] rot_order
// @return Size of orthogonal axes
static int get_orthogonal_axis(int ortho_axes[], const int proper_rot[3][3],
                               const int rot_order) {
    int vec[3];
    int sum_rot[3][3], rot[3][3];

    // sum_rot = proper_rot + ... + proper_rot^rot_order
    mat_copy_matrix_i3(sum_rot, identity);
    mat_copy_matrix_i3(rot, identity);
    for (int i = 0; i < rot_order - 1; i++) {
        mat_multiply_matrix_i3(rot, proper_rot, rot);
        mat_add_matrix_i3(sum_rot, rot, sum_rot);
    }

    int num_ortho_axis = 0;
    for (int i = 0; i < NUM_ROT_AXES; i++) {
        // Kernel of `sum_rot` is always orthogonal to the axis of `proper_rot`
        mat_multiply_matrix_vector_i3(vec, sum_rot, rot_axes[i]);
        if (vec[0] == 0 && vec[1] == 0 && vec[2] == 0) {
            ortho_axes[num_ortho_axis] = i;
            num_ortho_axis++;
        }
    }

    return num_ortho_axis;
}

// @brief If `rot` is improper, output `rot` with inversion.
static void get_proper_rotation(int prop_rot[3][3], const int rot[3][3]) {
    if (mat_get_determinant_i3(rot) == -1) {
        mat_multiply_matrix_i3(prop_rot, inversion, rot);
    } else {
        mat_copy_matrix_i3(prop_rot, rot);
    }
}

// @brief Construct `tmat` from given indices corresponding to `rot_axes`
static void set_transformation_matrix(int tmat[3][3], const int axes[3]) {
    int s[3];

    for (int i = 0; i < 3; i++)
        /* axes[i] + NUM_ROT_AXES means improper rotation. */
        s[i] = (axes[i] < NUM_ROT_AXES) ? 1 : -1;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            // NOLINTNEXTLINE(clang-analyzer-core.UndefinedBinaryOperatorResult)
            tmat[i][j] = s[j] * rot_axes[axes[j] % NUM_ROT_AXES][i];
}

// @brief Return 1 if axis_vec == rot_axes[axis_index],
//        -1 if axis_vec == -rot_axes[axis_index], otherwise return 0.
static int is_exist_axis(const int axis_vec[3], const int axis_index) {
    if ((axis_vec[0] == rot_axes[axis_index][0]) &&
        (axis_vec[1] == rot_axes[axis_index][1]) &&
        (axis_vec[2] == rot_axes[axis_index][2])) {
        return 1;
    }
    if ((axis_vec[0] == -rot_axes[axis_index][0]) &&
        (axis_vec[1] == -rot_axes[axis_index][1]) &&
        (axis_vec[2] == -rot_axes[axis_index][2])) {
        return -1;
    }
    return 0;
}

static void sort_axes(int axes[3]) {
    int t_mat[3][3];

    if (axes[1] > axes[2]) {
        int axis = axes[1];
        axes[1] = axes[2];
        axes[2] = axis;
    }

    if (axes[0] > axes[1]) {
        int axis = axes[0];
        axes[0] = axes[1];
        axes[1] = axis;
    }

    if (axes[1] > axes[2]) {
        int axis = axes[1];
        axes[1] = axes[2];
        axes[2] = axis;
    }

    set_transformation_matrix(t_mat, axes);
    if (mat_get_determinant_i3(t_mat) < 0) {
        int axis = axes[1];
        axes[1] = axes[2];
        axes[2] = axis;
    }
}

// Sort two-fold axes for Laue class mmm for layer group.
// Non-periodic axis is maintained to be c-axis.
static void layer_check_and_sort_axes(int axes[3], const int aperiodic_axis) {
    int t_mat[3][3];

    int lattice_rank = 0;
    int arank = 0;
    int axis_i = -1;
    int axis = -1;
    for (int i = 0; i < 3; i++) {
        if (rot_axes[axes[i]][aperiodic_axis] == 0) {
            lattice_rank++;
        } else if (rot_axes[axes[i]][aperiodic_axis] == 1 ||
                   rot_axes[axes[i]][aperiodic_axis] == -1) {
            axis = axes[i];
            axis_i = i;
            arank++;
        }
    }
    assert(axis_i > -1);
    assert(axis > -1);

    if (lattice_rank == 2 && arank == 1) {
        axes[axis_i] = axes[2];
        axes[2] = axis;

        set_transformation_matrix(t_mat, axes);
        if (mat_get_determinant_i3(t_mat) < 0) {
            axis = axes[0];
            axes[0] = axes[1];
            axes[1] = axis;
        }
    } else {
        // Warning when rot_axes[axes[i]][aperiodic_axis] == -2, -3, 2, 3
        // I am not sure if this would ever happen.
        warning_print("spglib: Invalid axes were found ");
        warning_print("(line %d, %s).\n", __LINE__, __FILE__);
        for (int i = 0; i < 3; i++) {
            axes[i] = 0;
        }
    }
}
