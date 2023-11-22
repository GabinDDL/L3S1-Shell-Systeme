#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "core.h"

const char *DEFAULT_COLOR = "\033[00m";
const char *GREEN_COLOR = "\033[32m";
const char *YELLOW_COLOR = "\033[33m";
const char *BLUE_COLOR = "\033[34m";
const char *CYAN_COLOR = "\033[36m";
const char *RED_COLOR = "\033[91m";

const char *HOME;
const char *PATH;

const int SUCCESS = 0;
const int COMMAND_FAILURE = 1;
const int FATAL_ERROR = -1;
const int COMMAND_NOT_FOUND = 127;

int init_const() {
    HOME = getenv("HOME");

    if (HOME == NULL) {
        print_error("Cannot access environment variable $HOME.");
        return FATAL_ERROR;
    }
    PATH = getenv("PATH");

    if (PATH == NULL) {
        print_error("Cannot access environment variable $PATH.");
        return FATAL_ERROR;
    }
    return SUCCESS;
}
