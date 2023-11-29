#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser/parser.h"
#include "run/run.h"
#include "utils/constants.h"
#include "utils/core.h"

int main() {
    if (init_core() == FATAL_ERROR) {
        free_core();
        exit(EXIT_FAILURE);
    }

    rl_outstream = stderr;
    while (1) {
        char *line = readline(prompt);
        if (line == NULL) {
            break;
        }
        add_history(line);

        command *cmd = parse_command(line);

        if (cmd == NULL) {
            free(line);
            free_core();
            return EXIT_FAILURE;
        }

        int run_output = run_command(cmd);

        if (run_output == FATAL_ERROR) {
            free(line);
            free_command(cmd);
            free_core();
            return EXIT_FAILURE;
        }

        last_command_exit_value = run_output;

        free(line);
        free_command(cmd);
    }

    free_core();
    return last_command_exit_value;
}
