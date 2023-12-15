#include "parser.h"
#include "../utils/string_utils.h"
#include <stdlib.h>
#include <string.h>

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
char **tokenize(const char *input, size_t *token_count, const char *delimiter) {
    char *mutable_input = strdup(input);

    char **tokens = malloc(MAX_TOKENS * sizeof(char *));
    if (tokens == NULL) {
        return NULL;
    }
    size_t i = 0;
    for (char *token = strtok(mutable_input, delimiter); token != NULL && i < MAX_TOKENS;
         token = strtok(NULL, delimiter)) {
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
    char **tokens = tokenize(input, &token_count, TOKEN_COMMAND_DELIM);

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

pipeline *parse_pipeline(const char *input, bool to_job) {
    pipeline *pip = malloc(sizeof(pipeline));

    pip->commands = malloc(sizeof(command *));
    if (pip->commands == NULL) {
        return NULL;
    }

    pip->commands[0] = parse_command(input);
    pip->command_count = 1;
    if (pip->commands[0] == NULL) {
        free(pip);
        return NULL;
    }
    if (pip->commands[0]->name == NULL) {
        free_pipeline(pip);
        return NULL;
    }
    pip->to_job = to_job;

    // TODO: Pipe and redirections sequel
    return pip;
}

pipeline_list *parse_pipeline_list(const char *input) {
    if (has_sequence_of(input, TOKEN_PIPELINE_DELIM_C) ||
        start_with_exception(input, TOKEN_PIPELINE_DELIM, TOKEN_COMMAND_DELIM_C)) {
        return NULL;
    }

    size_t token_count = 0;
    char **tokens = tokenize(input, &token_count, TOKEN_PIPELINE_DELIM);

    if (tokens == NULL) {
        exit(EXIT_FAILURE);
    }

    pipeline_list *pips = malloc(sizeof(pipeline_list));
    if (pips == NULL) {
        free_tokens(tokens, token_count);
        exit(EXIT_FAILURE);
    }
    if (token_count == 0) {
        pips->pipelines = NULL;
        pips->pipeline_count = 0;
        free_tokens(tokens, token_count);

        return pips;
    }
    pips->pipeline_count = 0;
    pips->pipelines = malloc(sizeof(pipeline *) * token_count);
    for (size_t i = 0; i < token_count - 1; i++) {
        pips->pipelines[i] = parse_pipeline(tokens[i], true);

        if (pips->pipelines[i] == NULL) {
            free_tokens(tokens, token_count);
            free_pipeline_list(pips);
            return NULL;
        }
        pips->pipeline_count += 1;
    }
    bool last_pipeline_to_job = input[strlen(input) - 1] == TOKEN_PIPELINE_DELIM_C;
    pips->pipelines[token_count - 1] = parse_pipeline(tokens[token_count - 1], last_pipeline_to_job);
    pips->pipeline_count += 1;

    free_tokens(tokens, token_count);

    return pips;
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

void free_pipeline(pipeline *pip) {
    if (pip == NULL) {
        return;
    }
    if (pip->commands == NULL) {
        free(pip);
        return;
    }
    for (size_t i = 0; i < pip->command_count; i++) {
        if (pip->commands[i] != NULL) {
            free_command(pip->commands[i]);
        }
    }
    free(pip->commands);
    free(pip);
}

void free_pipeline_list(pipeline_list *pips) {
    if (pips == NULL) {
        return;
    }
    if (pips->pipelines == NULL) {
        free(pips);
        return;
    }
    for (size_t i = 0; i < pips->pipeline_count; i++) {
        if (pips->pipelines[i] != NULL) {
            free_pipeline(pips->pipelines[i]);
        }
    }
    free(pips->pipelines);
    free(pips);
}

void free_pipeline_list_without_jobs(pipeline_list *pips) {
    if (pips == NULL) {
        return;
    }
    if (pips->pipelines == NULL) {
        free(pips);
        return;
    }
    for (size_t i = 0; i < pips->pipeline_count; i++) {
        if (pips->pipelines[i] != NULL && !pips->pipelines[i]->to_job) {
            free_pipeline(pips->pipelines[i]);
        }
    }
    free(pips->pipelines);
    free(pips);
}
