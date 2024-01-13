#include "fg.h"
#include "../utils/core.h"
#include "../utils/jobs_core.h"
#include "../utils/string_utils.h"
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wait.h>

int fg(const command_without_substitution *cmd) {
    if (cmd->argc != 2) {
        print_error("fg: incorrect number of arguments\n");
        return COMMAND_FAILURE;
    }
    if (strlen(cmd->argv[1]) < 2 || cmd->argv[1][0] != '%') {
        print_error("fg: invalid job reference\n");
        return COMMAND_FAILURE;
    }
    if (!is_integer(cmd->argv[1] + 1)) {
        print_error("fg: invalid job reference");
        return COMMAND_FAILURE;
    }
    unsigned job_number = atoi(cmd->argv[1] + 1);

    int job_placement = get_jobs_placement_with_id(job_number);
    if (job_placement == -1) {
        print_error("fg: %: invalid job id");
        return COMMAND_FAILURE;
    }

    int status;
    job *j = jobs[job_placement];
    pid_t pgid = j->pgid;
    pid_t pid_leader = j->pid_leader;
    tcsetpgrp(STDERR_FILENO, pgid);
    killpg(pgid, SIGCONT);

    waitpid(pid_leader, &status, WUNTRACED);

    if (WIFSTOPPED(status)) {
        j->status = STOPPED;
        print_job(j, false);
    } else {
        remove_job_from_jobs(j->id);
    }
    tcsetpgrp(STDERR_FILENO, getpgrp());

    return SUCCESS;
}
