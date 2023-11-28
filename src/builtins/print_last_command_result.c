#include "print_last_command_result.h"
#include "../parser/parser.h"
#include "../utils/core.h"

int print_last_command_result(const command *cmd) {
    if (cmd->argc != 1) {
        print_error("?: too many arguments");
        return COMMAND_FAILURE;
    }
    printf("%d\n", last_command_exit_value);
    return SUCCESS;
}