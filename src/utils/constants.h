#ifndef CONSTANTS_H
#define CONSTANTS_H

/* ENVIRONMENT VARIABLES */

extern const char *HOME;
extern const char *PATH;

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
extern const int FATAL_ERROR;
extern const int COMMAND_NOT_FOUND;

/* FUNCTIONS */

int init_const();
// initializes the PATH and HOME environment variables, which could be erroneous if the value of getenv is not checked

#endif
