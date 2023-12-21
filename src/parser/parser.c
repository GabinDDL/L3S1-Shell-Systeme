#include "parser.h"
#include "../utils/string_utils.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/**
 * Returns the string corresponding to an output_redirection
 * Example :
 *  - " >> a.out"
 */
char *str_of_output_redirection(output_redirection *redir) {

    char *s;
    if (redir->type == REDIRECT_STDOUT && redir->mode == REDIRECT_NO_OVERWRITE) {
        s = strdup(">");
    } else if (redir->type == REDIRECT_STDOUT && redir->mode == REDIRECT_OVERWRITE) {
        s = strdup(">|");
    } else if (redir->type == REDIRECT_STDOUT && redir->mode == REDIRECT_APPEND) {
        s = strdup(">>");
    } else if (redir->type == REDIRECT_STDERR && redir->mode == REDIRECT_NO_OVERWRITE) {
        s = strdup("2>");
    } else if (redir->type == REDIRECT_STDERR && redir->mode == REDIRECT_OVERWRITE) {
        s = strdup("2>|");
    } else {
        s = strdup("2>>");
    }

    size_t result_length = strlen(s) + strlen(redir->filename) + 3;
    char *result = malloc(result_length * sizeof(char));
    snprintf(result, result_length, " %s %s", s, redir->filename);
    free(s);
    return result;
}

/**
 * Prints a command, its arguments and options
 */
char *str_of_command(const command *cmd) {
    size_t result_length = 1;

    for (size_t i = 0; i < cmd->argc; ++i) {
        result_length += strlen(cmd->argv[i]);
        if (i < cmd->argc - 1) {
            result_length += 1;
        }
    }

    if (cmd->input_redirection_filename != NULL) {
        result_length += strlen(cmd->input_redirection_filename) + 3;
    }

    for (size_t i = 0; i < cmd->output_redirection_count; ++i) {
        RedirectionType redir_type = (cmd->output_redirections + 1)->type;
        RedirectionMode redir_mode = (cmd->output_redirections + 1)->mode;

        if (redir_type == REDIRECT_STDOUT && redir_mode == REDIRECT_NO_OVERWRITE) {
            result_length += 3 + strlen((cmd->output_redirections + i)->filename);
        } else if (redir_type == REDIRECT_STDERR && redir_mode == REDIRECT_APPEND) {
            result_length += 5 + strlen((cmd->output_redirections + i)->filename);
        } else {
            result_length += 4 + strlen((cmd->output_redirections + i)->filename);
        }
    }

    char *result = malloc(result_length * sizeof(char));
    int marker = 0;

    for (size_t i = 0; i < cmd->argc; ++i) {
        marker += snprintf(result + marker, strlen(cmd->argv[i]) + 1, "%s", cmd->argv[i]);
        if (i < cmd->argc - 1) {
            result[marker] = ' ';
            marker++;
        }
    }

    if (cmd->input_redirection_filename != NULL) {
        marker += snprintf(result + marker, strlen(cmd->input_redirection_filename) + 4, "< %s ",
                           cmd->input_redirection_filename);
    }

    for (size_t i = 0; i < cmd->output_redirection_count; ++i) {
        char *redirection = str_of_output_redirection(cmd->output_redirections + i);
        marker += snprintf(result + marker, strlen(redirection) + 1, "%s", redirection);
        free(redirection);
    }

    result[marker] = '\0';
    return result;
}

char *str_of_pipeline(pipeline *p) {
    int result_length = 1;

    for (size_t i = 0; i < p->command_count; ++i) {
        char *cmd = str_of_command(p->commands[i]);
        result_length += strlen(cmd);
        free(cmd);
        if (i < p->command_count - 1) {
            result_length += 4;
        }
    }

    char *result = malloc(result_length * sizeof(char));
    int marker = 0;

    for (size_t i = 0; i < p->command_count; ++i) {
        char *cmd = str_of_command(p->commands[i]);
        marker += snprintf(result, strlen(cmd) + 1, "%s", cmd);
        free(cmd);
        if (i < p->command_count - 1) {
            result[marker] = ' ';
            result[marker + 1] = '|';
            result[marker + 2] = ' ';
            marker += 3;
        }
    }

    result[marker] = '\0';
    return result;
}

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
    assert(tokens != NULL);
    size_t i = 0;
    for (char *token = strtok(mutable_input, delimiter); token != NULL && i < MAX_TOKENS;
         token = strtok(NULL, delimiter)) {
        size_t token_length = strlen(token);
        tokens[i] = malloc(sizeof(char) * (token_length + 1));
        assert(tokens[i] != NULL);
        strncpy(tokens[i], token, token_length + 1);
        ++i;
    }

    free(mutable_input);

    *token_count = i;

    return tokens;
}

