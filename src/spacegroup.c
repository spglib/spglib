/* Copyright (C) 2010 Atsushi Togo */
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

#include "spacegroup.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cell.h"
#include "debug.h"
#include "delaunay.h"
#include "hall_symbol.h"
#include "mathfunc.h"
#include "niggli.h"
#include "pointgroup.h"
#include "primitive.h"
#include "spg_database.h"
#include "symmetry.h"

#define REDUCE_RATE 0.95
#define NUM_ATTEMPT 100
#define INT_PREC 0.1
#define ZERO_PREC 1e-10

static const double change_of_basis_monocli[48][3][3] = {
    {{1, 0, 0}, /* b  first turn; two axes are flipped in second turn */
     {0, 1, 0},
     {0, 0, 1}},
    {{0, 0, 1}, /* b */
     {0, -1, 0},
     {1, 0, 0}},
    {{0, 0, 1}, /* a */
     {1, 0, 0},
     {0, 1, 0}},
    {{1, 0, 0}, /* c */
     {0, 0, 1},
     {0, -1, 0}},
    {{0, 1, 0}, /* c */
     {0, 0, 1},
     {1, 0, 0}},
    {{0, -1, 0}, /* a */
     {1, 0, 0},
     {0, 0, 1}},
    {{-1, 0, 1}, /* b */
     {0, 1, 0},
     {-1, 0, 0}},
    {{1, 0, -1}, /* b */
     {0, -1, 0},
     {0, 0, -1}},
    {{0, 1, -1}, /* a */
     {1, 0, 0},
     {0, 0, -1}},
    {{-1, -1, 0}, /* c */
     {0, 0, 1},
     {-1, 0, 0}},
    {{1, -1, 0}, /* c */
     {0, 0, 1},
     {0, -1, 0}},
    {{0, 1, 1}, /* a */
     {1, 0, 0},
     {0, 1, 0}},
    {{0, 0, -1}, /* b */
     {0, 1, 0},
     {1, 0, -1}},
    {{-1, 0, 0}, /* b */
     {0, -1, 0},
     {-1, 0, 1}},
    {{0, -1, 0}, /* a */
     {1, 0, 0},
     {0, -1, 1}},
    {{0, 1, 0}, /* c */
     {0, 0, 1},
     {1, 1, 0}},
    {{-1, 0, 0}, /* c */
     {0, 0, 1},
     {-1, 1, 0}},
    {{0, 0, -1}, /* a */
     {1, 0, 0},
     {0, -1, -1}},
    {{1, 0, 0}, /* b  two axes are flipped to look for non-acute axes */
     {0, -1, 0},
     {0, 0, -1}},
    {{0, 0, -1}, /* b */
     {0, 1, 0},
     {1, 0, 0}},
    {{0, 0, 1}, /* a */
     {-1, 0, 0},
     {0, -1, 0}},
    {{-1, 0, 0}, /* c */
     {0, 0, -1},
     {0, -1, 0}},
    {{0, 1, 0}, /* c */
     {0, 0, -1},
     {-1, 0, 0}},
    {{0, 1, 0}, /* a */
     {-1, 0, 0},
     {0, 0, 1}},
    {{-1, 0, -1}, /* b */
     {0, -1, 0},
     {-1, 0, 0}},
    {{1, 0, 1}, /* b */
     {0, 1, 0},
     {0, 0, 1}},
    {{0, -1, -1}, /* a */
     {-1, 0, 0},
     {0, 0, -1}},
    {{1, -1, 0}, /* c */
     {0, 0, -1},
     {1, 0, 0}},
    {{-1, -1, 0}, /* c */
     {0, 0, -1},
     {0, -1, 0}},
    {{0, -1, 1}, /* a */
     {-1, 0, 0},
     {0, -1, 0}},
    {{0, 0, 1}, /* b */
     {0, -1, 0},
     {1, 0, 1}},
    {{-1, 0, 0}, /* b */
     {0, 1, 0},
     {-1, 0, -1}},
    {{0, 1, 0}, /* a */
     {-1, 0, 0},
     {0, 1, 1}},
    {{0, 1, 0}, /* c */
     {0, 0, -1},
     {-1, 1, 0}},
    {{1, 0, 0}, /* c */
     {0, 0, -1},
     {1, 1, 0}},
    {{0, 0, -1}, /* a */
     {-1, 0, 0},
     {0, 1, -1}},
    {{1, 0, 0}, /*  c1  Monoclinic/Oblique layer group; first turn */
     {0, 1, 0},
     {0, 0, 1}},
    {{0, 1, 0}, /* -c1 */
     {1, 0, 0},
     {0, 0, -1}},
    {{0, -1, 0}, /*  c2 */
     {1, -1, 0},
     {0, 0, 1}},
    {{-1, 0, 0}, /* -c2 */
     {-1, 1, 0},
     {0, 0, -1}},
    {{-1, 1, 0}, /*  c3 */
     {-1, 0, 0},
     {0, 0, 1}},
    {{1, -1, 0}, /* -c3 */
     {0, -1, 0},
     {0, 0, -1}},
    {{1, 0, 0}, /* -c1  Monoclinic/Oblique layer group; second turn */
     {0, -1, 0},
     {0, 0, -1}},
    {{0, -1, 0}, /*  c1 */
     {1, 0, 0},
     {0, 0, 1}},
    {{0, 1, 0}, /* -c2 */
     {1, 1, 0},
     {0, 0, -1}},
    {{-1, 0, 0}, /*  c2 */
     {-1, -1, 0},
     {0, 0, 1}},
    {{-1, -1, 0}, /* -c3 */
     {-1, 0, 0},
     {0, 0, -1}},
    {{1, 1, 0}, /*  c3 */
     {0, 1, 0},
     {0, 0, 1}},
};

/* Change of basis for Monoclinic/Rectangular layer group */
static const int index_monocli_rectang[8] = {0, 5, 7, 8, 18, 23, 25, 26};

static const Centering change_of_centering_monocli[36] = {
    C_FACE, /* first turn */
    A_FACE, B_FACE, B_FACE, A_FACE, C_FACE, A_FACE,
    C_FACE, C_FACE, A_FACE, B_FACE, B_FACE, BODY,
    BODY,   BODY,   BODY,   BODY,   BODY,   C_FACE, /* second turn */
    A_FACE, B_FACE, B_FACE, A_FACE, C_FACE, A_FACE,
    C_FACE, C_FACE, A_FACE, B_FACE, B_FACE, BODY,
    BODY,   BODY,   BODY,   BODY,   BODY};

static const int change_of_unique_axis_monocli[48] = {
    1, 1, 0, 2, 2, 0, 1, 1, 0, 2, 2, 0, 1, 1, 0, 2, 2, 0, 1, 1, 0, 2, 2, 0,
    1, 1, 0, 2, 2, 0, 1, 1, 0, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};

// Transformations to {abc, bca, cab, ba-c, a-cb, -cba}
static const double change_of_basis_ortho[6][3][3] = {
    {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}},  {{0, 0, 1}, {1, 0, 0}, {0, 1, 0}},
    {{0, 1, 0}, {0, 0, 1}, {1, 0, 0}},  {{0, 1, 0}, {1, 0, 0}, {0, 0, -1}},
    {{1, 0, 0}, {0, 0, 1}, {0, -1, 0}}, {{0, 0, 1}, {0, 1, 0}, {-1, 0, 0}}};

// Changed centerings of C-centering of abc to {abc, bca, cab, ba-c, a-cb, -cba}
static const Centering change_of_centering_ortho[6] = {C_FACE, B_FACE, A_FACE,
                                                       C_FACE, B_FACE, A_FACE};
// Index of c axis in the changed coordinates system.
static const int change_of_unique_axis_ortho[6] = {2, 1, 0, 2, 1, 0};

/* n_l : the index of L(g) in N_\epsilon(g) of SPG No.16-74 */
/* See ITA: Affine normalizer or highest symmetry Euclidean normalizer */
/* Previous implementation below was not correct for 67, 68, 73, 74, */
/* Cmma, Ccca, Ibca, Imma */
/* 6 / ((Number of hall symbols of each spg-type) / x) */
/* where x=1 and x=2 with without and with centring. */
/* 6, 2, 2, 6, 2, */
/* 2, 6, 6, 6, 2, 1, 2, 1, 1, 1, */
/* 1, 2, 1, 2, 2, 1, 2, 1, 1, 1, */
/* 1, 2, 2, 2, 2, 1, 6, 6, 2, 2, */
/* 1, 1, 1, 1, 2, 2, 1, 2, 2, 1, */
/* 3, 1, 1, 1, 2, 2, 1, 1, 6, 6, */
/* 6, 2, 3, 1 */
static const int num_axis_choices_ortho[59] = {
    6, 2, 2, 6, 2,                /* 16-20 */
    2, 6, 6, 6, 2, 1, 2, 1, 1, 1, /* 21-30 */
    1, 2, 1, 2, 2, 1, 2, 1, 1, 1, /* 31-40 */
    1, 2, 2, 2, 2, 1, 6, 6, 2, 2, /* 41-50 */
    1, 1, 1, 1, 2, 2, 1, 2, 2, 1, /* 51-60 */
    3, 1, 1, 1, 2, 2, 2, 2, 6, 6, /* 61-70 */
    6, 2, 6, 2};                  /* 71-74 */

