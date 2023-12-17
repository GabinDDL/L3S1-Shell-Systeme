#ifndef RUN_H
#define RUN_H

#include <stdio.h>
#include <stdlib.h>

#include "../utils/core.h"
#include "../parser/parser.h"
#include "../utils/constants.h"
#include "../builtins/builtins.h"

int run_command(const command *cmd);
/* Run a command.
 * 
 * Parameters:
 *  - cmd: The command to run.
 * 
 * Returns:
 *  - SUCCESS if the command was run successfully.
 *  - COMMAND_FAILURE if the command failed.
 *  - COMMAND_NOT_FOUND if the command was not found.
 *  - Exit value from extern_command if the command was an external command.
 */

int run_pipeline(const pipeline *pip);
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

int run_pipeline_list(const pipeline_list *pips);
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

#endif  // RUN_H
