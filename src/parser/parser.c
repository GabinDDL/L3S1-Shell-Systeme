#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

#define MAX_TOKENS 256
#define TOKEN_DELIM " "

/*
 * Frees the array of tokens.
 * The tokens must have been allocated with malloc.
 * The token_count must be the number of tokens in the array.
 */
void free_tokens(char **tokens, int token_count) {
    int i = 0;
    for (i = 0; i < token_count; ++i) {
        free(tokens[i]);
    }
    free(tokens);
}

/*
 * Tokenizes the input string into an array of tokens.
 * The tokens are separated by whitespace.
 * The token_count is set to the number of tokens.
 * The returned array of tokens must be freed by the caller.
 * Returns NULL if the input string is empty or if there is an error.
 */
char **tokenize(const char *input, int *token_count) {
    char *mutable_input = strdup(input);

    char **tokens = malloc(MAX_TOKENS * sizeof(char *));
    if (tokens == NULL) {
        return NULL;
    }
    int i = 0;
    for (char *token = strtok(mutable_input, TOKEN_DELIM); token != NULL && i < MAX_TOKENS;
         token = strtok(NULL, TOKEN_DELIM)) {
        size_t token_length = strlen(token);
        tokens[i] = malloc(sizeof(char) * (token_length + 1));
        if (tokens[i] == NULL) {
            free_tokens(tokens, i);
            return NULL;
        }
        strncpy(tokens[i], token, token_length + 1);
        ++i;
    }

    free(mutable_input);

    *token_count = i;

    return tokens;
}

command *parse_command(const char *input) {
    int token_count = 0;
    char **tokens = tokenize(input, &token_count);

    if (token_count == 0) {
        free_tokens(tokens, token_count);
        return NULL;
    }

    command *cmd = malloc(sizeof(command));

    if (cmd == NULL) {
        free_tokens(tokens, token_count);
        return NULL;
    }

    cmd->name = strdup(tokens[0]);
    cmd->argc = token_count - 1;
    cmd->argv = malloc(sizeof(char *) * (cmd->argc + 1));

    if (cmd->argv == NULL) {
        free_tokens(tokens, token_count);
        free(cmd->name);
        free(cmd);
        return NULL;
    }

    int i = 0;
    for (i = 0; i < cmd->argc; ++i) {
        cmd->argv[i] = strdup(tokens[i + 1]);
    }
    cmd->argv[cmd->argc] = NULL;

    // TODO: Parse redirections
    cmd->redirection_count = 0;
    cmd->redirections = NULL;

    free_tokens(tokens, token_count);

    return cmd;
}

void free_command(command *cmd) {
    if (cmd == NULL) {
        return;
    }

    free(cmd->name);

    int i = 0;
    for (i = 0; i < cmd->argc; ++i) {
        free(cmd->argv[i]);
    }
    free(cmd->argv);

    for (i = 0; i < cmd->redirection_count; ++i) {
        free(cmd->redirections[i].filename);
    }
    free(cmd->redirections);

    free(cmd);
}