/* Use n_l of spacegroup with c != a and c != b */
static const int layer_num_axis_choices_ortho[30] = {
    2, 1,                         /* 19-20 */
    2, 2, 2, 1, 2, 2, 1, 1, 1, 1, /* 21-30 */
    1, 1, 1, 1, 1, 1, 2, 1, 2, 1, /* 31-40 */
    1, 1, 1, 2, 1, 2, 2, 2};      /* 41-48 */

/* hR_to_hP is the inverse of rhombo_obverse and used until the */
/* commit  e0398ba9. But now this is included in */
/* change_of_basis_rhombo in a way of */
/* dot(change_of_basis_rhombo_hex,hR_to_hP). */
/* static double hR_to_hP[3][3] = {{ 1, 0, 1 }, */
/*                                 {-1, 1, 1 }, */
/*                                 { 0,-1, 1 }}; */

static const double change_of_basis_rhombo[6][3][3] = {
    {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}},    {{0, 0, 1}, {1, 0, 0}, {0, 1, 0}},
    {{0, 1, 0}, {0, 0, 1}, {1, 0, 0}},    {{0, 0, -1}, {0, -1, 0}, {-1, 0, 0}},
    {{0, -1, 0}, {-1, 0, 0}, {0, 0, -1}}, {{-1, 0, 0}, {0, 0, -1}, {0, -1, 0}}};

static const double change_of_basis_rhombo_hex[6][3][3] = {
    {{1, 0, 1}, {-1, 1, 1}, {0, -1, 1}},
    {{0, -1, 1}, {1, 0, 1}, {-1, 1, 1}},
    {{-1, 1, 1}, {0, -1, 1}, {1, 0, 1}},
    {{0, 1, -1}, {1, -1, -1}, {-1, 0, -1}},
    {{1, -1, -1}, {-1, 0, -1}, {0, 1, -1}},
    {{-1, 0, -1}, {0, 1, -1}, {1, -1, -1}}};

static const double change_of_basis_C4[8][3][3] = {
    {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}},    {{0, -1, 0}, {1, 0, 0}, {0, 0, 1}},
    {{-1, 0, 0}, {0, -1, 0}, {0, 0, 1}},  {{0, 1, 0}, {-1, 0, 0}, {0, 0, 1}},
    {{0, 1, 0}, {1, 0, 0}, {0, 0, -1}},   {{-1, 0, 0}, {0, 1, 0}, {0, 0, -1}},
    {{0, -1, 0}, {-1, 0, 0}, {0, 0, -1}}, {{1, 0, 0}, {0, -1, 0}, {0, 0, -1}}};

static const double change_of_basis_C6[12][3][3] = {{{1, 0, 0}, /* 1 */
                                                     {0, 1, 0},
                                                     {0, 0, 1}},
                                                    {{1, -1, 0}, /* 2 */
                                                     {1, 0, 0},
                                                     {0, 0, 1}},
                                                    {{0, -1, 0}, /* 3 */
                                                     {1, -1, 0},
                                                     {0, 0, 1}},
                                                    {{-1, 0, 0}, /* 4 */
                                                     {0, -1, 0},
                                                     {0, 0, 1}},
                                                    {{-1, 1, 0}, /* 5 */
                                                     {-1, 0, 0},
                                                     {0, 0, 1}},
                                                    {{0, 1, 0}, /* 6 */
                                                     {-1, 1, 0},
                                                     {0, 0, 1}},
                                                    {{0, 1, 0}, /* 7 */
                                                     {1, 0, 0},
                                                     {0, 0, -1}},
                                                    {{-1, 1, 0}, /* 8 */
                                                     {0, 1, 0},
                                                     {0, 0, -1}},
                                                    {{-1, 0, 0}, /* 9 */
                                                     {-1, 1, 0},
                                                     {0, 0, -1}},
                                                    {{0, -1, 0}, /* 10 */
                                                     {-1, 0, 0},
                                                     {0, 0, -1}},
                                                    {{1, -1, 0}, /* 11 */
                                                     {0, -1, 0},
                                                     {0, 0, -1}},
                                                    {{1, 0, 0}, /* 12 */
                                                     {1, -1, 0},
                                                     {0, 0, -1}}};

/* Removed after commit 67997654 */
/* static double change_of_basis_501[3][3] = {{ 0, 0, 1}, */
/*                                            { 0,-1, 0}, */
/*                                            { 1, 0, 0}}; */

static const int spacegroup_to_hall_number[230] = {
    1,   2,   3,   6,   9,   18,  21,  30,  39,  57,  60,  63,  72,  81,  90,
    108, 109, 112, 115, 116, 119, 122, 123, 124, 125, 128, 134, 137, 143, 149,
    155, 161, 164, 170, 173, 176, 182, 185, 191, 197, 203, 209, 212, 215, 218,
    221, 227, 228, 230, 233, 239, 245, 251, 257, 263, 266, 269, 275, 278, 284,
    290, 292, 298, 304, 310, 313, 316, 322, 334, 335, 337, 338, 341, 343, 349,
    350, 351, 352, 353, 354, 355, 356, 357, 358, 359, 361, 363, 364, 366, 367,
    368, 369, 370, 371, 372, 373, 374, 375, 376, 377, 378, 379, 380, 381, 382,
    383, 384, 385, 386, 387, 388, 389, 390, 391, 392, 393, 394, 395, 396, 397,
    398, 399, 400, 401, 402, 404, 406, 407, 408, 410, 412, 413, 414, 416, 418,
    419, 420, 422, 424, 425, 426, 428, 430, 431, 432, 433, 435, 436, 438, 439,
    440, 441, 442, 443, 444, 446, 447, 448, 449, 450, 452, 454, 455, 456, 457,
    458, 460, 462, 463, 464, 465, 466, 467, 468, 469, 470, 471, 472, 473, 474,
    475, 476, 477, 478, 479, 480, 481, 482, 483, 484, 485, 486, 487, 488, 489,
    490, 491, 492, 493, 494, 495, 497, 498, 500, 501, 502, 503, 504, 505, 506,
    507, 508, 509, 510, 511, 512, 513, 514, 515, 516, 517, 518, 520, 521, 523,
    524, 525, 527, 529, 530,
};

static const int layer_group_to_hall_number[116] = {
    -1,   -2,   -3,   -4,   -5,   -8,   -9,   -12,  -14,  -16,  -18,  -20,
    -22,  -24,  -26,  -28,  -30,  -32,  -34,  -35,  -37,  -38,  -39,  -40,
    -42,  -43,  -44,  -46,  -48,  -50,  -52,  -54,  -56,  -58,  -60,  -62,
    -64,  -65,  -67,  -68,  -70,  -72,  -74,  -76,  -77,  -79,  -80,  -81,
    -82,  -83,  -84,  -85,  -87,  -88,  -89,  -90,  -91,  -92,  -93,  -94,
    -95,  -96,  -98,  -99,  -101, -102, -103, -104, -105, -106, -107, -108,
    -109, -110, -111, -112, -113, -114, -115, -116,
};

