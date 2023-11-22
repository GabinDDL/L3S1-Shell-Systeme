#include <stdio.h>
#include <stdlib.h>

#include "utils/constants.h"
#include "utils/core.h"

int main() {
    if (init_core() == FATAL_ERROR) {
        free_core();
        exit(EXIT_FAILURE);
    }
    printf("Hello world!\n");
    free_core();
    return 0;
}
