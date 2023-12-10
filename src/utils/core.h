#ifndef CORE_H
#define CORE_H

#include "../parser/parser.h"
#include "constants.h"
#include <sys/types.h>

/* ENUM */

typedef enum { RUNNING, STOPPED, DETACHED, KILLED, DONE } Status;

/* STRUCTURES */

typedef struct process {
    pid_t pid;
    Status status;
    command *cmd;
} process;

typedef struct job {
    unsigned id;
    pid_t pid;
    Status status;
    pipeline *pipeline;
} job;

/* VARIABLES */

extern char *current_folder; // current user position, initialized with PWD from constant
extern char *prompt;         // command readout prompt
extern int job_number;
extern int last_command_exit_value;   // last user command exit value
extern char *last_reference_position; // last user location, initialized with PWD from constant
extern char *last_line_read;          // last line typed by the user
extern job **jobs;

/* FUNCTIONS */

void print_error(const char *);
// print the error message on error output

int update_prompt();
// update the prompt according to the current position

int init_core();
// initialize constants and then initialize the variables that need them: current_folder, prompt and
// last_reference_position

int free_core();
/* free memory allocation for variables pwd, prompt and last_reference_position other than NULL if different from value
NULL */

int update_pwd(const char *);
/* updates the pwd environment variable by changing it to the string passed as argument, and returns an error otherwise
 */

int update_current_folder();
/* Changes the folder variable current_folder to the reference of the current directory */

int add_job_to_jobs(job *);
/* Adds a new job to the job list, and returns SUCESS if the command succeeds and FATAL_ERROR if there is an allocation
 * problem */

int remove_job_from_jobs(unsigned);
/* Removes the job with the given id from the list, and returns SUCESS if the command succeeds,
 * COMMAND_FAILURE if the job is not found, and FATAL_ERROR if there is an allocation problem */

#endif
