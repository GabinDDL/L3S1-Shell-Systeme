#include <stdio.h>

#include "../parser/parser.h"
#include "../utils/constants.h"
#include "../utils/core.h"
#include "pwd.h"

int pwd(const command_without_substitution *cmd) {
    if (cmd->argc != 1) {
        print_error("pwd: too many arguments\n");
        return COMMAND_FAILURE;
    }
    printf("%s\n", current_folder);

    return SUCCESS;
}
