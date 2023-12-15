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
    if (init_core() == FATAL_ERROR || init_const() == FATAL_ERROR) {
        free_core();
        exit(EXIT_FAILURE);
    }

    rl_outstream = stderr;
    while (1) {
        last_line_read = readline(prompt);
        if (last_line_read == NULL) {
            break;
        }
        add_history(last_line_read);

        current_pipeline_list = parse_pipeline_list(last_line_read);

        if (current_pipeline_list == NULL) {
            print_error("jsh: parse error");
            last_command_exit_value = COMMAND_FAILURE;
            continue;
        }

        int run_output = run_pipeline_list(current_pipeline_list);

        if (run_output == FATAL_ERROR) {
            free_pipeline_list(current_pipeline_list);
            free_core();
            return EXIT_FAILURE;
        }

        last_command_exit_value = run_output;

        free(last_line_read);
        free_pipeline_list_without_jobs(current_pipeline_list);
        current_pipeline_list = NULL;
    }

    free_core();
    return last_command_exit_value;
}
