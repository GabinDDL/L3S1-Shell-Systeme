#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

#define MAX_TOKENS 256
#define MAX_TOKEN_SIZE 256
#define TOKEN_DELIM " "

void free_tokens(char **tokens, int token_count) {
    int i = 0;
    for (i = 0; i < token_count; ++i) {
        free(tokens[i]);
    }
    free(tokens);
}

char **tokenize(char *input, int *token_count) {
    char *mutable_input = strdup(input);

    char **tokens = malloc(MAX_TOKENS * sizeof(char *));
    if (tokens == NULL) {
        return NULL;
    }
    int i = 0;
    for (char *token = strtok(mutable_input, TOKEN_DELIM); token != NULL && i < MAX_TOKENS;
         token = strtok(NULL, TOKEN_DELIM)) {
        tokens[i] = malloc(MAX_TOKEN_SIZE * sizeof(char));
        if (tokens[i] == NULL) {
            free_tokens(tokens, i);
            return NULL;
        }
        strcpy(tokens[i], token);
        ++i;
    }

    free(mutable_input);

    *token_count = i;

    return tokens;
}

command *parse_command(char *input) {
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

    cmd->redirection_count = 0;
    cmd->redirections = NULL;

    free_tokens(tokens, token_count);

    return cmd;
}