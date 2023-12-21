#ifndef JOBS_H
#define JOBS_H

#include "../utils/core.h"

int print_jobs(const command *);
/**
 * Prints the current jobs
*/

char *str_of_pipeline(pipeline *p);
/**
 * Print the elements of the pipeline p
*/

#endif