#ifndef EXTERN_COMMAND_H
#define EXTERN_COMMAND_H

#include "../parser/parser.h"

int extern_command(const command_without_substitution *);
/* Executes an external command, with or without
 * arguments, taking into account the PATH environment variable */

#endif
