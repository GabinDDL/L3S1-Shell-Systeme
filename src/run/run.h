#ifndef RUN_H
#define RUN_H

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../builtins/builtins.h"
#include "../parser/parser.h"
#include "../utils/constants.h"
#include "../utils/core.h"

char *fd_to_proc_path(int fd);
/*
 * Returns the path of the descriptor in the proc repertory
 */

int fd_from_subtitution_arg_with_pipe(argument *sub_arg, job *j);
/*
 * Returns the descriptor from the substitution created
 */

int run_command_without_redirections(const command_without_substitution *cmd_without_subst, bool is_job, pipeline *pip,
                                     job *j);
/* Run a command, without redirection.
 *
 * Parameters:
 *  - cmd: The command to run with its arguments and redirections.
 *  - is_already_forked : To know if a fork is already created, to
 *    not create a new fork again for extern command
 *  - pip : the pipeline of the command
 *  - j : the possibly job of the pipeline
 * Returns:
 *  - SUCCESS if the command was run successfully.
 *  - COMMAND_FAILURE if the command failed.
 *  - COMMAND_NOT_FOUND if the command was not found.
 *  - Exit value from extern_command if the command was an external command.
 */

int run_command(command *, bool, pipeline *, job *);
/* Run a command, with redirection possible.
 *
 * Parameters:
 *  - cmd: The command to run with its arguments and redirections.
 *  - is_already_forked : To know if a fork is already created, to
 *    not create a new fork again for extern command
 *  - pip : the pipeline of the command
 *  - j : the possibly job of the pipeline
 * Returns:
 *  - SUCCESS if the command was run successfully.
 *  - COMMAND_FAILURE if the command failed.
 *  - COMMAND_NOT_FOUND if the command was not found.
 *  - Exit value from extern_command if the command was an external command.
 */

int run_pipeline(pipeline *, job *);
/* Run a pipeline.
 *
 * Parameters:
 *  - pipeline: The pipeline to run.
 *
 * Returns:
 *  - SUCCESS if the pipeline was run successfully.
 *  - COMMAND_FAILURE if the pipeline failed.
 *  - COMMAND_NOT_FOUND if the pipeline was not found.
 *  - Exit value from extern_command if the pipeline was an external command.
 */

int run_pipeline_list(pipeline_list *pips);
/* Run a list of pipelines.
 *
 * Parameters:
 *  - pipeline_list: The pipeline_list to run.
 *
 * Returns:
 *  - SUCCESS if the pipeline_list was run successfully.
 *  - COMMAND_FAILURE if the last pipeline of pipeline_list failed.
 *  - COMMAND_NOT_FOUND if the last pipeline of pipeline_list was not found.
 *  - Exit value from extern_command if the last pipeline of pipeline_list was an external command.
 */

#endif // RUN_H