static const double identity[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
static const double monocli_i2c[3][3] = {{1, 0, -1}, {0, 1, 0}, {1, 0, 0}};
static const double monocli_a2c[3][3] = {{0, 0, 1}, {0, -1, 0}, {1, 0, 0}};
static const double rhombo_obverse[3][3] = {{2. / 3, -1. / 3, -1. / 3},
                                            {1. / 3, 1. / 3, -2. / 3},
                                            {1. / 3, 1. / 3, 1. / 3}};
static const double rhomb_reverse[3][3] = {{1. / 3, -2. / 3, 1. / 3},
                                           {2. / 3, -1. / 3, -1. / 3},
                                           {1. / 3, 1. / 3, 1. / 3}};
static const double a2c[3][3] = {{0, 0, 1}, {1, 0, 0}, {0, 1, 0}};
static const double b2c[3][3] = {{0, 1, 0}, {0, 0, 1}, {1, 0, 0}};

static const double A_mat[3][3] = {
    {1, 0, 0}, {0, 1. / 2, -1. / 2}, {0, 1. / 2, 1. / 2}};
static const double C_mat[3][3] = {
    {1. / 2, 1. / 2, 0}, {-1. / 2, 1. / 2, 0}, {0, 0, 1}};
static const double R_mat[3][3] = {{2. / 3, -1. / 3, -1. / 3},
                                   {1. / 3, 1. / 3, -2. / 3},
                                   {1. / 3, 1. / 3, 1. / 3}};
static const double I_mat[3][3] = {{-1. / 2, 1. / 2, 1. / 2},
                                   {1. / 2, -1. / 2, 1. / 2},
                                   {1. / 2, 1. / 2, -1. / 2}};
static const double F_mat[3][3] = {
    {0, 1. / 2, 1. / 2}, {1. / 2, 0, 1. / 2}, {1. / 2, 1. / 2, 0}};

static Spacegroup *search_spacegroup_with_symmetry(
    const Primitive *primitive, const int candidates[], int num_candidates,
    const Symmetry *symmetry, double symprec, double angle_tolerance);
static Spacegroup *get_spacegroup(int hall_number, const double origin_shift[3],
                                  const double conv_lattice[3][3]);
static int iterative_search_hall_number(
    double origin_shift[3], double conv_lattice[3][3], const int candidates[],
    int num_candidates, const Primitive *primitive, const Symmetry *symmetry,
    double symprec, double angle_tolerance);
static int change_basis_tricli(int tmat_int[3][3],
                               const double conv_lattice[3][3],
                               const double primitive_lattice[3][3],
                               double symprec, int aperiodic_axis);
static int change_basis_monocli(int tmat_int[3][3],
                                const double conv_lattice[3][3],
                                const double primitive_lattice[3][3],
                                double symprec, int aperiodic_axis_prim);
static Symmetry *get_initial_conventional_symmetry(Centering centering,
                                                   const double tmat[3][3],
                                                   const Symmetry *symmetry);
static int search_hall_number(double origin_shift[3], double conv_lattice[3][3],
                              const int candidates[], int num_candidates,
                              const Primitive *primitive,
                              const Symmetry *symmetry, double symprec);
static int match_hall_symbol_db(double origin_shift[3],
                                double conv_lattice[3][3],
                                const double (*orig_lattice)[3],
                                int hall_number, int pointgroup_number,
                                Holohedry holohedry, Centering centering,
                                const Symmetry *symmetry, double symprec);
static int match_hall_symbol_db_monocli(
    double origin_shift[3], double conv_lattice[3][3],
    const double (*orig_lattice)[3], int hall_number, int group_number,
    Centering centering, const Symmetry *conv_symmetry, double symprec);
static int match_hall_symbol_db_monocli_in_loop(
    double origin_shift[3], double conv_lattice[3][3], double norms_squared[2],
    int change_of_basis_index, const double (*orig_lattice)[3], int check_norms,
    int hall_number, Centering centering, const Symmetry *conv_symmetry,
    double symprec);
static int match_hall_symbol_db_ortho(double origin_shift[3],
                                      double conv_lattice[3][3],
                                      const double (*orig_lattice)[3],
                                      int hall_number, Centering centering,
                                      const Symmetry *symmetry,
                                      int num_free_axes, double symprec);
static int match_hall_symbol_db_ortho_in_loop(
    double origin_shift[3], double lattice[3][3],
    const double (*orig_lattice)[3], int axis_choice_index, int hall_number,
    Centering centering, const Symmetry *symmetry, int num_free_axes,
    double symprec);
static int match_hall_symbol_db_cubic(double origin_shift[3],
                                      double conv_lattice[3][3],
                                      const double (*orig_lattice)[3],
                                      int hall_number, Centering centering,
                                      const Symmetry *conv_symmetry,
                                      double symprec);
static int match_hall_symbol_db_cubic_in_loop(
    double origin_shift[3], double conv_lattice[3][3],
    const double (*orig_lattice)[3], int i, int hall_number,
    Centering centering, const Symmetry *conv_symmetry, double symprec);
static int match_hall_symbol_db_rhombo(double origin_shift[3],
                                       double conv_lattice[3][3],
                                       const double (*orig_lattice)[3],
                                       int hall_number,
                                       const Symmetry *conv_symmetry,
                                       double symprec);
static int match_hall_symbol_db_others(
    double origin_shift[3], double conv_lattice[3][3],
    const double (*orig_lattice)[3], int hall_number, Centering centering,
    Holohedry holohedry, const Symmetry *conv_symmetry, double symprec);
static int match_hall_symbol_db_change_of_basis_loop(
    double origin_shift[3], double conv_lattice[3][3],
    const double (*orig_lattice)[3], const double (*change_of_basis)[3][3],
    int num_change_of_basis, int hall_number, Centering centering,
    const Symmetry *conv_symmetry, double symprec);
static Symmetry *get_conventional_symmetry(const double tmat[3][3],
                                           Centering centering,
                                           const Symmetry *primitive_sym);
static Centering get_centering(double correction_mat[3][3],
                               const int tmat[3][3], Laue laue);
static Centering get_base_center(const int tmat[3][3]);
static int get_centering_shifts(double shift[3][3], Centering centering);
static int is_equivalent_lattice(double tmat[3][3], int allow_flip,
                                 const double lattice[3][3],
                                 const double orig_lattice[3][3],
                                 double symprec);

/* Return NULL if failed */
Spacegroup *spa_search_spacegroup(const Primitive *primitive,
                                  const int hall_number, const double symprec,
                                  const double angle_tolerance) {
    int candidate[1];

    debug_print("search_spacegroup (tolerance = %f):\n", symprec);

    Symmetry *symmetry =
        sym_get_operation(primitive->cell, symprec, angle_tolerance);
    if (symmetry == NULL) {
        return NULL;
    }

    if (hall_number != 0) {
        candidate[0] = hall_number;
    }

    Spacegroup *spacegroup = NULL;
    if (hall_number) {
        spacegroup = search_spacegroup_with_symmetry(
            primitive, candidate, 1, symmetry, symprec, angle_tolerance);
    } else if (primitive->cell->aperiodic_axis == -1) {
        spacegroup = search_spacegroup_with_symmetry(
            primitive, spacegroup_to_hall_number, 230, symmetry, symprec,
            angle_tolerance);
    } else {
        spacegroup = search_spacegroup_with_symmetry(
            primitive, layer_group_to_hall_number, 116, symmetry, symprec,
            angle_tolerance);
    };

    sym_free_symmetry(symmetry);
    symmetry = NULL;

    return spacegroup;
}

/* Return NULL if failed */
/* Assume symmetry is transformed in primitive cell. */
Spacegroup *spa_search_spacegroup_with_symmetry(const Symmetry *symmetry,
                                                const double prim_lat[3][3],
                                                const double symprec) {
    Primitive *primitive = prm_alloc_primitive(1);
    if (primitive == NULL) {
        return 0;
    }
    primitive->cell = cel_alloc_cell(1, NOSPIN);
    if (primitive->cell == NULL) {
        prm_free_primitive(primitive);
        return 0;
    }
    mat_copy_matrix_d3(primitive->cell->lattice, prim_lat);
    for (int i = 0; i < 3; i++) {
        primitive->cell->position[0][i] = 0;
    }
    Spacegroup *spacegroup = search_spacegroup_with_symmetry(
        primitive, spacegroup_to_hall_number, 230, symmetry, symprec, -1.0);
    prm_free_primitive(primitive);
    primitive = NULL;

    return spacegroup;
}

/* Return NULL if failed */
Cell *spa_transform_to_primitive(int *mapping_table, const Cell *cell,
                                 const double trans_mat[3][3],
                                 const Centering centering,
                                 const double symprec) {
    double tmat[3][3], tmat_inv[3][3], prim_lat[3][3];
    if (!mat_inverse_matrix_d3(tmat_inv, trans_mat, symprec)) {
        return NULL;
    }

    switch (centering) {
        case PRIMITIVE:
            mat_copy_matrix_d3(tmat, tmat_inv);
            break;
        case A_FACE:
            mat_multiply_matrix_d3(tmat, tmat_inv, A_mat);
            break;
        case C_FACE:
            mat_multiply_matrix_d3(tmat, tmat_inv, C_mat);
            break;
        case FACE:
            mat_multiply_matrix_d3(tmat, tmat_inv, F_mat);
            break;
        case BODY:
            mat_multiply_matrix_d3(tmat, tmat_inv, I_mat);
            break;
        case R_CENTER:
            mat_multiply_matrix_d3(tmat, tmat_inv, R_mat);
            break;
        default:
            return NULL;
    }

    mat_multiply_matrix_d3(prim_lat, cell->lattice, tmat);
    Cell *primitive = cel_trim_cell(mapping_table, prim_lat, cell, symprec);

    if (primitive == NULL) {
        warning_print("spglib: cel_trim_cell failed.");
        warning_print(" (line %d, %s).\n", __LINE__, __FILE__);
    }

    return primitive;
}

/* Return NULL if failed */
Cell *spa_transform_from_primitive(const Cell *primitive,
                                   const Centering centering,
                                   const double symprec) {
    double tmat[3][3], inv_tmat[3][3], shift[3][3];
    switch (centering) {
        case PRIMITIVE:
            break;
        case A_FACE:
            mat_copy_matrix_d3(tmat, A_mat);
            mat_inverse_matrix_d3(inv_tmat, A_mat, 0);
            break;
        case C_FACE:
            mat_copy_matrix_d3(tmat, C_mat);
            mat_inverse_matrix_d3(inv_tmat, C_mat, 0);
            break;
        case FACE:
            mat_copy_matrix_d3(tmat, F_mat);
            mat_inverse_matrix_d3(inv_tmat, F_mat, 0);
            break;
        case BODY:
            mat_copy_matrix_d3(tmat, I_mat);
            mat_inverse_matrix_d3(inv_tmat, I_mat, 0);
            break;
        case R_CENTER:
            mat_copy_matrix_d3(tmat, R_mat);
            mat_inverse_matrix_d3(inv_tmat, R_mat, 0);
            break;
        default:
            return NULL;
    }

    int multi = get_centering_shifts(shift, centering);

    int *mapping_table = (int *)malloc(sizeof(int) * primitive->size * multi);
    if (mapping_table == NULL) {
        warning_print("spglib: Memory could not be allocated ");
        return NULL;
    }

    Cell *std_cell =
        cel_alloc_cell(primitive->size * multi, primitive->tensor_rank);
    if (std_cell == NULL) {
        free(mapping_table);
        mapping_table = NULL;
        return NULL;
    }

    mat_multiply_matrix_d3(std_cell->lattice, primitive->lattice, inv_tmat);

    int num_atom = 0;
    for (int i = 0; i < primitive->size; i++) {
        mat_multiply_matrix_vector_d3(std_cell->position[num_atom], tmat,
                                      primitive->position[i]);
        std_cell->types[num_atom] = primitive->types[i];
        num_atom++;
    }

    for (int i = 0; i < multi - 1; i++) {
        for (int j = 0; j < primitive->size; j++) {
            mat_copy_vector_d3(std_cell->position[num_atom],
                               std_cell->position[j]);
            for (int k = 0; k < 3; k++) {
                std_cell->position[num_atom][k] += shift[i][k];
            }
            std_cell->types[num_atom] = std_cell->types[j];
            num_atom++;
        }
    }

    Cell *trimmed_cell =
        cel_trim_cell(mapping_table, std_cell->lattice, std_cell, symprec);
    cel_free_cell(std_cell);
    std_cell = NULL;
    free(mapping_table);
    mapping_table = NULL;
    return trimmed_cell;
}

void spa_copy_spacegroup(Spacegroup *dst, const Spacegroup *src) {
    dst->number = src->number;
    dst->hall_number = src->hall_number;
    dst->pointgroup_number = src->pointgroup_number;
    strcpy(dst->schoenflies, src->schoenflies);
    strcpy(dst->hall_symbol, src->hall_symbol);
    strcpy(dst->international, src->international);
    strcpy(dst->international_long, src->international_long);
    strcpy(dst->international_short, src->international_short);
    strcpy(dst->choice, src->choice);
    mat_copy_matrix_d3(dst->bravais_lattice, src->bravais_lattice);
    mat_copy_vector_d3(dst->origin_shift, src->origin_shift);
}

/* Return NULL if failed */
static Spacegroup *search_spacegroup_with_symmetry(
    const Primitive *primitive, const int candidates[],
    const int num_candidates, const Symmetry *symmetry, const double symprec,
    const double angle_tolerance) {
    double conv_lattice[3][3];
    double origin_shift[3];
    PointSymmetry pointsym;

    debug_print("search_spacegroup (tolerance = %f):\n", symprec);

    origin_shift[0] = 0;
    origin_shift[1] = 0;
    origin_shift[2] = 0;

    pointsym = ptg_get_pointsymmetry(symmetry->rot, symmetry->size);
    if (pointsym.size < symmetry->size) {
        warning_print("spglib: Point symmetry of primitive cell is broken. ");
        warning_print("(line %d, %s).\n", __LINE__, __FILE__);
        return NULL;
    }

    int hall_number = iterative_search_hall_number(
        origin_shift, conv_lattice, candidates, num_candidates, primitive,
        symmetry, symprec, angle_tolerance);
    if (hall_number == 0) {
        return NULL;
    }

    Spacegroup *spacegroup =
        get_spacegroup(hall_number, origin_shift, conv_lattice);

    return spacegroup;
}

/* Return spacegroup.number = 0 if failed */
static Spacegroup *get_spacegroup(const int hall_number,
                                  const double origin_shift[3],
                                  const double conv_lattice[3][3]) {
    SpacegroupType spacegroup_type;

    Spacegroup *spacegroup = (Spacegroup *)malloc(sizeof(Spacegroup));
    if (spacegroup == NULL) {
        warning_print("spglib: Memory could not be allocated.");
        return NULL;
    }

    if (-117 < hall_number && hall_number < 531 && hall_number != 0) {
        spacegroup_type = spgdb_get_spacegroup_type(hall_number);
        mat_copy_matrix_d3(spacegroup->bravais_lattice, conv_lattice);
        mat_copy_vector_d3(spacegroup->origin_shift, origin_shift);
        spacegroup->number = spacegroup_type.number;
        spacegroup->hall_number = hall_number;
        spacegroup->pointgroup_number = spacegroup_type.pointgroup_number;
        memcpy(spacegroup->schoenflies, spacegroup_type.schoenflies, 7);
        memcpy(spacegroup->hall_symbol, spacegroup_type.hall_symbol, 17);
        memcpy(spacegroup->international, spacegroup_type.international, 32);
        memcpy(spacegroup->international_long,
               spacegroup_type.international_full, 20);
        memcpy(spacegroup->international_short,
               spacegroup_type.international_short, 11);
        memcpy(spacegroup->choice, spacegroup_type.choice, 6);
    }

    return spacegroup;
}

/* Return 0 if failed */
static int iterative_search_hall_number(
    double origin_shift[3], double conv_lattice[3][3], const int candidates[],
    const int num_candidates, const Primitive *primitive,
    const Symmetry *symmetry, const double symprec,
    const double angle_tolerance) {
    debug_print("iterative_search_hall_number:\n");
    int hall_number =
        search_hall_number(origin_shift, conv_lattice, candidates,
                           num_candidates, primitive, symmetry, symprec);

    if (hall_number != 0) {
        return hall_number;
    }

    double tolerance = symprec;
    for (int attempt = 0; attempt < NUM_ATTEMPT; attempt++) {
        warning_print("spglib: Attempt %d tolerance = %e failed", attempt,
                      tolerance);
        warning_print("(line %d, %s).\n", __LINE__, __FILE__);

        tolerance *= REDUCE_RATE;
        Symmetry *sym_reduced = sym_reduce_operation(
            primitive->cell, symmetry, tolerance, angle_tolerance);
        hall_number =
            search_hall_number(origin_shift, conv_lattice, candidates,
                               num_candidates, primitive, sym_reduced, symprec);
        sym_free_symmetry(sym_reduced);
        sym_reduced = NULL;
        if (hall_number != 0) {
            return hall_number;
        }
    }
    return 0;
}

/* Return 0 if failed */
static int search_hall_number(double origin_shift[3], double conv_lattice[3][3],
                              const int candidates[], const int num_candidates,
                              const Primitive *primitive,
                              const Symmetry *symmetry, const double symprec) {
    Pointgroup pointgroup;
    int tmat_int[3][3];
    double correction_mat[3][3], tmat[3][3], conv_lattice_tmp[3][3];

    debug_print("search_hall_number:\n");

    int aperiodic_axis = primitive->cell->aperiodic_axis;

    /* primitive->cell->lattice is set right handed. */
    /* tmat_int is set right handed. */

    /* For rhombohedral system, basis vectors for hP */
    /* (hexagonal lattice basis) are provided by tmat_int, */
    /* but may be either obverse or reverse setting. */
    pointgroup = ptg_get_transformation_matrix(tmat_int, symmetry->rot,
                                               symmetry->size, aperiodic_axis);

    debug_print("[line %d, %s]\n", __LINE__, __FILE__);
    debug_print("initial transformation matrix\n");
    debug_print_matrix_i3(tmat_int);

    if (pointgroup.number == 0) {
        return 0;
    }

    /* For LAUE1 and LAUE2M, update tmat_int by making smallest lattice */
    if (pointgroup.laue == LAUE1 || pointgroup.laue == LAUE2M) {
        mat_multiply_matrix_di3(conv_lattice_tmp, primitive->cell->lattice,
                                tmat_int);

        if (pointgroup.laue == LAUE1) {
            if (!change_basis_tricli(tmat_int, conv_lattice_tmp,
                                     primitive->cell->lattice, symprec,
                                     aperiodic_axis)) {
                return 0;
            }
        }

        /* The angle between non-unique axes may be acute or non-acute. */
        if (pointgroup.laue == LAUE2M) {
            if (!change_basis_monocli(tmat_int, conv_lattice_tmp,
                                      primitive->cell->lattice, symprec,
                                      aperiodic_axis)) {
                return 0;
            }
        }
    }

    /* For rhombohedral system, correction matrix to hR */
    /* (a=b=c primitive lattice basis) is returned judging */
    /* either obverse or reverse setting. centering=R_CENTER. */
    Centering centering =
        get_centering(correction_mat, tmat_int, pointgroup.laue);
    if (centering == CENTERING_ERROR) {
        return 0;
    }

    mat_multiply_matrix_id3(tmat, tmat_int, correction_mat);
    mat_multiply_matrix_d3(conv_lattice, primitive->cell->lattice, tmat);

    debug_print("[line %d, %s]\n", __LINE__, __FILE__);
    debug_print("transformation matrix\n");
    debug_print_matrix_d3(tmat);

    /* For rhombohedral system, symmetry for a=b=c primitive lattice */
    /* basis is returned although centering == R_CENTER. */
    Symmetry *conv_symmetry =
        get_initial_conventional_symmetry(centering, tmat, symmetry);
    if (conv_symmetry == NULL) {
        return 0;
    }

    for (int i = 0; i < num_candidates; i++) {
        /* Check if hall_number is that of rhombohedral. */
        if (match_hall_symbol_db(
                origin_shift, conv_lattice, /* <-- modified only matched */
                primitive->orig_lattice, candidates[i], pointgroup.number,
                pointgroup.holohedry, centering, conv_symmetry, symprec)) {
            debug_print("[line %d, %s]\n", __LINE__, __FILE__);
            debug_print("origin shift\n");
            debug_print_vector_d3(origin_shift);

            sym_free_symmetry(conv_symmetry);
            conv_symmetry = NULL;
            return candidates[i];
        }
    }

    // Failed to find hall_number
    sym_free_symmetry(conv_symmetry);
    conv_symmetry = NULL;
    return 0;
}

// Perform Niggli cell reduction for Triclinic. Return 0 if failed.
static int change_basis_tricli(int tmat_int[3][3],
                               const double conv_lattice[3][3],
                               const double primitive_lattice[3][3],
                               const double symprec, const int aperiodic_axis) {
    double niggli_cell[9];
    double smallest_lattice[3][3], inv_lattice[3][3], tmat[3][3];

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            niggli_cell[i * 3 + j] = conv_lattice[i][j];
        }
    }

    if (!niggli_reduce(niggli_cell, symprec * symprec, aperiodic_axis)) {
        return 0;
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            smallest_lattice[i][j] = niggli_cell[i * 3 + j];
        }
    }
    if (mat_get_determinant_d3(smallest_lattice) < 0) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                smallest_lattice[i][j] = -smallest_lattice[i][j];
            }
        }
    }
    mat_inverse_matrix_d3(inv_lattice, primitive_lattice, 0);
    mat_multiply_matrix_d3(tmat, inv_lattice, smallest_lattice);
    mat_cast_matrix_3d_to_3i(tmat_int, tmat);

    return 1;
}

