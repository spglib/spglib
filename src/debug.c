// Copyright (C) 2008 Atsushi Togo
// This file is part of spglib.
// SPDX-License-Identifier: BSD-3-Clause

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"

bool debug_enabled(void) {
    char const *debug_env = getenv("SPGLIB_DEBUG");
    // If SPGLIB_DEBUG is not defined, do not output any debug info
    if (debug_env == NULL) return false;
    // Here we are not checking if SPGLIB_DEBUG is true/1/etc. we only check if
    // it is defined, including SPGLIB_DEBUG=""
    return true;
}
bool warning_enabled(void) {
    char const *warning_env = getenv("SPGLIB_WARNING");
    // If SPGLIB_WARNING is not defined assume warning is on
    if (warning_env == NULL) return true;
    // Check if SPGLIB_WARNING is disabled. Not performing case-insensitive
    // checks.
    if (strcmp(warning_env, "OFF") == 0) return false;
    // Otherwise assume it's an ill-defined value and ignore it
    return true;
}
bool info_enabled(void) {
    char const *info_env = getenv("SPGLIB_INFO");
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
void debug_print(char const *format, ...) {
    if (!debug_enabled()) return;
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stdout, format, argptr);
    va_end(argptr);
}
#endif

#ifdef SPGWARNING
void warning_print(char const *format, ...) {
    if (!warning_enabled()) return;
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
}
void warning_memory(char const *what) {
    warning_print("Spglib: Memory could not be allocated: %s\n", what);
}
#endif

#ifdef SPGINFO
void info_print(char const *format, ...) {
    if (!info_enabled()) return;
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
}
#endif
