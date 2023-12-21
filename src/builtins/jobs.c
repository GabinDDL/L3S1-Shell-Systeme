#include <string.h>

#include "../utils/core.h"
#include "../utils/int_utils.h"

char *str_of_output_redirection(output_redirection *redir) {

    char *s;
    if (redir->type == REDIRECT_STDOUT && redir->mode == REDIRECT_NO_OVERWRITE) {
        s = strdup(">");
    } else if (redir->type == REDIRECT_STDOUT && redir->mode == REDIRECT_OVERWRITE) {
        s = strdup(">|");
    } else if (redir->type == REDIRECT_STDOUT && redir->mode == REDIRECT_APPEND) {
        s = strdup(">>");
    } else if (redir->type == REDIRECT_STDERR && redir->mode == REDIRECT_NO_OVERWRITE) {
        s = strdup("2>");
    } else if (redir->type == REDIRECT_STDERR && redir->mode == REDIRECT_OVERWRITE) {
        s = strdup("2>|");
    } else {
        s = strdup("2>>");
    }

    size_t result_length = strlen(s) + strlen(redir->filename) + 3;
    char *result = malloc(result_length * sizeof(char));
    snprintf(result, result_length, " %s %s", s, redir->filename);
    free(s);
    return result;
}

/**
 * Prints a command, its arguments and options
 */
char *str_of_command(const command *cmd) {
    size_t result_length = 1;

    for (size_t i = 0; i < cmd->argc; ++i) {
        result_length += strlen(cmd->argv[i]);
        if (i < cmd->argc - 1) {
            result_length += 1;
        }
    }

    if (cmd->input_redirection_filename != NULL) {
        result_length += strlen(cmd->input_redirection_filename) + 3;
    }

    for (size_t i = 0; i < cmd->output_redirection_count; ++i) {
        RedirectionType redir_type = (cmd->output_redirections + 1)->type;
        RedirectionMode redir_mode = (cmd->output_redirections + 1)->mode;

        if (redir_type == REDIRECT_STDOUT && redir_mode == REDIRECT_NO_OVERWRITE) {
            result_length += 3 + strlen((cmd->output_redirections + i)->filename);
        } else if (redir_type == REDIRECT_STDERR && redir_mode == REDIRECT_APPEND) {
            result_length += 5 + strlen((cmd->output_redirections + i)->filename);
        } else {
            result_length += 4 + strlen((cmd->output_redirections + i)->filename);
        }
    }

    char *result = malloc(result_length * sizeof(char));
    int marker = 0;

    for (size_t i = 0; i < cmd->argc; ++i) {
        marker += snprintf(result + marker, strlen(cmd->argv[i]) + 1, "%s", cmd->argv[i]);
        if (i < cmd->argc - 1) {
            result[marker] = ' ';
            marker++;
        }
    }

    if (cmd->input_redirection_filename != NULL) {
        marker += snprintf(result + marker, strlen(cmd->input_redirection_filename) + 4, "< %s ",
                           cmd->input_redirection_filename);
    }

    for (size_t i = 0; i < cmd->output_redirection_count; ++i) {
        char *redirection = str_of_output_redirection(cmd->output_redirections + i);
        marker += snprintf(result + marker, strlen(redirection) + 1, "%s", redirection);
        free(redirection);
    }

    result[marker] = '\0';
    return result;
}

/**
 * Prints a pipeline on a single line
 */
char *str_of_pipeline(pipeline *p) {
    int result_length = 1;

    for (size_t i = 0; i < p->command_count; ++i) {
        char *cmd = str_of_command(p->commands[i]);
        result_length += strlen(cmd);
        free(cmd);
        if (i < p->command_count - 1) {
            result_length += 4;
        }
    }

    char *result = malloc(result_length * sizeof(char));
    int marker = 0;

    for (size_t i = 0; i < p->command_count; ++i) {
        char *cmd = str_of_command(p->commands[i]);
        marker += snprintf(result, strlen(cmd) + 1, "%s", cmd);
        free(cmd);
        if (i < p->command_count - 1) {
            result[marker] = ' ';
            result[marker + 1] = '|';
            result[marker + 2] = ' ';
            marker += 3;
        }
        free(cmd);
    }

    result[marker] = '\0';
    return result;
}

/**
 * Prints all informations from a job on a single line
 * For example :
 * [4] 591141  Running  ./a.out | wc -l > /tmp/tutu
 */
char *simple_str_of_job(job *j) {
    char *status = state_to_string(j->status);
    char *pipeline = str_of_pipeline(j->pipeline);

    int result_length = strlen(status) + strlen(pipeline) + get_nb_of_digits(j->id) + get_nb_of_digits(j->pid) + 17;
    char *result = malloc(result_length * sizeof(char));

    snprintf(result, result_length, "[%u]   %d        %s    %s", j->id, j->pid, status, pipeline);

    free(status);
    free(pipeline);

    return result;
}

int print_jobs(const command *cmd) {
    if (cmd->argc == 1) {
        update_status_of_jobs();
        for (size_t i = 0; i < job_number; ++i) {
            char *strjb = simple_str_of_job(jobs[i]);
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