/* Monoclinic: choose shortest a, c lattice vectors (|a| < |c|) */
/* Return 0 if failed */
static int change_basis_monocli(int tmat_int[3][3],
                                const double conv_lattice[3][3],
                                const double primitive_lattice[3][3],
                                const double symprec,
                                const int aperiodic_axis_prim) {
    double smallest_lattice[3][3], inv_lattice[3][3], tmat[3][3];

    /* Unique axis of 3D monoclinic crystal system is b */
    /* For layer with 2D op/oc lattice, unique axis is a */
    int unique_axis = -1;
    int aperiodic_axis_conv = 0;
    if (aperiodic_axis_prim == -1) {
        aperiodic_axis_conv = -1;
        unique_axis = 1;
    } else {
        for (int i = 0; i < 3; i++) {
            if (tmat_int[aperiodic_axis_prim][i] != 0) {
                // TODO: Should this one be set only once?
                aperiodic_axis_conv = i;
            }
        }
        unique_axis = 0;
    }
    assert(unique_axis != -1);

    if (!del_layer_delaunay_reduce_2D(smallest_lattice, conv_lattice,
                                      unique_axis, aperiodic_axis_conv,
                                      symprec)) {
        return 0;
    }

    /* For layer with 2D mp lattice, move the unique axis to c */
    if (aperiodic_axis_conv == 0) {
        mat_multiply_matrix_d3(smallest_lattice, smallest_lattice,
                               change_of_basis_monocli[2]);
    }

    mat_inverse_matrix_d3(inv_lattice, primitive_lattice, 0);
    mat_multiply_matrix_d3(tmat, inv_lattice, smallest_lattice);
    mat_cast_matrix_3d_to_3i(tmat_int, tmat);
    return 1;
}

