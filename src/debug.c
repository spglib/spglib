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

#include <stdio.h>

#include "debug.h"

void dbg_print_matrix_d3(double const a[3][3]) {
    int i;
    for (i = 0; i < 3; i++) {
        printf("%f %f %f\n", a[i][0], a[i][1], a[i][2]);
    }
}

void dbg_print_matrix_i3(int const a[3][3]) {
    int i;
    for (i = 0; i < 3; i++) {
        printf("%d %d %d\n", a[i][0], a[i][1], a[i][2]);
    }
}

void dbg_print_vectors_d3(double const a[][3], int size) {
    int i;
    for (i = 0; i < size; i++) {
        printf("%d: %f %f %f\n", i + 1, a[i][0], a[i][1], a[i][2]);
    }
}

void dbg_print_vector_d3(double const a[3]) {
    printf("%f %f %f\n", a[0], a[1], a[2]);
}

void dbg_print_vectors_with_label(double const a[][3], int const b[],
                                  int size) {
    int i;
    for (i = 0; i < size; i++) {
        printf("%d: %f %f %f\n", b[i], a[i][0], a[i][1], a[i][2]);
    }
}
