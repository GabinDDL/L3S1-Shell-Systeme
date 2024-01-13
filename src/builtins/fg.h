#ifndef FG_H
#define FG_H

#include "../parser/parser.h"

int fg(const command_without_substitution *);
/* Relaunches the job specified in the command argument in the foreground */

#endif
