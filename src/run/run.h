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
 *  - FATAL_ERROR if there was a fatal error.
 *  - COMMAND_FAILURE if the command failed.
 *  - COMMAND_NOT_FOUND if the command was not found.
 *  - Exit value from extern_command if the command was an external command.
 */

#endif  // RUN_H