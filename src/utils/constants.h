#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdlib.h>

/* ENVIRONMENT VARIABLES */

extern const char *HOME;

/* COLORS */

extern const char *DEFAULT_COLOR;
extern const char *GREEN_COLOR;
extern const char *YELLOW_COLOR;
extern const char *BLUE_COLOR;
extern const char *CYAN_COLOR;
extern const char *RED_COLOR;

/* EXIT CODE */

extern const int SUCCESS;
extern const int COMMAND_FAILURE;
extern const int COMMAND_NOT_FOUND;

/* PROMPT DATA*/

extern const size_t PROMPT_MAX_VISIBLE_LEN; //the maximum length of the prompt (color characters not included)
extern const size_t LITTERAL_CHARS_COUNT; //the number of litteral characters in the prompt (braces, $ and space)

/* FUNCTIONS */

void init_const();
// initializes the PATH and HOME environment variables, which could be erroneous if the value of getenv is not checked

#endif
