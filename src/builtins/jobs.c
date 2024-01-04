#include <string.h>

#include "../utils/core.h"
#include "../utils/int_utils.h"
#include "../utils/string_utils.h"

int print_given_jobs_from_argument_index(const command_without_substitution *cmd, size_t start_index) {
    update_status_of_jobs();
    for (size_t i = start_index; i < cmd->argc; ++i) {
        if (cmd->argv[i][0] == '%') {
            if (!is_integer(cmd->argv[i] + 1)) {
                print_error("jobs: %: invalid job id");
                return COMMAND_FAILURE;
            }
            unsigned job_number = atoi(cmd->argv[i] + 1);

            int job_placement = get_jobs_placement_with_id(job_number);
            if (job_placement == -1) {
                print_error("jobs: %: invalid job id");
                return COMMAND_FAILURE;
            }

            char *strjb = simple_str_of_job(jobs[job_placement], false);
            printf("%s\n", strjb);
            free(strjb);
        } else {
            print_error("jobs: invalid argument");
            return COMMAND_FAILURE;
        }
    }
    remove_terminated_jobs(false);
    return SUCCESS;
}

int print_jobs(const command_without_substitution *cmd) {
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

        if (cmd->argv[1][0] == '-') {
            // TODO : take into account the -t option
            print_error("jobs: option -t not yet implemented");
            return COMMAND_FAILURE;
        } else {
            return print_given_jobs_from_argument_index(cmd, 1);
        }
    }
}
