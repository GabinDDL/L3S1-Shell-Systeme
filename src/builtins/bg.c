#include "bg.h"
#include "../utils/core.h"
#include "../utils/jobs_core.h"
#include "../utils/string_utils.h"
#include <signal.h>
#include <unistd.h>

int bg(const command_without_substitution *cmd) {
    if (cmd->argc != 2) {
        print_error("bg: incorrect number of arguments\n");
        return COMMAND_FAILURE;
    }
    if (strlen(cmd->argv[1]) < 2 || cmd->argv[1][0] != '%') {
        print_error("bg: invalid job reference\n");
        return COMMAND_FAILURE;
    }
    if (!is_integer(cmd->argv[1] + 1)) {
        print_error("bg: invalid job reference");
        return COMMAND_FAILURE;
    }
    unsigned job_number = atoi(cmd->argv[1] + 1);

    int job_placement = get_jobs_placement_with_id(job_number);
    if (job_placement == -1) {
        print_error("bg: %: invalid job id");
        return COMMAND_FAILURE;
    }
    pid_t pgid = jobs[job_placement]->pgid;
    killpg(pgid, SIGCONT);
    return SUCCESS;
}
