#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../src/utils/core.h"

void test_add_job_to_jobs();
void test_remove_job_from_jobs();

void test_core() {
    printf("Test function add_job_to_jobs\n");
    test_add_job_to_jobs();
    printf("Test add_job_to_jobs passed\n");
    printf("Test function remove_job_to_jobs\n");
    test_remove_job_from_jobs();
    printf("Test remove_job_to_jobs passed\n");
}

void test_add_job_to_jobs() {
    assert(jobs == NULL);
    assert(job_number == 0);

    job *j1 = malloc(sizeof(job));
    add_job_to_jobs(j1);

    assert(jobs != NULL);
    assert(job_number == 1);
    assert(jobs[0] == j1);

    job *j2 = malloc(sizeof(job));
    add_job_to_jobs(j2);

    assert(jobs != NULL);
    assert(job_number == 2);
    assert(jobs[0] == j1);
    assert(jobs[1] == j2);

    job *j3 = malloc(sizeof(job));
    add_job_to_jobs(j3);

    assert(jobs != NULL);
    assert(job_number == 3);
    assert(jobs[0] == j1);
    assert(jobs[1] == j2);
    assert(jobs[2] == j3);

    job *j4 = malloc(sizeof(job));
    add_job_to_jobs(j4);

    assert(jobs != NULL);
    assert(job_number == 4);
    assert(jobs[0] == j1);
    assert(jobs[1] == j2);
    assert(jobs[2] == j3);
    assert(jobs[3] == j4);

    free(j1);
    free(j2);
    free(j3);
    free(j4);
    free(jobs);

    jobs = NULL;
    job_number = 0;
}

void test_remove_job_from_jobs() {
    assert(remove_job_from_jobs(0) == COMMAND_FAILURE);
    job *j1 = malloc(sizeof(job));
    j1->id = 0;
    j1->pipeline = NULL;
    add_job_to_jobs(j1);

    assert(remove_job_from_jobs(0) == SUCCESS);
    assert(jobs == NULL);
    assert(job_number == 0);

    job *j2 = malloc(sizeof(job));
    j2->id = 1;
    j2->pipeline = NULL;
    job *j3 = malloc(sizeof(job));
    j3->id = 2;
    j3->pipeline = NULL;
    job *j4 = malloc(sizeof(job));
    j4->id = 3;
    j4->pipeline = NULL;

    j1 = malloc(sizeof(job));
    j1->id = 0;
    j1->pipeline = NULL;
    add_job_to_jobs(j1);
    add_job_to_jobs(j2);
    add_job_to_jobs(j3);
    add_job_to_jobs(j4);

    assert(remove_job_from_jobs(4) == COMMAND_FAILURE);
    assert(remove_job_from_jobs(3) == SUCCESS);
    assert(job_number == 3);
    assert(jobs[0] == j1);
    assert(jobs[1] == j2);
    assert(jobs[2] == j3);
    assert(remove_job_from_jobs(3) == COMMAND_FAILURE);
    assert(remove_job_from_jobs(1) == SUCCESS);
    assert(job_number == 2);
    assert(jobs[0] == j1);
    assert(jobs[1] == j3);
    assert(remove_job_from_jobs(1) == COMMAND_FAILURE);
    assert(remove_job_from_jobs(0) == SUCCESS);
    assert(job_number == 1);
    assert(jobs[0] == j3);
    assert(remove_job_from_jobs(0) == COMMAND_FAILURE);
    assert(remove_job_from_jobs(2) == SUCCESS);
    assert(job_number == 0);
    assert(jobs == NULL);
}
