#include <stdio.h>

#include "spglib.h"

int main(int argc, char *argv[]) {
    printf("Spglib version: %s\n", spg_get_version());
    return 0;
}
