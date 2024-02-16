#ifndef SPGLIB_BASE_H
#define SPGLIB_BASE_H

#include "spglib.h"

#ifdef SPG_TESTING
    #define SPG_API_TEST SPG_API
#else
    #define SPG_API_TEST
#endif

#endif  // SPGLIB_BASE_H
