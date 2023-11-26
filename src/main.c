#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/constants.h"
#include "utils/core.h"

int main() {
    if (init_core() == FATAL_ERROR) {
        free_core();
        exit(EXIT_FAILURE);
    }

    rl_outstream = stderr;
    char *line = readline(prompt);
    add_history(line);

    printf("%s\n", line);
    free(line);

    free_core();
    return 0;
}
