#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../parser/parser.h"
#include "../utils/constants.h"
#include "../utils/core.h"
#include "extern_command.h"

int extern_command(const command *cmd) {
    int res;    // result of extern command
    int status; // status of the created process
    pid_t pid = fork();

    switch (pid) {
    case 0:
        res = execvp(cmd->name, cmd->argv);
        perror(cmd->name);
        exit(res);
        break;
    case -1:
        print_error("Error during process creation.");
        return FATAL_ERROR;
    default:
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status)) {
            print_error("Error during process termination.");
            return FATAL_ERROR;
        }
        return WEXITSTATUS(status);
    }
}
