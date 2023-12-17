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
#define TOKEN_COMMAND_DELIM_C ' '
#define TOKEN_PIPELINE_DELIM_C '&'

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
    REDIRECT_OVERWRITE,
    REDIRECT_APPEND,
    REDIRECT_NO_OVERWRITE,
} RedirectionMode;
/* Modes of redirections supported:
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
    char *name;
    size_t argc;
    char **argv;
    size_t redirection_count;
    redirection *redirections;
} command;
/*
 * A command is a list of arguments and redirections.
 * For example, the command "ls -l > output.txt" would be parsed as:
 *  - name: "ls"
 *  - argc: 2
 *  - argv: ["ls", "-l", NULL]
 *  - redirection_count: 1
 *  - redirections: [
 *      {
 *          type: REDIRECT_STDOUT,
 *          mode: REDIRECT_OVERWRITE,
 *          filename: "output.txt"
 *      }
 *  ]
 */

typedef struct {
    size_t command_count;
    command **commands;
    bool to_job;
} pipeline;
/* A pipeline is a list of commands with a variable to determine whether
 * it should become a job. */

typedef struct {
    size_t pipeline_count;
    pipeline **pipelines;
} pipeline_list;
/* pipelines is a list of pipeline */

command *parse_command(const char *input);
/* parse_command takes a string and parses it into a command struct.
 * The string is expected to be a single command, with no pipes.
 * The command struct is allocated on the heap, so it must be freed with free_command.
 * If the string is invalid, parse_command returns NULL.
 */

pipeline *parse_pipeline(const char *input, bool to_job, bool can_be_empty);
/* parse_pipeline takes a string and a boolean and parses both into a pipeline struct.
 * The boolean to_job is whether the pipeline should create a new job to execute the command or not.
 * The can_be_empty boolean is used to determine whether this is the last pipeline, or whether a NULL command is accepted.
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
