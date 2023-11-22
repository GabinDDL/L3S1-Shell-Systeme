#ifndef CORE_H
#define CORE_H

#include "constants.h"

/* VARIABLES */

extern char *pwd;    // current user position, initialized with HOME from constant
extern char *prompt; // command readout prompt
extern int job_number;
extern int last_command_exit_value;   // last user command exit value
extern char *last_reference_position; // last user location, initialized with HOME from constant

/* FUNCTIONS */

void print_error(const char*);
// print the error message on error output

int init_core();
// initialize constants and then initialize the variables that need them: pwd, prompt and last_reference_position

int free_core();
/* free memory allocation for variables pwd, prompt and last_reference_position other than NULL if different from value
NULL */

#endif