/*
 * Returns 1 if the token is an input redirection symbol, 0 otherwise.
 */
int is_input_redirection(const char *token) {
    return strcmp(token, "<") == 0;
}

/*
 * Returns 1 if the token is an output redirection symbol, 0 otherwise.
 */
int is_output_redirection(const char *token) {
    return strcmp(token, ">") == 0 || strcmp(token, ">|") == 0 || strcmp(token, ">>") == 0 ||
           strcmp(token, "2>") == 0 || strcmp(token, "2>|") == 0 || strcmp(token, "2>>") == 0;
}

void handle_parse_error(char **tokens, size_t token_count, command *cmd) {
    free_tokens(tokens, token_count);
    free_command(cmd);
}

command *finalize_command_reallocation(char **tokens, size_t token_count, command *cmd, size_t output_redirection_count,
                                       output_redirection *output_redirections) {
    // Reajust the size of the argv array
    char **tmp_argv = realloc(cmd->argv, sizeof(char *) * (cmd->argc + 1));
    if (tmp_argv == NULL) {
        handle_parse_error(tokens, token_count, cmd);
        return NULL;
    }
    cmd->argv = tmp_argv;

    // Reajust the size of the output redirections array
    cmd->output_redirection_count = output_redirection_count;
    if (output_redirection_count == 0) {
        free(output_redirections);
        cmd->output_redirections = NULL;
        free_tokens(tokens, token_count);
        return cmd;
    }

    output_redirection *tmp_output_redirection =
        realloc(output_redirections, sizeof(output_redirection) * output_redirection_count);
    if (tmp_output_redirection == NULL) {
        handle_parse_error(tokens, token_count, cmd);
        return NULL;
    }

    cmd->output_redirections = tmp_output_redirection;

    free_tokens(tokens, token_count);
    return cmd;
}

void reinitialize_command(command *cmd) {
    if (cmd->name != NULL) {
        free(cmd->name);
    }
    cmd->name = NULL;
    if (cmd->argv != NULL) {
        for (size_t i = 0; i < cmd->argc; ++i) {
            if (cmd->argv[i] != NULL) {
                free(cmd->argv[i]);
            }
        }
        free(cmd->argv);
    }
    cmd->argv = NULL;
    cmd->argc = 0;
    if (cmd->input_redirection_filename != NULL) {
        free(cmd->input_redirection_filename);
    }
    cmd->input_redirection_filename = NULL;
    for (size_t i = 0; i < cmd->output_redirection_count; ++i) {
        if (cmd->output_redirections[i].filename != NULL) {
            free(cmd->output_redirections[i].filename);
            cmd->output_redirections[i].filename = NULL;
        }
    }
    if (cmd->output_redirections != NULL) {
        free(cmd->output_redirections);
    }
    cmd->output_redirections = NULL;
    cmd->output_redirection_count = 0;
}

