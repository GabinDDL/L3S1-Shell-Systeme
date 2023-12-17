#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "core.h"
#include "int_utils.h"

char *current_folder;
char *prompt;
int job_number = 0;
int last_command_exit_value = 0;
char *last_reference_position;
char *last_line_read;
pipeline_list *current_pipeline_list = NULL;
job **jobs = NULL;

void print_error(const char *error) {
    fprintf(stderr, "%s\n", error);
}

void update_prompt() {
    // Defining the size of the prompt
    size_t nb_color_codes_char = strlen(DEFAULT_COLOR) + strlen(YELLOW_COLOR) + strlen(GREEN_COLOR);
    int job_number_len = get_nb_of_digits(job_number);
    size_t prompt_max_len = PROMPT_MAX_VISIBLE_LEN + nb_color_codes_char;
    size_t full_prompt_len = strlen(current_folder) + job_number_len + LITTERAL_CHARS_COUNT + nb_color_codes_char;
    size_t final_prompt_len = (full_prompt_len > prompt_max_len ? prompt_max_len : full_prompt_len) + 1;

    // Allocating memory for the prompt
    if (prompt != NULL) {
        free(prompt);
    }
    prompt = malloc((final_prompt_len + 1) * sizeof(char));
    assert(prompt != NULL);

    // Constructing the prompt
    if (full_prompt_len > prompt_max_len) {
        // Handling the case where the length of the path makes the prompt longer than the max length
        size_t shortened_path_len =
            prompt_max_len - LITTERAL_CHARS_COUNT - job_number_len - nb_color_codes_char - 3; // -3 for the three dots
        size_t start_of_path = strlen(current_folder) - shortened_path_len;
        snprintf(prompt, final_prompt_len + 1, "%s[%d]%s...%s%s$ ", YELLOW_COLOR, job_number, GREEN_COLOR,
                 current_folder + start_of_path, DEFAULT_COLOR);
    } else {
        // Regular case
        snprintf(prompt, final_prompt_len + 1, "%s[%d]%s%s%s$ ", YELLOW_COLOR, job_number, GREEN_COLOR, current_folder,
                 DEFAULT_COLOR);
    }
}

void init_core() {
    update_current_folder();
    update_prompt();

    size_t len_current_folder = strlen(current_folder);
    last_reference_position = malloc((len_current_folder + 1) * sizeof(char));

    assert(last_reference_position != NULL);
    memmove(last_reference_position, current_folder, len_current_folder + 1);
}

void free_job(job *j) {
    if (j == NULL) {
        return;
    }
    if (j->pipeline != NULL) {
        free(j->pipeline);
    }
    free(j);
}

void free_core() {
    if (current_folder != NULL) {
        free(current_folder);
    }
    if (last_reference_position != NULL) {
        free(last_reference_position);
    }
    if (prompt != NULL) {
        free(prompt);
    }
    if (last_line_read != NULL) {
        free(last_line_read);
    }
    if (jobs != NULL) {
        for (size_t i; i < job_number; i++) {
            free_job(jobs[i]);
        }
        free(jobs);
    }
    free_pipeline_list(current_pipeline_list);
}

int change_pwd(const char *path) {
    if (chdir(path) != 0) {
        return COMMAND_FAILURE;
    }
    return SUCCESS;
}

void update_current_folder() {
    char *new_current_folder = getcwd(NULL, 0);
    assert(new_current_folder != NULL);

    if (last_reference_position != NULL) {
        free(last_reference_position);
    }

    last_reference_position = current_folder;
    current_folder = new_current_folder;
}

int add_job_to_jobs(job *j) {
    if (jobs == NULL) {
        jobs = malloc(sizeof(job *));

        assert(jobs != NULL);
        jobs[0] = j;
        job_number++;
        return SUCCESS;
    }
    job **temp = malloc(sizeof(job *) * (job_number + 1));
    assert(temp != NULL);

    memmove(temp, jobs, job_number * sizeof(job *));
    temp[job_number] = j;

    free(jobs);
    jobs = temp;

    job_number++;

    return SUCCESS;
}

int get_jobs_placement_with_id(unsigned id) {
    job *acc;
    for (int i = 0; i < job_number; i++) {
        acc = jobs[i];
        if (acc != NULL && acc->id == id) {
            return i;
        }
    }
    return -1;
}

int remove_job_from_jobs(unsigned id) {
    if (jobs == NULL) {
        return COMMAND_FAILURE;
    }
    int job_placement = get_jobs_placement_with_id(id);

    if (job_placement < 0) {
        return COMMAND_FAILURE;
    }
    job **temp = malloc(sizeof(job *) * (job_number - 1));

    assert(temp != NULL);

    memmove(temp, jobs, job_placement * sizeof(char *));
    memmove(temp + job_placement, jobs + job_placement + 1, (job_number - job_placement - 1) * sizeof(char *));

    job *j_removed = jobs[job_placement];
    free_job(j_removed);

    free(jobs);
    jobs = temp;
    job_number--;

    if (job_number == 0) {
        free(jobs);
        jobs = NULL;
    }
    return SUCCESS;
}