/* Return NULL if failed */
static Symmetry *get_initial_conventional_symmetry(const Centering centering,
                                                   const double tmat[3][3],
                                                   const Symmetry *symmetry) {
    debug_print("get_initial_conventional_symmetry\n");

    /* hP for rhombohedral */
    if (centering == R_CENTER)
        return get_conventional_symmetry(tmat, PRIMITIVE, symmetry);

    return get_conventional_symmetry(tmat, centering, symmetry);
}

/* Return 0 if failed */
static int match_hall_symbol_db(
    double origin_shift[3], double conv_lattice[3][3],
    const double (*orig_lattice)[3], const int hall_number,
    const int pointgroup_number, const Holohedry holohedry,
    const Centering centering, const Symmetry *symmetry, const double symprec) {
    double changed_lattice[3][3], inv_lattice[3][3], tmat[3][3];

    SpacegroupType spacegroup_type = spgdb_get_spacegroup_type(hall_number);

    if (pointgroup_number != spacegroup_type.pointgroup_number) {
        return 0;
    }

    switch (holohedry) {
        case MONOCLI:
            return match_hall_symbol_db_monocli(
                origin_shift, conv_lattice, orig_lattice, hall_number,
                spacegroup_type.number, centering, symmetry, symprec);

        case ORTHO:
            assert(hall_number != 0);
            int num_free_axes =
                (hall_number > 0)
                    ? num_axis_choices_ortho[spacegroup_type.number - 16]
                    : layer_num_axis_choices_ortho[spacegroup_type.number - 19];
            assert(num_free_axes > 0);

            /* Switching two axes */
            /* Two steps: */
            /*   1. Finding principal axis for the representative hall symbol */
            /*      of the specified hall symbol without checking basis vector
             */
            /*      lengths preference. */
            /*   2. Finding transformation matrix and origin shift for the */
            /*      specified hall symbol. */
            if (hall_number > 0 && num_free_axes == 2) {
                mat_copy_matrix_d3(changed_lattice, conv_lattice);
                if (!match_hall_symbol_db_ortho(
                        origin_shift, changed_lattice, orig_lattice,
                        spacegroup_to_hall_number[spacegroup_type.number - 1],
                        centering, symmetry, 0, symprec)) {
                    return 0;
                }
                mat_inverse_matrix_d3(inv_lattice, conv_lattice, 0);
                mat_multiply_matrix_d3(tmat, inv_lattice, changed_lattice);

                Symmetry *changed_symmetry =
                    get_conventional_symmetry(tmat, PRIMITIVE, symmetry);
                if (changed_symmetry == NULL) {
                    return 0;
                }

                int is_found = match_hall_symbol_db_ortho(
                    origin_shift, changed_lattice, orig_lattice, hall_number,
                    centering, changed_symmetry, 2, symprec);
                sym_free_symmetry(changed_symmetry);
                changed_symmetry = NULL;
                if (is_found) {
                    mat_copy_matrix_d3(conv_lattice, changed_lattice);
                    return 1;
                }
                return 0;
            }

            return match_hall_symbol_db_ortho(
                origin_shift, conv_lattice, orig_lattice, hall_number,
                centering, symmetry, num_free_axes, symprec);

        case CUBIC:
            return match_hall_symbol_db_cubic(origin_shift, conv_lattice,
                                              orig_lattice, hall_number,
                                              centering, symmetry, symprec);

        case TRIGO:
            if ((centering == R_CENTER) &&
                (hall_number == 433 || hall_number == 434 ||
                 hall_number == 436 || hall_number == 437 ||
                 hall_number == 444 || hall_number == 445 ||
                 hall_number == 450 || hall_number == 451 ||
                 hall_number == 452 || hall_number == 453 ||
                 hall_number == 458 || hall_number == 459 ||
                 hall_number == 460 || hall_number == 461)) {
                /* Rhombohedral. symmetry is for a=b=c basis. */
                return match_hall_symbol_db_rhombo(origin_shift, conv_lattice,
                                                   orig_lattice, hall_number,
                                                   symmetry, symprec);
            }
            /* Do not break for other trigonal cases */
        default: /* HEXA, TETRA, TRICLI and rest of TRIGO */
            return match_hall_symbol_db_others(
                origin_shift, conv_lattice, orig_lattice, hall_number,
                centering, holohedry, symmetry, symprec);
    }
}

