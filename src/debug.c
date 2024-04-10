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

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

bool debug_enabled() {
    char const* debug_env = getenv("SPGLIB_DEBUG");
    // If SPGLIB_DEBUG is not defined, do not output any debug info
    if (debug_env == NULL) return false;
    // Here we are not checking if SPGLIB_DEBUG is true/1/etc. we only check if
    // it is defined, including SPGLIB_DEBUG=""
    return true;
}
bool warning_enabled() {
    char const* warning_env = getenv("SPGLIB_WARNING");
    // If SPGLIB_WARNING is not defined assume warning is on
    if (warning_env == NULL) return true;
    // Check if SPGLIB_WARNING is disabled. Not performing case-insensitive
    // checks.
    if (strcmp(warning_env, "OFF") == 0) return false;
    // Otherwise assume it's an ill-defined value and ignore it
    return true;
}
bool info_enabled() {
    char const* info_env = getenv("SPGLIB_INFO");
    // If SPGLIB_INFO is not defined, do not output any info messages
    if (info_env == NULL) return false;
    // Here we are not checking if SPGLIB_INFO is true/1/etc. we only check if
    // it is defined, including SPGLIB_INFO=""
    return true;
}

#ifdef SPGDEBUG
void debug_print_matrix_d3(double const a[3][3]) {
    if (!debug_enabled()) return;
    for (int i = 0; i < 3; i++) {
        fprintf(stdout, "%f %f %f\n", a[i][0], a[i][1], a[i][2]);
    }
}

void debug_print_matrix_i3(int const a[3][3]) {
    if (!debug_enabled()) return;
    for (int i = 0; i < 3; i++) {
        fprintf(stdout, "%d %d %d\n", a[i][0], a[i][1], a[i][2]);
    }
}

void debug_print_vectors_d3(double const a[][3], int size) {
    if (!debug_enabled()) return;
    for (int i = 0; i < size; i++) {
        fprintf(stdout, "%d: %f %f %f\n", i + 1, a[i][0], a[i][1], a[i][2]);
    }
}

void debug_print_vector_d3(double const a[3]) {
    if (!debug_enabled()) return;
    fprintf(stdout, "%f %f %f\n", a[0], a[1], a[2]);
}

void debug_print_vectors_with_label(double const a[][3], int const b[],
                                    int size) {
    if (!debug_enabled()) return;
    for (int i = 0; i < size; i++) {
        fprintf(stdout, "%d: %f %f %f\n", b[i], a[i][0], a[i][1], a[i][2]);
    }
}
void debug_print(char const* format, ...) {
    if (!debug_enabled()) return;
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stdout, format, argptr);
    va_end(argptr);
}
#else
void debug_print_matrix_d3(double const a[3][3]) {}

void debug_print_matrix_i3(int const a[3][3]) {}

void debug_print_vectors_d3(double const a[][3], int size) {}

void debug_print_vector_d3(double const a[3]) {}

void debug_print_vectors_with_label(double const a[][3], int const b[],
                                    int size) {}
void debug_print(char const* format, ...) {}
#endif

#ifdef SPGWARNING
void warning_print(char const* format, ...) {
    if (!warning_enabled()) return;
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
}
#else
void warning_print(char const* format, ...) {}
#endif

#ifdef SPGINFO
void info_print(char const* format, ...) {
    if (!info_enabled()) return;
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
}
#else
void info_print(char const* format, ...) {}
#endif

void warning_memory(char const* what) {
    warning_print("Spglib: Memory could not be allocated: %s\n", what);
}
