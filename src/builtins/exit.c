#include "../parser/parser.h"
#include "../utils/constants.h"
#include "../utils/core.h"
#include "../utils/string_utils.h"

#include "exit.h"

int exit_jsh(command_without_substitution *cmd) {
    if (cmd->argc > 2) {
        print_error("exit: too many arguments");
        return COMMAND_FAILURE;
    }
    if (job_number > 0) {
        print_error("exit: warning: there is still at least one active or suspended job");
        return COMMAND_FAILURE;
    }
    int exit_value = last_command_exit_value;
    if (cmd->argc == 2) { // if a return value is specified, exiting the program with this value, else with the value of
                          // the last executed command
        if (!is_integer(cmd->argv[1])) {
            print_error("exit: argument specified must be an integer set between 0 and 255");
            return COMMAND_FAILURE;
        }
        exit_value = atoi(cmd->argv[1]);
        if (exit_value < 0 || exit_value > 255) {
            print_error("exit: argument specified must be an integer set between 0 and 255");
            return COMMAND_FAILURE;
        }
    }
    free_command_without_substitution(cmd);
    free_core();
    exit(exit_value);
}
