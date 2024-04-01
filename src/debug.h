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

#ifndef __debug_H__
#define __debug_H__

#include <stdio.h>

// Macros to either print or do nothing if in debug mode or not
#ifdef SPGDEBUG
    #define debug_print(...) printf(__VA_ARGS__)
    #define debug_print_matrix_d3(a) dbg_print_matrix_d3(a)
    #define debug_print_matrix_i3(a) dbg_print_matrix_i3(a)
    #define debug_print_vector_d3(a) dbg_print_vector_d3(a)
    #define debug_print_vectors_d3(...) dbg_print_vectors_d3(__VA_ARGS__)
    #define debug_print_vectors_with_label(...) \
        dbg_print_vectors_with_label(__VA_ARGS__)

// Just to make sure these are never used, these definitions are #ifdef guarded
void dbg_print_matrix_d3(double const a[3][3]);
void dbg_print_matrix_i3(int const a[3][3]);
void dbg_print_vector_d3(double const a[3]);
void dbg_print_vectors_d3(double const a[][3], int size);
void dbg_print_vectors_with_label(double const a[][3], int const b[], int size);

#else
    #define debug_print(...)
    #define debug_print_matrix_d3(a)
    #define debug_print_matrix_i3(a)
    #define debug_print_vector_d3(a)
    #define debug_print_vectors_d3(...)
    #define debug_print_vectors_with_label(...)
#endif

#ifdef SPGWARNING
    #define warning_print(...) fprintf(stderr, __VA_ARGS__)
#else
    #define warning_print(...)
#endif
#ifdef SPGINFO
    #define info_print(...) fprintf(stderr, __VA_ARGS__)
#else
    #define info_print(...)
#endif
#endif
