#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "constants.h"
#include "core.h"
#include "int_utils.h"

char *current_folder;
char *prompt;
int job_number = 0;
int last_command_exit_value = 0;
char *last_reference_position;
char *last_line_read;

void print_error(const char *error) {
    fprintf(stderr, "%s\n", error);
}

int update_prompt() {
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
    if (prompt == NULL) {
        print_error("Cannot allocate a memory zone.");
        return FATAL_ERROR;
    }

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

    return SUCCESS;
}

int init_core() {
    if (init_const() == FATAL_ERROR) {
        print_error("Cannot allocate a memory zone");
        return FATAL_ERROR;
    }

    if (update_current_folder()) {
        print_error("Cannot access to $PWD.");
        return FATAL_ERROR;
    }

    if (update_prompt() != 0) {
        print_error("Cannot generate prompt");
        return FATAL_ERROR;
    }

    size_t len_home = strlen(HOME);
    last_reference_position = malloc((len_home + 1) * sizeof(char));

    if (last_reference_position == NULL) {
        print_error("Cannot allocate a memory zone.");
        return FATAL_ERROR;
    }
    memmove(last_reference_position, HOME, len_home + 1);

    return SUCCESS;
}

int free_core() {
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
    return SUCCESS;
}

int update_pwd(const char *path) {
    if (chdir(path) != 0) {
        return COMMAND_FAILURE;
    }
    return SUCCESS;
}

int update_current_folder() {
    char *new_current_folder = getcwd(NULL, 0);

    if (new_current_folder == NULL) {
        print_error("Cannot allocate a memory zone.");
        return FATAL_ERROR;
    }
    if (last_reference_position != NULL) {
        free(last_reference_position);
    }
    last_reference_position = current_folder;
    current_folder = new_current_folder;
    return SUCCESS;
}
