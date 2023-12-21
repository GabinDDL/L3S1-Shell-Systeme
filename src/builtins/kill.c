#include "../parser/parser.h"
#include "../utils/core.h"
#include "../utils/string_utils.h"

#include <signal.h>
#include <unistd.h>

#include "kill.h"

int kill_job(int job_id, int signal) {
    for (size_t i = 0; i < job_number; ++i) {
        if ((*(jobs + i))->id == job_id) {
            if (killpg(getpgid((*(jobs + i))->pid), signal) == -1) {
                print_error("kill: an error occured");
                return COMMAND_FAILURE;
            }
            return SUCCESS;
        }
    }
    print_error("kill: no job with corresponding id");
    return COMMAND_FAILURE;
}

int jsh_kill(const command *cmd) {
    if (cmd->argc < 2) {
        print_error("kill: not enough arguments");
        return COMMAND_FAILURE;
    }

    int signal = SIGTERM;
    char *target = *(cmd->argv + 1);
    if (*target == '-') {
        if (!is_integer(cmd->argv[1] + 1)) {
            print_error("kill: invalid signal");
            return COMMAND_FAILURE;
        }

        signal = atoi(cmd->argv[1] + 1);
        if (signal < 1 || signal > NSIG) {
            print_error("kill: invalid signal");
            return COMMAND_FAILURE;
        }

        target = cmd->argv[2];
    }

    if (*target == '%') {
        if (!is_integer(target + 1)) {
            print_error("kill: invalid argument");
            return COMMAND_FAILURE;
        }
        return kill_job(atoi((target + 1)), signal);
    }

    if (!is_integer(target)) {
        print_error("kill: invalid argument");
        return COMMAND_FAILURE;
    }

    if (killpg(getpgid(atoi(target)), signal) == -1) {
        print_error("kill: an error occured");
        return COMMAND_FAILURE;
    }

    return SUCCESS;
}