#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "core.h"

const char *DEFAULT_COLOR = "\001\033[00m\002";
const char *GREEN_COLOR = "\001\033[32m\002";
const char *YELLOW_COLOR = "\001\033[33m\002";
const char *BLUE_COLOR = "\001\033[34m\002";
const char *CYAN_COLOR = "\001\033[36m\002";
const char *RED_COLOR = "\001\033[91m\002";

const char *HOME;

const int SUCCESS = 0;
const int COMMAND_FAILURE = 1;
const int COMMAND_NOT_FOUND = 127;

const size_t PROMPT_MAX_VISIBLE_LEN = 30;
const size_t LITTERAL_CHARS_COUNT = 4;

const int FORMAT_SPECIFIERS_CHARACTERS_COUNT = 6;

void init_const() {
    HOME = getenv("HOME");

    assert(HOME != NULL);
}
