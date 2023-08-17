#include <stdio.h>

#include "spglib.h"

int main(int argc, char* argv[]) {
    printf("Spglib version: %d.%d.%d", spg_get_major_version(),
           spg_get_minor_version(), spg_get_micro_version());
    return 0;
}
