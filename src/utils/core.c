#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "constants.h"
#include "core.h"

char *current_folder;
char *prompt;
int job_number = 0;
int last_command_exit_value = 0;
char *last_reference_position;

void print_error(const char *error) {
    fprintf(stderr, "%s\n", error);
}

int init_core() {
    if (init_const() == FATAL_ERROR) {
        print_error("Cannot allocate a memory zone");
        return FATAL_ERROR;
    }

    prompt = malloc(0); /* TODO */

    if (update_pwd(HOME) != SUCCESS) {
        print_error("Cannot access to $HOME.");
        return FATAL_ERROR;
    }

    if (update_current_folder()) {
        print_error("Cannot access to $PWD.");
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
