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
    int status; // status of the created process
    pid_t pid = fork();
    int res_exec;

    assert(pid != -1);

    switch (pid) {
    case 0:
        res_exec = execvp(cmd->name, cmd->argv);

        if (res_exec < 0) {
            perror("execvp");
            exit(errno);
        }
        exit(SUCCESS);
        break;
    default:
        waitpid(pid, &status, 0);
        assert(WIFEXITED(status));

        return WEXITSTATUS(status);
    }
}