/* Return 0 if failed */
static int match_hall_symbol_db_monocli(
    double origin_shift[3], double conv_lattice[3][3],
    const double (*orig_lattice)[3], const int hall_number,
    const int group_number, const Centering centering,
    const Symmetry *conv_symmetry, const double symprec) {
    int is_found[36];
    double norms_squared[36][2];
    double all_conv_lattices[36][3][3], all_origin_shifts[36][3];

    /* E. Parthe and L. M. Gelato */
    /* "The best unit cell for monoclinic structure..." (1983) */
    // Space group: 3: P2, 4: P2_1, 6: Pm, 10: P2/m, 11: P2_1/m
    // Layer group: 3: p112, 4: p11m, 6: p112/m
    int check_norms = 0;
    if ((hall_number > 0 &&
         (group_number == 3 || group_number == 4 || group_number == 6 ||
          group_number == 10 || group_number == 11)) ||
        (hall_number < 0 &&
         (group_number == 3 || group_number == 4 || group_number == 6))) {
        /* |a| < |c| for unique axis b. (This is as written in the paper 1983.)
         */
        /* |a| < |b| for unique axis c. (This is spgilb definition.) */
        /* |b| < |c| for unique axis a. (This is spgilb definition.) */
        check_norms = 1;
    }

    /* For layer group, 3-11: Monoclinic/Oblique, 12-33: Monoclinic/Rectangular
     */
    int num_change_of_basis = 0;
    if (hall_number > 0) {
        num_change_of_basis = 36;
    } else if (-11 <= hall_number && hall_number <= -3) {
        num_change_of_basis = 12;
    } else if (-33 <= hall_number && hall_number <= -12) {
        num_change_of_basis = 8;
    }
    assert(num_change_of_basis > 0);

    for (int i = 0; i < num_change_of_basis; i++) {
        mat_copy_matrix_d3(all_conv_lattices[i], conv_lattice);

        int j = 0;
        if (hall_number > 0) {
            j = i;
        } else if (-11 <= hall_number && hall_number <= -3) {
            // Monoclinic/oblique
            // Try change_of_basis_monocli[36:48]
            j = i + 36;
        } else if (-33 <= hall_number && hall_number <= -12) {
            // Monoclinic/rectangular
            j = index_monocli_rectang[i];
        }
        assert(j > 0);

        /* all_origin_shifts[i] is possibly overwritten. */
        /* is_found == 0: Not found */
        /* is_found == 1: Found. conv_lattice may not be similar to orig_lattice
         */
        /* is_found == 2: Found. conv_lattice is similar to orig_lattice */
        is_found[i] = match_hall_symbol_db_monocli_in_loop(
            all_origin_shifts[i], all_conv_lattices[i], norms_squared[i], j,
            orig_lattice, check_norms, hall_number, centering, conv_symmetry,
            symprec);
    }

    int is_found_any = 0;
    double shortest_norm_sum = 0.0;
    for (int i = 0; i < num_change_of_basis; i++) {
        if (is_found[i]) {
            // TODO: Error: i_shortest is never used before initialization below
            //            i_shortest = i;
            shortest_norm_sum =
                sqrt(norms_squared[i][0]) + sqrt(norms_squared[i][1]);
            is_found_any = 1;
            break;
        }
    }

    assert(shortest_norm_sum > 0.0);
    if (!is_found_any) {
        return 0;
    }

    /* Find shortest non-unique axes lengths */
    for (int i = 0; i < num_change_of_basis; i++) {
        if (is_found[i]) {
            double norm_sum =
                sqrt(norms_squared[i][0]) + sqrt(norms_squared[i][1]);
            if (shortest_norm_sum > norm_sum) {
                shortest_norm_sum = norm_sum;
            }
        }
    }

    /* Prefers is_found == 2, i.e., similar to orig_lattice */
    int i_shortest = -1;
    for (int i = 0; i < num_change_of_basis; i++) {
        if (is_found[i]) {
            double norm_sum =
                sqrt(norms_squared[i][0]) + sqrt(norms_squared[i][1]);
            if (mat_Dabs(norm_sum - shortest_norm_sum) < symprec) {
                if (is_found[i] == 2) {
                    i_shortest = i;
                    break;
                }
                if (i_shortest < 0) {
                    i_shortest = i;
                }
            }
        }
    }

    mat_copy_vector_d3(origin_shift, all_origin_shifts[i_shortest]);
    mat_copy_matrix_d3(conv_lattice, all_conv_lattices[i_shortest]);
    return 1;
}

static int match_hall_symbol_db_monocli_in_loop(
    double origin_shift[3], double conv_lattice[3][3], double norms_squared[2],
    const int change_of_basis_index, const double (*orig_lattice)[3],
    const int check_norms, const int hall_number, const Centering centering,
    const Symmetry *conv_symmetry, const double symprec) {
    double vec[2][3];
    double tmat[3][3], change_of_basis[3][3];

    /* centring type should be P or C */
    Centering changed_centering =
        (centering == C_FACE)
            ? change_of_centering_monocli[change_of_basis_index]
            : centering;

    mat_copy_matrix_d3(change_of_basis,
                       change_of_basis_monocli[change_of_basis_index]);
    mat_multiply_matrix_d3(conv_lattice, conv_lattice, change_of_basis);
    int unique_axis = change_of_unique_axis_monocli[change_of_basis_index];

    /* Make non-acute and length preference */
    /* norms_squared[0] and norms_squared[1] are the norms of the two */
    /* non unique axes among a,b,c. */
    for (int j = 0, l = 0; j < 3; j++) {
        if (j == unique_axis) {
            continue;
        }
        for (int k = 0; k < 3; k++) {
            vec[l][k] = conv_lattice[k][j];
        }
        norms_squared[l] = mat_norm_squared_d3(vec[l]);
        l++;
    }

    /* discard if principal angle is acute. */
    if (vec[0][0] * vec[1][0] + vec[0][1] * vec[1][1] + vec[0][2] * vec[1][2] >
        ZERO_PREC) {
        return 0;
    }

    /* Choose |a| < |b| < |c| among two non-principles axes */
    /* if there are freedom. */
    if (check_norms) {
        if (norms_squared[0] > norms_squared[1] + ZERO_PREC) {
            return 0;
        }
    }

    /* When orig_lattice is given not NULL, try to find similar (a, b, c) */
    /* choices to the input (a, b, c) by flipping a, b, c axes. */
    /* Here flipping means a -> -a, and so on. */
    /* Note that flipped (a,b,c) that match those of input should not */
    /* change centring for monoclinic case. */
    int retval = 1;
    if (orig_lattice != NULL) {
        if (mat_get_determinant_d3(orig_lattice) > symprec) {
            /* This (mode=1) effectively checks C2 rotation along unique axis.
             */
            if (is_equivalent_lattice(tmat, 1, conv_lattice, orig_lattice,
                                      symprec)) {
                if (tmat[(unique_axis + 1) % 3][(unique_axis + 1) % 3] *
                        tmat[(unique_axis + 2) % 3][(unique_axis + 2) % 3] >
                    ZERO_PREC) {
                    mat_multiply_matrix_d3(conv_lattice, conv_lattice, tmat);
                    mat_multiply_matrix_d3(change_of_basis, change_of_basis,
                                           tmat);
                    retval = 2;
                }
            }
        }
    }

    Symmetry *changed_symmetry =
        get_conventional_symmetry(change_of_basis, PRIMITIVE, conv_symmetry);
    if (changed_symmetry == NULL) {
        return 0;
    }
    int is_found =
        hal_match_hall_symbol_db(origin_shift, conv_lattice, hall_number,
                                 changed_centering, changed_symmetry, symprec);
    sym_free_symmetry(changed_symmetry);
    changed_symmetry = NULL;

    return is_found ? retval : 0;
}

/* Return 0 if failed */
static int match_hall_symbol_db_ortho(
    double origin_shift[3], double conv_lattice[3][3],
    const double (*orig_lattice)[3], const int hall_number,
    const Centering centering, const Symmetry *conv_symmetry,
    const int num_free_axes, const double symprec) {
    // For layer, try axis choices abc and ba-c, which preserve the aperiodic
    // axis.
    int j = (hall_number > 0) ? 1 : 3;

    /* Try to find the best (a, b, c) by flipping axes. */
    if (orig_lattice != NULL) {
        if (mat_get_determinant_d3(orig_lattice) > symprec) {
            for (int i = 0; i < 6; i += j) {
                if (match_hall_symbol_db_ortho_in_loop(
                        origin_shift, conv_lattice, orig_lattice, i,
                        hall_number, centering, conv_symmetry, num_free_axes,
                        symprec)) {
                    return 1;
                }
            }
        }
    }

    /* If flipping didn't work, usual search is made. */
    for (int i = 0; i < 6; i += j) {
        if (match_hall_symbol_db_ortho_in_loop(
                origin_shift, conv_lattice, NULL, i, hall_number, centering,
                conv_symmetry, num_free_axes, symprec)) {
            return 1;
        }
    }

    return 0;
}

// @param[out] origin_shift
// @param[out] conv_lattice
// @param[in] orig_lattice
// @param[in] axis_choice_index Index of axis choices, {abc, bca, cab, ba-c,
// a-cb, -cba}
// @param[in] hall_number
// @param[in] centering
// @param[in] symmetry
// @param[in] num_free_axes
// @param[in] symprec
static int match_hall_symbol_db_ortho_in_loop(
    double origin_shift[3], double conv_lattice[3][3],
    const double (*orig_lattice)[3], const int axis_choice_index,
    const int hall_number, const Centering centering, const Symmetry *symmetry,
    const int num_free_axes, const double symprec) {
    double vec[3], norms[3];
    double changed_lattice[3][3], tmat[3][3], change_of_basis[3][3];

    Centering changed_centering =
        (centering == C_FACE) ? change_of_centering_ortho[axis_choice_index]
                              : centering;

    mat_copy_matrix_d3(change_of_basis,
                       change_of_basis_ortho[axis_choice_index]);
    mat_multiply_matrix_d3(changed_lattice, conv_lattice, change_of_basis);

    /* When orig_lattice is given not NULL, try to find similar (a, b, c) */
    /* choices to the input (a, b, c) by flipping a, b, c axes. */
    /* Here flipping means a -> -a, and so on. */
    /* Note that flipping of axes doesn't change centering. */
    if (orig_lattice != NULL) {
        if (is_equivalent_lattice(tmat, 1, changed_lattice, orig_lattice,
                                  symprec)) {
            mat_multiply_matrix_d3(changed_lattice, changed_lattice, tmat);
            mat_multiply_matrix_d3(change_of_basis, change_of_basis, tmat);
        } else
            /* This is necessary to run through all */
            /* change_of_basis_ortho. */
            return 0;
    }

    // When num_free_axes=2, axis choices {abc, ba-c}, {cab, -cba}, and {bca,
    // a-cb} give the identical space groups, respectively.
    if (num_free_axes == 2) {
        for (int j = 0, l = 0; j < 3; j++) {
            if (j == change_of_unique_axis_ortho[axis_choice_index]) {
                continue;
            }
            for (int k = 0; k < 3; k++) {
                vec[k] = changed_lattice[k][j];
            }
            norms[l] = mat_norm_squared_d3(vec);
            l++;
        }
        if (norms[0] > norms[1] + ZERO_PREC) {
            return 0;
        }
    }

    // When num_free_axes=3, axis choices {abc, bca, cab} and {ba-c, a-cb, -cba}
    // give the identical space groups, respectively.
    if (num_free_axes == 3) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                vec[k] = changed_lattice[k][j];
            }
            norms[j] = mat_norm_squared_d3(vec);
        }
        if ((norms[0] > norms[1] + ZERO_PREC) ||
            (norms[0] > norms[2] + ZERO_PREC)) {
            return 0;
        }
    }

    // When num_free_axes=6, each axis choice gives a different space group.
    if (num_free_axes == 6) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                vec[k] = changed_lattice[k][j];
            }
            norms[j] = mat_norm_squared_d3(vec);
        }
        if ((norms[0] > norms[1] + ZERO_PREC) ||
            (norms[1] > norms[2] + ZERO_PREC)) {
            return 0;
        }
    }

    Symmetry *changed_symmetry =
        get_conventional_symmetry(change_of_basis, PRIMITIVE, symmetry);
    if (changed_symmetry == NULL) {
        return 0;
    }

    int is_found =
        hal_match_hall_symbol_db(origin_shift, changed_lattice, hall_number,
                                 changed_centering, changed_symmetry, symprec);
    sym_free_symmetry(changed_symmetry);
    changed_symmetry = NULL;
    if (is_found) {
        mat_copy_matrix_d3(conv_lattice, changed_lattice);
        return 1;
    }
    return 0;
}

