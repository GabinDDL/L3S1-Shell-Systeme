#ifndef CORE_H
#define CORE_H

#include "constants.h"

/* VARIABLES */

extern char *current_folder; // current user position, initialized with PWD from constant
extern char *prompt;         // command readout prompt
extern int job_number;
extern int last_command_exit_value;   // last user command exit value
extern char *last_reference_position; // last user location, initialized with PWD from constant
extern char *last_line_read; // last line typed by the user

/* FUNCTIONS */

void print_error(const char *);
// print the error message on error output

int update_prompt();
//update the prompt according to  the current position

int init_core();
// initialize constants and then initialize the variables that need them: pwd, prompt and last_reference_position

int free_core();
/* free memory allocation for variables pwd, prompt and last_reference_position other than NULL if different from value
NULL */

int update_pwd(const char *);
/* updates the pwd environment variable by changing it to the string passed as argument, and returns an error otherwise
 */

int update_current_folder();
/* Changes the folder variable to the reference of the current directory */

#endif
