#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "../parser/parser.h"
#include "../utils/constants.h"
#include "../utils/core.h"
#include "../utils/string_utils.h"
#include "cd.h"

char *get_correct_path(const char *);
/* Returns the string, allocated and if the
 * path starts with ~, replaces it with $HOME */

int cd(const command *cmd) {
    if (cmd->argc > 2) { // Checks if its the good number of arguments
        print_error("cd: too many arguments");
        return COMMAND_FAILURE;
    }

    int res_command;
    if (cmd->argc == 1) { // No argument case
        if ((res_command = change_pwd(HOME)) != SUCCESS) {
            return res_command;
        }
        update_current_folder();
        return SUCCESS;
    }
    if (strcmp(cmd->argv[1], "-") == 0) { // Last reference case
        if ((res_command = change_pwd(last_reference_position)) != SUCCESS) {
            return res_command;
        }
        update_current_folder();
        return SUCCESS;
    }
    char *correct_path = get_correct_path(cmd->argv[1]); // Corrects the path for ~

    assert(correct_path != NULL);
    struct stat st;

    if (stat(correct_path, &st) != 0) { // Checks if the directory exists
        print_error("cd: no such file or directory");
        free(correct_path);
        return COMMAND_FAILURE;
    }
    if ((st.st_mode & S_IFMT) != S_IFDIR) { // Checks if its a directory
        print_error("cd: not a directory");
        free(correct_path);
        return COMMAND_FAILURE;
    }
    if ((res_command = change_pwd(correct_path)) != SUCCESS) { // Updates pwd
        free(correct_path);
        return res_command;
    }

    update_current_folder(); // Updates current_folder var

    free(correct_path);

    return SUCCESS;
}

char *get_correct_path(const char *path) {
    char *new_path;

    if (start_with(path, "~")) {
        if (strlen(path) == 1) {
            size_t len_home = strlen(HOME);
            new_path = malloc((len_home + 1) * sizeof(char));

            assert(new_path != NULL);
            memmove(new_path, HOME, len_home + 1);
            return new_path;
        }
        return concat_with_delimiter(HOME, path + 2, '/');
    }
    size_t len_path = strlen(path);
    new_path = malloc((len_path + 1) * sizeof(char));
    assert(new_path != NULL);

    memmove(new_path, path, len_path + 1);
    return new_path;
}