static int match_hall_symbol_db_cubic(double origin_shift[3],
                                      double conv_lattice[3][3],
                                      const double (*orig_lattice)[3],
                                      const int hall_number,
                                      const Centering centering,
                                      const Symmetry *conv_symmetry,
                                      const double symprec) {
    /* Special treatment for No. 205 (501) is included in this change of */
    /* basis. To see old code, commit hash 67997654 and change_of_basis_501. */
    if (orig_lattice != NULL) {
        if (mat_get_determinant_d3(orig_lattice) > symprec) {
            for (int i = 0; i < 6; i++) {
                if (match_hall_symbol_db_cubic_in_loop(
                        origin_shift, conv_lattice, orig_lattice, i,
                        hall_number, centering, conv_symmetry, symprec)) {
                    return 1;
                }
            }
        }
    }

    for (int i = 0; i < 6; i++) {
        if (match_hall_symbol_db_cubic_in_loop(origin_shift, conv_lattice, NULL,
                                               i, hall_number, centering,
                                               conv_symmetry, symprec)) {
            return 1;
        }
    }
    return 0;
}

static int match_hall_symbol_db_cubic_in_loop(
    double origin_shift[3], double conv_lattice[3][3],
    const double (*orig_lattice)[3], const int i, const int hall_number,
    const Centering centering, const Symmetry *conv_symmetry,
    const double symprec) {
    double changed_lattice[3][3], tmat[3][3], change_of_basis[3][3];

    mat_copy_matrix_d3(change_of_basis, change_of_basis_ortho[i]);
    mat_multiply_matrix_d3(changed_lattice, conv_lattice, change_of_basis);

    if (orig_lattice != NULL) {
        if (is_equivalent_lattice(tmat, 1, changed_lattice, orig_lattice,
                                  symprec)) {
            mat_multiply_matrix_d3(changed_lattice, changed_lattice, tmat);
            mat_multiply_matrix_d3(change_of_basis, change_of_basis, tmat);
        } else {
            return 0; /* This is necessary to run through all */
                      /* change_of_basis_ortho. */
        }
    }

    Symmetry *changed_symmetry =
        get_conventional_symmetry(change_of_basis, PRIMITIVE, conv_symmetry);
    if (changed_symmetry == NULL) {
        return 0;
    }

    int is_found =
        hal_match_hall_symbol_db(origin_shift, changed_lattice, hall_number,
                                 centering, changed_symmetry, symprec);

    sym_free_symmetry(changed_symmetry);
    changed_symmetry = NULL;

    if (is_found) {
        mat_copy_matrix_d3(conv_lattice, changed_lattice);
        return 1;
    }
    return 0;
}

static int match_hall_symbol_db_rhombo(double origin_shift[3],
                                       double conv_lattice[3][3],
                                       const double (*orig_lattice)[3],
                                       const int hall_number,
                                       const Symmetry *conv_symmetry,
                                       const double symprec) {
    if (hall_number == 433 || hall_number == 436 || hall_number == 444 ||
        hall_number == 450 || hall_number == 452 || hall_number == 458 ||
        hall_number == 460) { /* Hexagonal lattice */
        int is_found = match_hall_symbol_db_change_of_basis_loop(
            origin_shift, conv_lattice, orig_lattice,
            change_of_basis_rhombo_hex, 6, hall_number, R_CENTER, conv_symmetry,
            symprec);
        if (is_found) {
            // TODO: Debug code?
            /* mat_copy_matrix_d3(conv_lattice, changed_lattice); */
            return 1;
        }
        return 0;

    } /* Rhombohedral (a=b=c) lattice */
    return match_hall_symbol_db_change_of_basis_loop(
        origin_shift, conv_lattice, orig_lattice, change_of_basis_rhombo, 6,
        hall_number, PRIMITIVE, conv_symmetry, symprec);

    return 0;
}

/* HEXA, TETRA, TRICLI and TRIGO but not Rhombohedral a=b=c */
static int match_hall_symbol_db_others(
    double origin_shift[3], double conv_lattice[3][3],
    const double (*orig_lattice)[3], const int hall_number,
    const Centering centering, const Holohedry holohedry,
    const Symmetry *conv_symmetry, const double symprec) {
    /* TRICLI: No check. */
    if (holohedry == TRICLI) {
        return hal_match_hall_symbol_db(origin_shift, conv_lattice, hall_number,
                                        centering, conv_symmetry, symprec);
    }

    /* TETRA: Check over 4 fold rotations with/without flipping c-axis */
    if (holohedry == TETRA) {
        return match_hall_symbol_db_change_of_basis_loop(
            origin_shift, conv_lattice, orig_lattice, change_of_basis_C4, 8,
            hall_number, centering, conv_symmetry, symprec);
    }

    /* HEXA and TRIGO but not Rhombohedral: */
    /*    Check over 6 fold rotations with/without flipping c-axis */
    return match_hall_symbol_db_change_of_basis_loop(
        origin_shift, conv_lattice, orig_lattice, change_of_basis_C6, 12,
        hall_number, centering, conv_symmetry, symprec);
}

static int match_hall_symbol_db_change_of_basis_loop(
    double origin_shift[3], double conv_lattice[3][3],
    const double (*orig_lattice)[3], const double (*change_of_basis)[3][3],
    const int num_change_of_basis, const int hall_number,
    const Centering centering, const Symmetry *conv_symmetry,
    const double symprec) {
    double changed_lattice[3][3], tmat[3][3];

    Centering centering_for_symmetry =
        (centering == R_CENTER) ? R_CENTER : PRIMITIVE;

    /* Check of similarity of basis vectors to those of input */
    if (orig_lattice != NULL) {
        if (mat_get_determinant_d3(orig_lattice) > symprec) {
            for (int i = 0; i < num_change_of_basis; i++) {
                Symmetry *changed_symmetry = get_conventional_symmetry(
                    change_of_basis[i], centering_for_symmetry, conv_symmetry);
                if (changed_symmetry == NULL) {
                    continue;
                }
                mat_multiply_matrix_d3(changed_lattice, conv_lattice,
                                       change_of_basis[i]);
                int is_found = 0;
                if (is_equivalent_lattice(tmat, 0, changed_lattice,
                                          orig_lattice, symprec)) {
                    /* Here R_CENTER means hP (hexagonal) setting. */
                    is_found = hal_match_hall_symbol_db(
                        origin_shift, changed_lattice, hall_number, centering,
                        changed_symmetry, symprec);
                }
                sym_free_symmetry(changed_symmetry);
                changed_symmetry = NULL;
                if (is_found) {
                    mat_copy_matrix_d3(conv_lattice, changed_lattice);
                    return 1;
                }
            }
        }
    }

    /* No check of similarity of basis vectors to those of input */
    for (int i = 0; i < num_change_of_basis; i++) {
        Symmetry *changed_symmetry = get_conventional_symmetry(
            change_of_basis[i], centering_for_symmetry, conv_symmetry);
        if (changed_symmetry == NULL) {
            continue;
        }
        mat_multiply_matrix_d3(changed_lattice, conv_lattice,
                               change_of_basis[i]);
        int is_found =
            hal_match_hall_symbol_db(origin_shift, changed_lattice, hall_number,
                                     centering, changed_symmetry, symprec);
        sym_free_symmetry(changed_symmetry);
        changed_symmetry = NULL;
        if (is_found) {
            mat_copy_matrix_d3(conv_lattice, changed_lattice);
            return 1;
        }
    }

    return 0;
}

