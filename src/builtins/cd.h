#ifndef CD_H
#define CD_H

#include "../parser/parser.h"

int cd(const command_without_substitution *);
/* Changes the current working directory to the "ref" directory
 * if it is is valid, the previous working directory if the parameter
 * is -, or $HOME if no parameter is specified.a
 * It is possible to use ~ to reference $HOME.
 * Returns SUCCESS on success, COMMAND_FAILURE on failure. */

#endif
