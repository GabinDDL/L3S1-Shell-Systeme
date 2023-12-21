#ifndef JOBS_CORE_H
#define JOBS_CORE_H

#include "../parser/parser.h"
#include "../builtins/builtins.h"

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

extern int job_number;
extern job **jobs;

/* FUNCTIONS */

void free_jobs_core();
/* Free allocation of all jobs and their pipelines */

char *state_to_string(Status);
/*From a status, returns the corresponding string*/

int add_job_to_jobs(job *);
/* Adds a new job to the job list, and returns SUCCESS if the command succeeds */

int add_new_forked_process_to_jobs(pid_t, pipeline *, Status);
/* Adds a new fork with the pipeline to the job list, and returns SUCCESS if the command succeeds */

int remove_job_from_jobs(unsigned);
/* Removes the job with the given id from the list, and returns SUCCESS if the command succeeds,
 * COMMAND_FAILURE if the job is not found */

void remove_terminated_jobs(bool);
/* Removes jobs from list if done, detached or killed and print it if true is given */


void update_status_of_jobs();
/* Updates job status according to waitpid */
#endif