command *parse_redirections(char **tokens, size_t token_count, command *cmd) {

    cmd->input_redirection_filename = NULL;
    cmd->output_redirection_count = 0;
    cmd->output_redirections = NULL;

    size_t output_redirection_count = 0;

    output_redirection *output_redirections = malloc(sizeof(output_redirection) * token_count);

    if (output_redirections == NULL) {
        handle_parse_error(tokens, token_count, cmd);
        return NULL;
    }

    size_t i = 0;
    for (i = cmd->argc; i < token_count;) {
        if (is_input_redirection(tokens[i])) {
            if (i + 1 >= token_count) {
                reinitialize_command(cmd);
                fprintf(stderr, "jsh: parse error near `%s'\n", tokens[i]);
                free_tokens(tokens, token_count);
                for (size_t i = 0; i < output_redirection_count; ++i) {
                    if (output_redirections[i].filename != NULL) {
                        free(output_redirections[i].filename);
                    }
                }
                free(output_redirections);
                return cmd;
            }

            if (cmd->input_redirection_filename != NULL) {
                free(cmd->input_redirection_filename);
            }

            if (is_input_redirection(tokens[i + 1]) || is_output_redirection(tokens[i + 1])) {
                reinitialize_command(cmd);
                fprintf(stderr, "jsh: parse error near `%s'\n", tokens[i]);
                free_tokens(tokens, token_count);
                for (size_t i = 0; i < output_redirection_count; ++i) {
                    if (output_redirections[i].filename != NULL) {
                        free(output_redirections[i].filename);
                    }
                }
                free(output_redirections);
                return cmd;
            }

            cmd->input_redirection_filename = strdup(tokens[i + 1]);
            if (cmd->input_redirection_filename == NULL) {
                reinitialize_command(cmd);
                free_tokens(tokens, token_count);
                for (size_t i = 0; i < output_redirection_count; ++i) {
                    if (output_redirections[i].filename != NULL) {
                        free(output_redirections[i].filename);
                    }
                }
                free(output_redirections);
                return cmd;
            }

            i += 2;
        } else if (is_output_redirection(tokens[i])) {
            if (i + 1 >= token_count) {
                reinitialize_command(cmd);
                fprintf(stderr, "jsh: parse error near `%s'\n", tokens[i]);
                free_tokens(tokens, token_count);
                for (size_t i = 0; i < output_redirection_count; ++i) {
                    if (output_redirections[i].filename != NULL) {
                        free(output_redirections[i].filename);
                    }
                }
                free(output_redirections);
                return cmd;
            }

            if (strcmp(tokens[i], ">") == 0) {
                output_redirections[output_redirection_count].type = REDIRECT_STDOUT;
                output_redirections[output_redirection_count].mode = REDIRECT_NO_OVERWRITE;
            } else if (strcmp(tokens[i], ">|") == 0) {
                output_redirections[output_redirection_count].type = REDIRECT_STDOUT;
                output_redirections[output_redirection_count].mode = REDIRECT_OVERWRITE;
            } else if (strcmp(tokens[i], ">>") == 0) {
                output_redirections[output_redirection_count].type = REDIRECT_STDOUT;
                output_redirections[output_redirection_count].mode = REDIRECT_APPEND;
            } else if (strcmp(tokens[i], "2>") == 0) {
                output_redirections[output_redirection_count].type = REDIRECT_STDERR;
                output_redirections[output_redirection_count].mode = REDIRECT_NO_OVERWRITE;
            } else if (strcmp(tokens[i], "2>|") == 0) {
                output_redirections[output_redirection_count].type = REDIRECT_STDERR;
                output_redirections[output_redirection_count].mode = REDIRECT_OVERWRITE;
            } else if (strcmp(tokens[i], "2>>") == 0) {
                output_redirections[output_redirection_count].type = REDIRECT_STDERR;
                output_redirections[output_redirection_count].mode = REDIRECT_APPEND;
            }

            if (is_input_redirection(tokens[i + 1]) || is_output_redirection(tokens[i + 1])) {
                reinitialize_command(cmd);
                fprintf(stderr, "jsh: parse error near `%s'\n", tokens[i]);
                free_tokens(tokens, token_count);
                for (size_t i = 0; i < output_redirection_count; ++i) {
                    if (output_redirections[i].filename != NULL) {
                        free(output_redirections[i].filename);
                    }
                }
                free(output_redirections);
                return cmd;
            }

            output_redirections[output_redirection_count].filename = strdup(tokens[i + 1]);
            if (output_redirections[output_redirection_count].filename == NULL) {
                reinitialize_command(cmd);
                fprintf(stderr, "jsh: parse error near `%s'\n", tokens[i]);
                free_tokens(tokens, token_count);
                for (size_t i = 0; i < output_redirection_count; ++i) {
                    if (output_redirections[i].filename != NULL) {
                        free(output_redirections[i].filename);
                    }
                }
                free(output_redirections);
                return cmd;
            }

            ++output_redirection_count;
            i += 2;
        } else {
            cmd->argv[cmd->argc] = strdup(tokens[i]);
            if (cmd->argv[cmd->argc] == NULL) {
                handle_parse_error(tokens, token_count, cmd);
                for (size_t i = 0; i < output_redirection_count; ++i) {
                    if (output_redirections[i].filename != NULL) {
                        free(output_redirections[i].filename);
                    }
                }
                free(output_redirections);
                return NULL;
            }
            ++cmd->argc;
            ++i;
        }

        cmd->argv[cmd->argc] = NULL;
    }

    if (finalize_command_reallocation(tokens, token_count, cmd, output_redirection_count, output_redirections) ==
        NULL) {
        for (size_t i = 0; i < output_redirection_count; ++i) {
            if (output_redirections[i].filename != NULL) {
                free(output_redirections[i].filename);
            }
        }
        free(output_redirections);
        return NULL;
    }
    return cmd;
}

