#include <string.h>

#include "../utils/core.h"
#include "../utils/int_utils.h"

int print_jobs(const command *cmd) {
    if (cmd->argc == 1) {
        update_status_of_jobs();
        for (size_t i = 0; i < job_number; ++i) {
            char *strjb = simple_str_of_job(jobs[i], false);
            printf("%s\n", strjb);
            free(strjb);
        }
        remove_terminated_jobs(false);
        return SUCCESS;
    } else {
        // TODO : For next milestone, take into account the -t option and the %jobs argument
        print_error("jobs: too many arguments");
        return COMMAND_FAILURE;
    }
}
