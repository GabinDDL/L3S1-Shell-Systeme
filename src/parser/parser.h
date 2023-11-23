#ifndef PARSER_H
#define PARSER_H

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
    int argc;
    char **argv;
    int redirection_count;
    redirection *redirections;
} command;
/*
    * A command is a list of arguments and redirections.
    * For example, the command "ls -l > output.txt" would be parsed as:
    *  - name: "ls"
    *  - argc: 2
    *  - argv: ["ls", "-l"]
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
    int command_count;
    command *commands;
} pipeline;
/*
    * A pipeline is a list of commands.
    * For example, the command "ls -l | grep foo" would be parsed as:
    *  - command_count: 2
    *  - commands: [
    *      {
    *          name: "ls",
    *          argc: 2,
    *          argv: ["ls", "-l"],
    *          redirection_count: 0,
    *          redirections: []
    *      },
    *      {
    *          name: "grep",
    *          argc: 1,
    *          argv: ["grep", "foo"],
    *          redirection_count: 0,
    *          redirections: []
    *      }
    *  ]
*/

command *parse_command(const char *input);
/* parse_command takes a string and parses it into a command struct. 
 * The string is expected to be a single command, with no pipes.
 * The command struct is allocated on the heap, so it must be freed with free_command.
 * If the string is invalid, parse_command returns NULL.
*/

void free_command(command *cmd);
// free_command frees the memory allocated by parse_command, the command struct and its fields.

#endif