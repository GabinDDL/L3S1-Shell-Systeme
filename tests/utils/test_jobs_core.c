#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../src/utils/core.h"
#include "../../src/utils/jobs_core.h"

void test_add_job_to_jobs();
void test_remove_job_from_jobs();
void test_simple_str_of_job_new_job_running();
void test_simple_str_of_job_old_job_detached();
void test_simple_str_of_job_old_job_stopped();
void test_simple_str_of_job_old_job_running();
void test_simple_str_of_job_old_job_killed();
void test_simple_str_of_job_old_job_done();
void test_simple_str_of_job_new_job_running_with_pipe();
void test_simple_str_of_job_old_job_detached_with_pipe();
void test_simple_str_of_job_old_job_stopped_with_pipe();
void test_simple_str_of_job_old_job_running_with_pipe();
void test_simple_str_of_job_old_job_killed_with_pipe();
void test_simple_str_of_job_old_job_done_with_pipe();

void test_jobs_core() {
    printf("Test function add_job_to_jobs\n");
    test_add_job_to_jobs();
    printf("Test add_job_to_jobs passed\n");

    printf("Test function remove_job_to_jobs\n");
    test_remove_job_from_jobs();
    printf("Test remove_job_to_jobs passed\n");

    printf("Test function test_simple_str_of_job_new_job_running\n");
    test_simple_str_of_job_new_job_running();
    printf("Test test_simple_str_of_job_new_job_running passed\n");

    printf("Test function test_simple_str_of_job_old_job_detached\n");
    test_simple_str_of_job_old_job_detached();
    printf("Test test_simple_str_of_job_old_job_detached passed\n");

    printf("Test function test_simple_str_of_job_old_job_stopped\n");
    test_simple_str_of_job_old_job_stopped();
    printf("Test test_simple_str_of_job_old_job_stopped passed\n");

    printf("Test function test_simple_str_of_job_old_job_running\n");
    test_simple_str_of_job_old_job_running();
    printf("Test test_simple_str_of_job_old_job_running passed\n");

    printf("Test function test_simple_str_of_job_old_job_killed\n");
    test_simple_str_of_job_old_job_killed();
    printf("Test test_simple_str_of_job_old_job_killed passed\n");
    
    printf("Test function test_simple_str_of_job_old_job_done\n");
    test_simple_str_of_job_old_job_done();
    printf("Test test_simple_str_of_job_old_job_done passed\n");

    printf("Test function test_simple_str_of_job_new_job_running_with_pipe\n");
    test_simple_str_of_job_new_job_running_with_pipe();
    printf("Test test_simple_str_of_job_new_job_running_with_pipe passed\n");

    printf("Test function test_simple_str_of_job_old_job_detached_with_pipe\n");
    test_simple_str_of_job_old_job_detached_with_pipe();
    printf("Test test_simple_str_of_job_old_job_detached_with_pipe passed\n");

    printf("Test function test_simple_str_of_job_old_job_stopped_with_pipe\n");
    test_simple_str_of_job_old_job_stopped_with_pipe();
    printf("Test test_simple_str_of_job_old_job_stopped_with_pipe passed\n");

    printf("Test function test_simple_str_of_job_old_job_running_with_pipe\n");
    test_simple_str_of_job_old_job_running_with_pipe();
    printf("Test test_simple_str_of_job_old_job_running_with_pipe passed\n");

    printf("Test function test_simple_str_of_job_old_job_killed_with_pipe\n");
    test_simple_str_of_job_old_job_killed_with_pipe();
    printf("Test test_simple_str_of_job_old_job_killed_with_pipe passed\n");

    printf("Test function test_simple_str_of_job_old_job_done_with_pipe\n");
    test_simple_str_of_job_old_job_done_with_pipe();
    printf("Test test_simple_str_of_job_old_job_done_with_pipe passed\n");
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
    j1->job_process = NULL;
    add_job_to_jobs(j1);

    assert(remove_job_from_jobs(0) == SUCCESS);
    assert(jobs == NULL);
    assert(job_number == 0);

    job *j2 = malloc(sizeof(job));
    j2->id = 1;
    j2->pipeline = NULL;
    j2->job_process = NULL;
    job *j3 = malloc(sizeof(job));
    j3->id = 2;
    j3->pipeline = NULL;
    j3->job_process = NULL;
    job *j4 = malloc(sizeof(job));
    j4->id = 3;
    j4->pipeline = NULL;
    j4->job_process = NULL;

    j1 = malloc(sizeof(job));
    j1->id = 0;
    j1->pipeline = NULL;
    j1->job_process = NULL;
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

void test_simple_str_of_job_new_job_running() {
    
    // Set up
    job *jb = malloc(sizeof(job));
    jb->id = 1;
    jb->pid_leader = 12345;
    jb->pgid = 12345;
    jb->status = RUNNING;
    jb->job_process = NULL;
    char *input = "sleep 500";
    jb->pipeline = parse_pipeline(input, true);
    add_job_to_jobs(jb);

    // Call the function to test
    char* strjob = simple_str_of_job(jb, true);

    // Check if the string is correct
    assert(strcmp(strjob, "[1]   12345        Running sleep 500") == 0);

    // Clean up
    remove_job_from_jobs(jb->id);
    free(strjob);
}

void test_simple_str_of_job_old_job_detached() {
    
    // Set up
    job *jb = malloc(sizeof(job));
    jb->id = 1;
    jb->pid_leader = 12345;
    jb->pgid = 12345;
    jb->status = DETACHED;
    jb->job_process = NULL;
    char *input = "sleep 500";
    jb->pipeline = parse_pipeline(input, true);
    add_job_to_jobs(jb);
    

    // Call the function to test
    char* strjob = simple_str_of_job(jb, false);

    // Check if the string is correct
    assert(strcmp(strjob, "[1]   12345        Detached    sleep 500") == 0);

    // Clean up
    remove_job_from_jobs(jb->id);
    free(strjob);
}

void test_simple_str_of_job_old_job_stopped() {
    
    // Set up
    job *jb = malloc(sizeof(job));
    jb->id = 10;
    jb->pid_leader = 4234;
    jb->pgid = 4234;
    jb->status = STOPPED;
    jb->job_process = NULL;
    char *input = "wc -l fic1 > /tmp/tutu";
    jb->pipeline = parse_pipeline(input, true);
    add_job_to_jobs(jb);

    // Call the function to test
    char* strjob = simple_str_of_job(jb, false);

    // Check if the string is correct
    assert(strcmp(strjob, "[10]   4234        Stopped    wc -l fic1 > /tmp/tutu") == 0);

    // Clean up
    remove_job_from_jobs(jb->id);
    free(strjob);
}

void test_simple_str_of_job_old_job_running() {
    
    // Set up
    job *jb = malloc(sizeof(job));
    jb->id = 4;
    jb->pid_leader = 591141;
    jb->pgid = 591141;
    jb->status = RUNNING;
    jb->job_process = NULL;
    char *input = "wc -l fic1 > /tmp/tutu";
    jb->pipeline = parse_pipeline(input, true);
    add_job_to_jobs(jb);

    // Call the function to test
    char* strjob = simple_str_of_job(jb, false);

    // Check if the string is correct
    assert(strcmp(strjob, "[4]   591141        Running    wc -l fic1 > /tmp/tutu") == 0);

    // Clean up
    remove_job_from_jobs(jb->id);
    free(strjob);
}

void test_simple_str_of_job_old_job_killed() {
    
    // Set up
    job *jb = malloc(sizeof(job));
    jb->id = 213;
    jb->pid_leader = 546;
    jb->pgid = 546;
    jb->status = KILLED;
    jb->job_process = NULL;
    char *input = "wc -l fic1 > /tmp/tutu";
    jb->pipeline = parse_pipeline(input, true);
    add_job_to_jobs(jb);

    // Call the function to test
    char* strjob = simple_str_of_job(jb, false);

    // Check if the string is correct
    assert(strcmp(strjob, "[213]   546        Killed    wc -l fic1 > /tmp/tutu") == 0);

    // Clean up
    remove_job_from_jobs(jb->id);
    free(strjob);
}

void test_simple_str_of_job_old_job_done() {
    
    // Set up
    job *jb = malloc(sizeof(job));
    jb->id = 0;
    jb->pid_leader = 10000;
    jb->pgid = 10000;
    jb->status = DONE;
    jb->job_process = NULL;
    char *input = "wc -l fic1 > /tmp/tutu";
    jb->pipeline = parse_pipeline(input, true);
    add_job_to_jobs(jb);

    // Call the function to test
    char* strjob = simple_str_of_job(jb, false);

    // Check if the string is correct
    assert(strcmp(strjob, "[0]   10000        Done    wc -l fic1 > /tmp/tutu") == 0);

    // Clean up
    remove_job_from_jobs(jb->id);
    free(strjob);
}

void test_simple_str_of_job_new_job_running_with_pipe() {
    
    // Set up
    job *jb = malloc(sizeof(job));
    jb->id = 1;
    jb->pid_leader = 12345;
    jb->pgid = 12345;
    jb->status = RUNNING;
    jb->job_process = NULL;
    char *input = "./a.out | wc -l > /tmp/tutu";
    jb->pipeline = parse_pipeline(input, true);
    add_job_to_jobs(jb);

    // Call the function to test
    char* strjob = simple_str_of_job(jb, true);

    // Check if the string is correct
    assert(strcmp(strjob, "[1]   12345        Running ./a.out | wc -l > /tmp/tutu") == 0);

    // Clean up
    remove_job_from_jobs(jb->id);
    free(strjob);
}

void test_simple_str_of_job_old_job_detached_with_pipe() {
    
    // Set up
    job *jb = malloc(sizeof(job));
    jb->id = 1;
    jb->pid_leader = 12345;
    jb->pgid = 12345;
    jb->status = DETACHED;
    jb->job_process = NULL;
    char *input = "./a.out | wc -l > /tmp/tutu";
    jb->pipeline = parse_pipeline(input, true);
    add_job_to_jobs(jb);
    

    // Call the function to test
    char* strjob = simple_str_of_job(jb, false);

    // Check if the string is correct
    assert(strcmp(strjob, "[1]   12345        Detached    ./a.out | wc -l > /tmp/tutu") == 0);

    // Clean up
    remove_job_from_jobs(jb->id);
    free(strjob);
}

void test_simple_str_of_job_old_job_stopped_with_pipe() {
    
    // Set up
    job *jb = malloc(sizeof(job));
    jb->id = 10;
    jb->pid_leader = 4234;
    jb->pgid = 4234;
    jb->status = STOPPED;
    jb->job_process = NULL;
    char *input = "./a.out | wc -l > /tmp/tutu";
    jb->pipeline = parse_pipeline(input, true);
    add_job_to_jobs(jb);

    // Call the function to test
    char* strjob = simple_str_of_job(jb, false);

    // Check if the string is correct
    assert(strcmp(strjob, "[10]   4234        Stopped    ./a.out | wc -l > /tmp/tutu") == 0);

    // Clean up
    remove_job_from_jobs(jb->id);
    free(strjob);
}

void test_simple_str_of_job_old_job_running_with_pipe() {
    
    // Set up
    job *jb = malloc(sizeof(job));
    jb->id = 4;
    jb->pid_leader = 591141;
    jb->pgid = 591141;
    jb->status = RUNNING;
    jb->job_process = NULL;
    char *input = "./a.out | wc -l > /tmp/tutu";
    jb->pipeline = parse_pipeline(input, true);
    add_job_to_jobs(jb);

    // Call the function to test
    char* strjob = simple_str_of_job(jb, false);

    // Check if the string is correct
    assert(strcmp(strjob, "[4]   591141        Running    ./a.out | wc -l > /tmp/tutu") == 0);

    // Clean up
    remove_job_from_jobs(jb->id);
    free(strjob);
}

void test_simple_str_of_job_old_job_killed_with_pipe() {
    
    // Set up
    job *jb = malloc(sizeof(job));
    jb->id = 213;
    jb->pid_leader = 546;
    jb->pgid = 546;
    jb->status = KILLED;
    jb->job_process = NULL;
    char *input = "./a.out | wc -l > /tmp/tutu";
    jb->pipeline = parse_pipeline(input, true);
    add_job_to_jobs(jb);

    // Call the function to test
    char* strjob = simple_str_of_job(jb, false);

    // Check if the string is correct
    assert(strcmp(strjob, "[213]   546        Killed    ./a.out | wc -l > /tmp/tutu") == 0);

    // Clean up
    remove_job_from_jobs(jb->id);
    free(strjob);
}

void test_simple_str_of_job_old_job_done_with_pipe() {
    
    // Set up
    job *jb = malloc(sizeof(job));
    jb->id = 0;
    jb->pid_leader = 10000;
    jb->pgid = 10000;
    jb->status = DONE;
    jb->job_process = NULL;
    char *input = "./a.out | wc -l > /tmp/tutu";
    jb->pipeline = parse_pipeline(input, true);
    add_job_to_jobs(jb);

    // Call the function to test
    char* strjob = simple_str_of_job(jb, false);

    // Check if the string is correct
    assert(strcmp(strjob, "[0]   10000        Done    ./a.out | wc -l > /tmp/tutu") == 0);

    // Clean up
    remove_job_from_jobs(jb->id);
    free(strjob);
}
