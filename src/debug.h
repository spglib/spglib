// Copyright (C) 2008 Atsushi Togo
// This file is part of spglib.
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

// If printing is disabled define empty inlined functions to be optimized out
#ifndef SPGDEBUG
static inline void debug_print_matrix_d3(double const a[3][3]) {}

static inline void debug_print_matrix_i3(int const a[3][3]) {}

static inline void debug_print_vectors_d3(double const a[][3], int size) {}

static inline void debug_print_vector_d3(double const a[3]) {}

static inline void debug_print_vectors_with_label(double const a[][3],
                                                  int const b[], int size) {}
static inline void debug_print(char const *format, ...) {}
#endif
#ifndef SPGWARNING
static inline void warning_print(char const *format, ...) {}
static inline void warning_memory(char const *what) {}
#endif
#ifndef SPGINFO
static inline void info_print(char const *format, ...) {}
#endif

// Otherwise the print functions are proper functions
// Main print interface
void warning_print(char const *format, ...);
void debug_print(char const *format, ...);
void info_print(char const *format, ...);
void debug_print_matrix_d3(double const a[3][3]);
void debug_print_matrix_i3(int const a[3][3]);
void debug_print_vector_d3(double const a[3]);
void debug_print_vectors_d3(double const a[][3], int size);
void debug_print_vectors_with_label(double const a[][3], int const b[],
                                    int size);

// Common messages
void warning_memory(char const *what);
