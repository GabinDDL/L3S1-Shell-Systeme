#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKENS 256
#define TOKEN_COMMAND_DELIM " "
#define TOKEN_PIPELINE_DELIM "&"
#define TOKEN_PIPE_DELIM_WITHOUT_SPACE "|"
#define TOKEN_PIPE_DELIM " | "
#define TOKEN_COMMAND_DELIM_C ' '
#define TOKEN_PIPELINE_DELIM_C '&'
#define TOKEN_PIPE_DELIM_C '|'

typedef struct pipeline pipeline;

typedef enum {
    REDIRECT_STDIN,
    REDIRECT_STDOUT,
    REDIRECT_STDERR,
} RedirectionType;
/* Types of redirections supported:
 *  - Redirect stdin
 *  - Redirect stdout
 *  - Redirect stderr
 */

typedef enum {
    REDIRECT_NONE,
    REDIRECT_OVERWRITE,
    REDIRECT_APPEND,
    REDIRECT_NO_OVERWRITE,
} RedirectionMode;
/* Modes of redirections supported:
 *  - No redirection mode (stdin)
 *  - Overwrite the file
 *  - Append to the file
 *  - Don't overwrite the file
 */

typedef struct {
    RedirectionType type;
    RedirectionMode mode;
    char *filename;
} redirection;

typedef struct {
    // Enum to determine the type of argument
    enum { ARG_SIMPLE, ARG_SUBSTITUTION } type;

    // Union to store the actual argument value
    union {
        char *simple;      // Pointer to a simple string argument
        pipeline *substitution;  // Pointer to a pipeline for substitution
    } value;
} argument;

typedef struct {
    char *name;
    size_t argc;
    argument **argv;
    size_t redirection_count;
    redirection *redirections;
} command;
/*
    * A command is a single command with its arguments and redirections.
    * For example, the command "ls -l < (cat file.txt) > file2.txt" will be parsed as:
    * - name: "ls"
    * - argc: 3
    * - argv: (list of argument) 
    *   | type: ARG_SIMPLE, value: "ls"
    *   | type: ARG_SIMPLE, value: "-l"
    *   | type: ARG_SUBSTITUTION, value: pipeline
    *       - command_count: 1
    *       - commands:
    *           - name: "cat"
    *           - argc: 2
    *           - argv: (list of argument)
    *               | type: ARG_SIMPLE, value: "cat"
    *               | type: ARG_SIMPLE, value: "file.txt"
    *               | NULL
    *       - to_job: false 
    *   | NULL
    * - redirection_count: 1
    * - redirections:
    *   - type: REDIRECT_STDOUT
    *   - mode: REDIRECT_NO_OVERWRITE
    *   - filename: "file2.txt"
    */

typedef struct {
    char *name;
    size_t argc;
    char **argv;
    size_t redirection_count;
    redirection *redirections;
} command_without_substitution;
/*
 * A command without substitution is a command with its arguments as strings and redirections.
 */

struct pipeline{
    size_t command_count;
    command **commands;
    bool to_job;
};
/* A pipeline is a list of commands with a variable to determine whether
 * it should become a job. */


typedef struct {
    size_t pipeline_count;
    pipeline **pipelines;
} pipeline_list;
/* pipelines is a list of pipeline */

char *str_of_pipeline(pipeline *p);
/**
 * Prints a pipeline on a single line
 */

void free_tokens(char **, size_t);
/* Frees tokens */

char **tokenize(const char *input, size_t *token_count, const char *delim);
/*
 * Tokenizes the input string into an array of tokens.
 * The tokens are separated by whitespace.
 * The token_count is set to the number of tokens.
 * The returned array of tokens must be freed by the caller.
 */

char **tokenize_command_with_special_pipe(const char *input, size_t *token_count);
/*
 * Tokenizes the input string into an array of tokens for parsing a command with special pipe.
 */

char **tokenize_pipeline_with_special_pipe(const char *input, size_t *token_count);
/*
 * Tokenizes the input string into an array of tokens for parsing a pipeline with special pipe.
 */

int is_substitution(const char *token);
/* Checks if a token is a pipe substitution */

command *parse_command(const char *input);
/* parse_command takes a string and parses it into a command struct.
 * The string is expected to be a single command, with no pipes.
 * The command struct is allocated on the heap, so it must be freed with free_command.
 * If the string is invalid, parse_command returns NULL.
 */

pipeline *parse_pipeline(const char *input, bool to_job);
/* parse_pipeline takes a string and a boolean and parses both into a pipeline struct.
 * The boolean to_job is whether the pipeline should create a new job to execute the command or not.
 * A job pipeline cannot be empty, so it will returns NULL if the command is NULL.
 * The string is expected to be a single command, with no pipes.
 * The pipeline struct is allocated on the heap, so it must be freed with free_pipeline.
 * If a command is incorrect or if it's a NULL command and not the last one, returns NULL */

pipeline_list *parse_pipeline_list(const char *input);
/* parse_pipeline_list takes a string and parses it into a pipeline_list struct.
 * The string is expected to be few command, with no pipes, delimited by &.
 * The pipeline_list struct is allocated on the heap, so it must be freed with free_pipeline_list.
 * If the string is invalid, parse_pipeline_list returns NULL.
 */

void free_command(command *cmd);
/* free_command frees the memory allocated by parse_command,
 * the command struct and its fields.*/

void free_command_without_substitution(command_without_substitution *cmd);

void free_pipeline(pipeline *pip);
/* free_pipeline frees the memory allocated by parse_pipeline,
 * the pipeline struct and its fields.*/

void free_pipeline_list(pipeline_list *pips);
/* free_pipeline_list frees the memory allocated by parse_pipeline_list,
 * the pipelines, pipelines struct, and their fields */

void free_pipeline_list_without_jobs(pipeline_list *pips);
/* free_pipeline_list_without_jobs frees the memory allocated by parse_pipeline_list,
 * the pipelines, pipelines struct with to_job false, and their commands */

#endif
