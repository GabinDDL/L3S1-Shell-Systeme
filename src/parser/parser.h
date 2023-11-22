#ifndef PARSER_H
#define PARSER_H

typedef enum {
    REDIRECT_STDIN,
    REDIRECT_STDOUT,
    REDIRECT_STDERR,
} RedirectionType;

typedef enum {
    REDIRECT_OVERWRITE,
    REDIRECT_APPEND,
    REDIRECT_NO_OVERWRITE,
} RedirectionMode;

typedef struct {
    RedirectionType type;
    RedirectionMode mode;
    char *filename;
} redirection;

typedef struct {
    char *name;
    int argc;
    char **argv;
    int redirection_count;
    redirection *redirections;
} command;

typedef struct {
    int command_count;
    command *commands;
} pipeline;

command *parse_command(char *input);

#endif