command *parse_command(const char *input) {

    size_t token_count = 0;
    char **tokens = tokenize(input, &token_count, TOKEN_COMMAND_DELIM);

    assert(tokens != NULL);

    command *cmd = malloc(sizeof(command));

    assert(cmd != NULL);

    if (token_count == 0) { // If Spaces only
        cmd->name = NULL;
        cmd->argc = 0;
        cmd->argv = NULL;
        cmd->input_redirection_filename = NULL;
        cmd->output_redirection_count = 0;
        cmd->output_redirections = NULL;
        free_tokens(tokens, token_count);
        return cmd;
    }

    if (is_input_redirection(tokens[0]) || is_output_redirection(tokens[0])) { // If Spaces only
        fprintf(stderr, "jsh: parse error near `%s'\n", tokens[0]);
        cmd->name = NULL;
        cmd->argc = 0;
        cmd->argv = NULL;
        cmd->input_redirection_filename = NULL;
        cmd->output_redirection_count = 0;
        cmd->output_redirections = NULL;
        free_tokens(tokens, token_count);
        return cmd;
    }

    cmd->name = strdup(tokens[0]);
    cmd->argc = token_count; // Takes into account the command name
    cmd->argv = malloc(sizeof(char *) * (cmd->argc + 1));

    assert(cmd->argv != NULL);

    size_t i = 0;
    for (i = 0; i < cmd->argc; ++i) {
        if (is_input_redirection(tokens[i]) || is_output_redirection(tokens[i])) {
            break;
        }
        cmd->argv[i] = strdup(tokens[i]);
    }

    cmd->argv[i] = NULL;

    cmd->argc = i;

    if (parse_redirections(tokens, token_count, cmd) == NULL) {
        return NULL;
    }

    return cmd;
}

pipeline *parse_pipeline(const char *input, bool to_job, bool can_be_empty) {
    pipeline *pip = malloc(sizeof(pipeline));
    assert(pip != NULL);

    pip->commands = malloc(sizeof(command *));
    assert(pip->commands != NULL);

    pip->commands[0] = parse_command(input);
    pip->command_count = 1;
    if (pip->commands[0] == NULL) {
        free(pip->commands);
        free(pip);
        return NULL;
    }
    if (pip->commands[0]->name == NULL && !can_be_empty) {
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
    assert(tokens != NULL);

    pipeline_list *pips = malloc(sizeof(pipeline_list));
    assert(pips != NULL);

    if (token_count == 0) {
        pips->pipelines = NULL;
        pips->pipeline_count = 0;
        free_tokens(tokens, token_count);

        return pips;
    }
    pips->pipeline_count = 0;
    pips->pipelines = malloc(sizeof(pipeline *) * token_count);
    assert(pips->pipelines != NULL);

    for (size_t i = 0; i < token_count - 1; i++) {
        pips->pipelines[i] = parse_pipeline(tokens[i], true, false);

        if (pips->pipelines[i] == NULL) {
            free_tokens(tokens, token_count);
            free_pipeline_list(pips);
            return NULL;
        }
        pips->pipeline_count += 1;
    }
    bool last_pipeline_to_job = input[strlen(input) - 1] == TOKEN_PIPELINE_DELIM_C;
    pips->pipelines[token_count - 1] =
        parse_pipeline(tokens[token_count - 1], last_pipeline_to_job, !last_pipeline_to_job);
    if (pips->pipelines[token_count - 1] == NULL) {
        free_tokens(tokens, token_count);
        free_pipeline_list(pips);
        return NULL;
    }
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

    cmd->name = NULL;

    size_t i = 0;
    for (i = 0; i < cmd->argc; ++i) {
        if (cmd->argv[i] != NULL)
            free(cmd->argv[i]);
        cmd->argv[i] = NULL;
    }

    if (cmd->argv != NULL)
        free(cmd->argv);
    cmd->argv = NULL;

    if (cmd->input_redirection_filename != NULL)
        free(cmd->input_redirection_filename);
    cmd->input_redirection_filename = NULL;

    for (i = 0; i < cmd->output_redirection_count; ++i) {
        if (cmd->output_redirections[i].filename != NULL)
            free(cmd->output_redirections[i].filename);
        cmd->output_redirections[i].filename = NULL;
    }

    if (cmd->output_redirections != NULL)
        free(cmd->output_redirections);
    cmd->output_redirections = NULL;
    cmd->output_redirection_count = 0;

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
            if (pip->commands[i]->name != NULL) {
                free_command(pip->commands[i]);
            } else {
                free(pip->commands[i]);
            }
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
