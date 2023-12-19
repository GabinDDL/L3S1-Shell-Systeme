#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../parser/parser.h"
#include "../utils/constants.h"
#include "../utils/core.h"
#include "extern_command.h"

int extern_command(const command *cmd) {
    int res_exec;

    res_exec = execvp(cmd->name, cmd->argv);
    return res_exec;
}