/* Return NULL if failed */
static Symmetry *get_conventional_symmetry(const double tmat[3][3],
                                           const Centering centering,
                                           const Symmetry *primitive_sym) {
    double inv_tmat[3][3], shift[3][3];
    double symmetry_rot_d3[3][3], primitive_sym_rot_d3[3][3];

    int size = primitive_sym->size;

    Symmetry *symmetry = NULL;
    switch (centering) {
        case FACE:
            symmetry = sym_alloc_symmetry(size * 4);
            break;
        case R_CENTER:
            symmetry = sym_alloc_symmetry(size * 3);
            break;
        case BODY:
        case A_FACE:
        case B_FACE:
        case C_FACE:
            symmetry = sym_alloc_symmetry(size * 2);
            break;
        default:
            symmetry = sym_alloc_symmetry(size);
    }
    if (symmetry == NULL) return NULL;

    for (int i = 0; i < size; i++) {
        mat_cast_matrix_3i_to_3d(primitive_sym_rot_d3, primitive_sym->rot[i]);

        /* C*S*C^-1: recover conventional cell symmetry operation */
        mat_get_similar_matrix_d3(symmetry_rot_d3, primitive_sym_rot_d3, tmat,
                                  0);
        mat_cast_matrix_3d_to_3i(symmetry->rot[i], symmetry_rot_d3);

        /* translation in conventional cell: C = B^-1*P */
        mat_inverse_matrix_d3(inv_tmat, tmat, 0);
        mat_multiply_matrix_vector_d3(symmetry->trans[i], inv_tmat,
                                      primitive_sym->trans[i]);
    }

    if (centering != PRIMITIVE) {
        int multi = get_centering_shifts(shift, centering);
        for (int i = 0; i < multi - 1; i++) {
            for (int j = 0; j < size; j++) {
                mat_copy_matrix_i3(symmetry->rot[(i + 1) * size + j],
                                   symmetry->rot[j]);
                for (int k = 0; k < 3; k++) {
                    symmetry->trans[(i + 1) * size + j][k] =
                        symmetry->trans[j][k] + shift[i][k];
                }
            }
        }
    }

    /* This is done in get_origin_shift
    for (int i = 0; i < multi; i++) {
      for (int j = 0; j < size; j++) {
        for (int k = 0; k < 3; k++) {
          symmetry->trans[i * size + j][k] =
            mat_Dmod1(symmetry->trans[i * size + j][k]);
        }
      }
    }
    */

    return symmetry;
}

/* Return CENTERING_ERROR if failed */
static Centering get_centering(double correction_mat[3][3],
                               const int tmat[3][3], const Laue laue) {
    double trans_corr_mat[3][3];

    mat_copy_matrix_d3(correction_mat, identity);
    int det = abs(mat_get_determinant_i3(tmat));
    debug_print("laue class: %d\n", laue);
    debug_print("multiplicity: %d\n", det);

    Centering centering = CENTERING_ERROR;
    switch (det) {
        case 1:
            return PRIMITIVE;

        case 2:
            centering = get_base_center(tmat);
            if (centering == A_FACE) {
                if (laue == LAUE2M) {
                    debug_print("Monocli A to C\n");
                    mat_copy_matrix_d3(correction_mat, monocli_a2c);
                } else {
                    mat_copy_matrix_d3(correction_mat, a2c);
                }
                centering = C_FACE;
            }
            if (centering == B_FACE) {
                mat_copy_matrix_d3(correction_mat, b2c);
                centering = C_FACE;
            }
            if (laue == LAUE2M && centering == BODY) {
                debug_print("Monocli I to C\n");
                mat_copy_matrix_d3(correction_mat, monocli_i2c);
                centering = C_FACE;
            }
            return centering;

        case 3:
            /* hP (a=b) but not hR (a=b=c) */
            mat_multiply_matrix_id3(trans_corr_mat, tmat, rhombo_obverse);
            if (mat_is_int_matrix(trans_corr_mat, INT_PREC)) {
                mat_copy_matrix_d3(correction_mat, rhombo_obverse);
                debug_print("R-center observe setting\n");
                debug_print_matrix_d3(trans_corr_mat);
            }
            mat_multiply_matrix_id3(trans_corr_mat, tmat, rhomb_reverse);
            if (mat_is_int_matrix(trans_corr_mat, INT_PREC)) {
                mat_copy_matrix_d3(correction_mat, rhomb_reverse);
                debug_print("R-center reverse setting\n");
                debug_print_matrix_d3(trans_corr_mat);
            }
            return R_CENTER;

        case 4:
            return FACE;

        default:
            return CENTERING_ERROR;
    }
}

static Centering get_base_center(const int tmat[3][3]) {
    debug_print("lat_get_base_center\n");

    /* C center */
    for (int i = 0; i < 3; i++) {
        if (tmat[i][0] == 0 && tmat[i][1] == 0 && abs(tmat[i][2]) == 1) {
            return C_FACE;
        }
    }

    /* A center */
    for (int i = 0; i < 3; i++) {
        if (abs(tmat[i][0]) == 1 && tmat[i][1] == 0 && tmat[i][2] == 0) {
            return A_FACE;
        }
    }

    /* B center */
    for (int i = 0; i < 3; i++) {
        if (tmat[i][0] == 0 && abs(tmat[i][1]) == 1 && tmat[i][2] == 0) {
            return B_FACE;
        }
    }

    /* body center */
    if ((abs(tmat[0][0]) + abs(tmat[0][1]) + abs(tmat[0][2]) == 2) &&
        (abs(tmat[1][0]) + abs(tmat[1][1]) + abs(tmat[1][2]) == 2) &&
        (abs(tmat[2][0]) + abs(tmat[2][1]) + abs(tmat[2][2]) == 2)) {
        return BODY;
    }

    /* This should not happen. */
    warning_print("spglib: No centring was found (line %d, %s).\n", __LINE__,
                  __FILE__);
    return PRIMITIVE;
}

static int get_centering_shifts(double shift[3][3], const Centering centering) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            shift[i][j] = 0;
        }
    }

    switch (centering) {
        case PRIMITIVE:
            return 1;
        case FACE:
            shift[0][0] = 0;
            shift[0][1] = 0.5;
            shift[0][2] = 0.5;
            shift[1][0] = 0.5;
            shift[1][1] = 0;
            shift[1][2] = 0.5;
            shift[2][0] = 0.5;
            shift[2][1] = 0.5;
            shift[2][2] = 0;
            return 4;
        case R_CENTER:
            shift[0][0] = 2. / 3;
            shift[0][1] = 1. / 3;
            shift[0][2] = 1. / 3;
            shift[1][0] = 1. / 3;
            shift[1][1] = 2. / 3;
            shift[1][2] = 2. / 3;
            return 3;
        default:
            for (int i = 0; i < 3; i++) {
                shift[0][i] = 0.5;
            } /* BODY */
            if (centering == A_FACE) {
                shift[0][0] = 0;
            }
            if (centering == B_FACE) {
                shift[0][1] = 0;
            }
            if (centering == C_FACE) {
                shift[0][2] = 0;
            }
            return 2;
    }
}

// @brief Return 1 if `lattice` and `orig_lattice` is equivalent.
//        The equivalence is defined though `mode`.
// @param[out] tmat Isometric transformation s.t. orig_lattice = lattice * tmat
// if exists
// @param[in] mode
//            mode=0: Equivalent if `lattice` and `orig_lattice` are identical
//            as matrices. mode=1: Allow to flip axes when abs(P)=I but P!=I.
//            mode=2: Check equivalence from their metric tensors.
// @param[in] lattice
// @param[in] orig_lattice
// @param[in] symprec
static int is_equivalent_lattice(double tmat[3][3], const int mode,
                                 const double lattice[3][3],
                                 const double orig_lattice[3][3],
                                 const double symprec) {
    double inv_lat[3][3], tmat_abs[3][3], metric[3][3], orig_metric[3][3];
    int tmat_int[3][3];

    if (mat_Dabs(mat_get_determinant_d3(lattice) -
                 mat_get_determinant_d3(orig_lattice)) > symprec) {
        return 0;
    }

    if (!mat_inverse_matrix_d3(inv_lat, lattice, symprec)) {
        return 0;
    }

    // orig_lattice == lattice @ tmat
    mat_multiply_matrix_d3(tmat, inv_lat, orig_lattice);

    switch (mode) {
        case 0: /* Check identity of all elements */
            return mat_check_identity_matrix_d3(identity, tmat, symprec);

        case 1: /* Check identity of all elements allowing axes flips */
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    tmat_abs[i][j] = mat_Dabs(tmat[i][j]);
                }
            }

            return mat_check_identity_matrix_d3(identity, tmat_abs, symprec);

        case 2: /* Check metric tensors */
            mat_cast_matrix_3d_to_3i(tmat_int, tmat);
            if (!mat_check_identity_matrix_id3(tmat_int, tmat, symprec)) {
                break;
            }
            if (mat_get_determinant_i3(tmat_int) != 1) {
                break;
            }
            mat_get_metric(orig_metric, orig_lattice);
            mat_get_metric(metric, lattice);
            return mat_check_identity_matrix_d3(orig_metric, metric, symprec);
        default:
            return 0;
    }
}
