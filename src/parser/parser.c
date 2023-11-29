#include "parser.h"

/*
 * Frees the array of tokens.
 * The tokens must have been allocated with malloc.
 * The token_count must be the number of tokens in the array.
 */
void free_tokens(char **tokens, size_t token_count) {
    size_t i = 0;
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
char **tokenize(const char *input, size_t *token_count) {
    char *mutable_input = strdup(input);

    char **tokens = malloc(MAX_TOKENS * sizeof(char *));
    if (tokens == NULL) {
        return NULL;
    }
    size_t i = 0;
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
    size_t token_count = 0;
    char **tokens = tokenize(input, &token_count);

    if (tokens == NULL) {
        return NULL;
    }

    command *cmd = malloc(sizeof(command));

    if (cmd == NULL) {
        free_tokens(tokens, token_count);
        return NULL;
    }

    if (token_count == 0) { // Spaces only
        cmd->name = NULL;
        cmd->argc = 0;
        cmd->argv = NULL;
        cmd->redirection_count = 0;
        cmd->redirections = NULL;

        free_tokens(tokens, token_count);

        return cmd;
    }

    cmd->name = strdup(tokens[0]);
    cmd->argc = token_count; // Takes into account the command name
    cmd->argv = malloc(sizeof(char *) * (cmd->argc + 1));

    if (cmd->argv == NULL) {
        free_tokens(tokens, token_count);
        free(cmd->name);
        free(cmd);
        return NULL;
    }

    size_t i = 0;
    for (i = 0; i < cmd->argc; ++i) {
        cmd->argv[i] = strdup(tokens[i]);
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

    if (cmd->name != NULL)
        free(cmd->name);

    size_t i = 0;
    for (i = 0; i < cmd->argc; ++i) {
        free(cmd->argv[i]);
    }

    if (cmd->argv != NULL)
        free(cmd->argv);

    for (i = 0; i < cmd->redirection_count; ++i) {
        free(cmd->redirections[i].filename);
    }

    if (cmd->redirections != NULL)
        free(cmd->redirections);

    free(cmd);
}