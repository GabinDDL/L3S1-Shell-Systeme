#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>
#include <unistd.h>

#include "constants.h"
#include "core.h"

char *pwd;
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
    size_t len_home = strlen(HOME);
    pwd = malloc(len_home + 1);

    if (pwd == NULL) {
        print_error("Cannot allocate a memory zone.");
        return FATAL_ERROR;
    }
    memmove(pwd, HOME, len_home + 1);

    prompt = malloc(0); /* TODO */
    last_reference_position = malloc(len_home + 1);

    if (last_reference_position == NULL) {
        print_error("Cannot allocate a memory zone.");
        return FATAL_ERROR;
    }
    memmove(last_reference_position, HOME, len_home + 1);

    if (chdir(pwd) != 0) {
        print_error("Cannot move into $HOME folder.");
        return FATAL_ERROR;
    }

    return SUCCESS;
}

int free_core() {
    if (pwd != NULL) {
        free(pwd);
    }
    if (last_reference_position != NULL) {
        free(last_reference_position);
    }
    if (prompt != NULL) {
        free(prompt);
    }
    return SUCCESS;
}
