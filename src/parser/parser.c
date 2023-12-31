#include "parser.h"
#include "../utils/string_utils.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Returns the string corresponding to a redirection
 * Example :
 *  - " >> a.out"
 */
char *str_of_redirection(redirection *redir) {

    char *s;
    if (redir->type == REDIRECT_STDIN && redir->mode == REDIRECT_NONE) {
        s = strdup("<");
    } else if (redir->type == REDIRECT_STDOUT && redir->mode == REDIRECT_NO_OVERWRITE) {
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
        argument arg = cmd->argv[i];

        if (arg.type == ARG_SIMPLE) {
            result_length += strlen(arg.value.simple);
        } else {
            result_length += strlen(str_of_pipeline(arg.value.substitution));
            result_length += 3;     // For the `<(` and `)` characters
        }

        if (i < cmd->argc - 1) {
            result_length += 1;
        }
    }

    for (size_t i = 0; i < cmd->redirection_count; ++i) {
        RedirectionType redir_type = (cmd->redirections + i)->type;
        RedirectionMode redir_mode = (cmd->redirections + i)->mode;

        if (redir_type == REDIRECT_STDIN && redir_mode == REDIRECT_NONE) {
            result_length += 4 + strlen((cmd->redirections + i)->filename);
        } else if (redir_type == REDIRECT_STDOUT && redir_mode == REDIRECT_NO_OVERWRITE) {
            result_length += 4 + strlen((cmd->redirections + i)->filename);
        } else if (redir_type == REDIRECT_STDERR && redir_mode == REDIRECT_APPEND) {
            result_length += 6 + strlen((cmd->redirections + i)->filename);
        } else {
            result_length += 5 + strlen((cmd->redirections + i)->filename);
        }
    }

    char *result = malloc(result_length * sizeof(char));
    int marker = 0;

    for (size_t i = 0; i < cmd->argc; ++i) {
        argument arg = cmd->argv[i];
        
        if (arg.type == ARG_SIMPLE) {
            marker += snprintf(result + marker, strlen(arg.value.simple) + 1, "%s", arg.value.simple);
        } else {
            char *substitution = str_of_pipeline(arg.value.substitution);
            marker += snprintf(result + marker, 3, "%s", "<(");
            marker += snprintf(result + marker, strlen(substitution) + 1, "%s", substitution);
            marker += snprintf(result + marker, 2, "%s", ")");
            free(substitution);
        }

        if (i < cmd->argc - 1) {
            result[marker] = ' ';
            marker++;
        }
    }

    for (size_t i = 0; i < cmd->redirection_count; ++i) {
        char *redirection = str_of_redirection(cmd->redirections + i);
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
        result_length += strlen(cmd) + 1;
        free(cmd);
        if (i < p->command_count - 1) {
            result_length += 3;
        }
    }

    char *result = malloc(result_length * sizeof(char));
    int marker = 0;

    for (size_t i = 0; i < p->command_count; ++i) {
        char *cmd = str_of_command(p->commands[i]);
        marker += snprintf(result + marker, strlen(cmd) + 1, "%s", cmd);
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

char **tokenize_command_with_special_pipe(const char *input, size_t *token_count) {
    size_t len_input = strlen(input);
    char **tokens = malloc(MAX_TOKENS * sizeof(char *));

    if (len_input == 0) {
        *token_count = 0;
        return tokens;
    }

    size_t start = 0;
    size_t end = 0;
    size_t substitution_depth = 0;
    size_t count = 0;

    while (end < len_input) {
        if (strcmp(input[end], "(") && end > 0 && strcmp(input[end - 1], "<")) {
            substitution_depth++;
        } else if (strcmp(input[end], ")")) {
            substitution_depth--;
        }

        if (substitution_depth < 0) {
            fprintf(stderr, "jsh: parse error near `%c'\n", TOKEN_PIPE_DELIM_C);
            free_tokens(tokens, count);
            return NULL;
        }

        if (strcmp(input[end], " ") && substitution_depth == 0) {
            if (end - start > 0) {
                tokens[count] = malloc(sizeof(char) * (end - start + 1));
                memmove(tokens[count], input + start, end - start);
                tokens[count][end - start] = '\0';
                count++;
            }
            start = end + 1;
            end = start;
        } else {
            end++;
        }
    }

    if (end - start > 0) {
        tokens[count] = malloc(sizeof(char) * (end - start + 1));
        memmove(tokens[count], input + start, end - start);
        tokens[count][end - start] = '\0';
        count++;
    }

    *token_count = count;

    return tokens;
}

char **tokenize_with_sequence(const char *input, size_t *token_count, const char *seq_delimiter) {
    size_t len_input = strlen(input);
    size_t len_seq_delimiter = strlen(seq_delimiter);
    char **tokens = malloc(MAX_TOKENS * sizeof(char *));

    if (len_input == 0) {
        *token_count = 0;
        return tokens;
    }

    size_t count = 0;
    size_t start = 0;
    size_t end = 0;

    while (end < len_input) {
        bool is_pattern = false;
        if (len_seq_delimiter > 0 && end < len_input - len_seq_delimiter + 1) {
            is_pattern = true;
            for (size_t i = 0; i < len_seq_delimiter; i++) {
                if (input[end + i] != seq_delimiter[i]) {
                    is_pattern = false;
                    break;
                }
            }
        }
        if (is_pattern) {
            if (end - start == 0) {
                start += len_seq_delimiter;
                end += len_seq_delimiter;
                continue;
            }
            tokens[count] = malloc(sizeof(char) * (end - start + 1));
            memmove(tokens[count], input + start, end - start);
            tokens[count][end - start] = '\0';
            end += len_seq_delimiter;
            start = end;
            count++;
            continue;
        }
        end++;
    }

    if (end - start != 0) {
        tokens[count] = malloc(sizeof(char) * (end - start + 1));
        memmove(tokens[count], input + start, end - start);
        tokens[count][end - start] = '\0';
        count++;
    }
    *token_count = count;
    return tokens;}

/*
 * Tokenizes the input string into an array of tokens considering '|' as delimiter,
 * except when preceded by '>' or within substitutions marked by '<(' and ')'.
 */
char **tokenize_pipeline_with_special_pipe(const char *input, size_t *token_count) {
    if (!input || !token_count) {
        return NULL;
    }

    size_t len_input = strlen(input);
    char **tokens = malloc(MAX_TOKENS * sizeof(char *));
    if (!tokens) {
        return NULL;
    }

    size_t count = 0;
    size_t start = 0;
    int substitution_depth = 0;
    
    for (size_t i = 0; i < len_input; i++) {
        // Handle substitution depth
        if (input[i] == '<' && (i + 1 < len_input) && input[i + 1] == '(') {
            substitution_depth++;
            i++;
        } else if (input[i] == ')' && substitution_depth > 0) {
            substitution_depth--;
        }

        // Check for pipe delimiter outside substitutions and not preceded by '>'
        bool is_delimiter = (input[i] == '|' && substitution_depth == 0 && (i == 0 || input[i - 1] != '>'));

        if (is_delimiter || i == len_input - 1) {
            size_t token_len = is_delimiter ? i - start : i - start + 1;
            if (token_len > 0) {
                tokens[count] = malloc(token_len + 1);
                if (!tokens[count]) {
                    for (size_t j = 0; j < count; j++) {
                        free(tokens[j]);
                    }
                    free(tokens);
                    return NULL;
                }
                memcpy(tokens[count], input + start, token_len);
                tokens[count][token_len] = '\0';
                count++;
            }
            start = i + 1;
        }

        // Check for token limit
        if (count >= MAX_TOKENS) {
            break;
        }
    }

    *token_count = count;
    return tokens;
}

/*
 * Returns 1 if the token is an redirection symbol, 0 otherwise.
 */
int is_redirection(const char *token) {
    return strcmp(token, "<") == 0 || strcmp(token, ">") == 0 || strcmp(token, ">|") == 0 || strcmp(token, ">>") == 0 ||
           strcmp(token, "2>") == 0 || strcmp(token, "2>|") == 0 || strcmp(token, "2>>") == 0;
}

void handle_parse_error(char **tokens, size_t token_count, command *cmd) {
    free_tokens(tokens, token_count);
    free_command(cmd);
}

command *finalize_command_reallocation(char **tokens, size_t token_count, command *cmd, size_t redirection_count,
                                       redirection *redirections) {
    // Reajust the size of the argv array
    char **tmp_argv = realloc(cmd->argv, sizeof(char *) * (cmd->argc + 1));
    if (tmp_argv == NULL) {
        handle_parse_error(tokens, token_count, cmd);
        return NULL;
    }
    cmd->argv = tmp_argv;

    // Reajust the size of the output redirections array
    cmd->redirection_count = redirection_count;
    if (redirection_count == 0) {
        free(redirections);
        cmd->redirections = NULL;
        free_tokens(tokens, token_count);
        return cmd;
    }

    redirection *tmp_redirection = realloc(redirections, sizeof(redirection) * redirection_count);
    if (tmp_redirection == NULL) {
        handle_parse_error(tokens, token_count, cmd);
        return NULL;
    }

    cmd->redirections = tmp_redirection;

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
            argument arg = cmd->argv[i];
            if (arg.type == ARG_SIMPLE) {
                if (arg.value.simple != NULL) {
                    free(arg.value.simple);
                }
                arg.value.simple = NULL;
            } else {
                if (arg.value.substitution != NULL) {
                    free_pipeline(arg.value.substitution);
                }
                arg.value.substitution = NULL;
            }
        }
        free(cmd->argv);
    }
    cmd->argv = NULL;
    cmd->argc = 0;

    for (size_t i = 0; i < cmd->redirection_count; ++i) {
        if (cmd->redirections[i].filename != NULL) {
            free(cmd->redirections[i].filename);
            cmd->redirections[i].filename = NULL;
        }
    }
    if (cmd->redirections != NULL) {
        free(cmd->redirections);
    }
    cmd->redirections = NULL;
    cmd->redirection_count = 0;
}

command *parse_redirections(char **tokens, size_t token_count, command *cmd) {

    cmd->redirection_count = 0;
    cmd->redirections = NULL;

    size_t redirection_count = 0;

    redirection *redirections = malloc(sizeof(redirection) * token_count);

    if (redirections == NULL) {
        handle_parse_error(tokens, token_count, cmd);
        return NULL;
    }

    size_t i = 0;
    for (i = cmd->argc; i < token_count;) {
        if (is_redirection(tokens[i])) {
            if (i + 1 >= token_count) {
                reinitialize_command(cmd);
                fprintf(stderr, "jsh: parse error near `%s'\n", tokens[i]);
                free_tokens(tokens, token_count);
                for (size_t i = 0; i < redirection_count; ++i) {
                    if (redirections[i].filename != NULL) {
                        free(redirections[i].filename);
                    }
                }
                free(redirections);
                free_command(cmd);
                return NULL;
            }

            if (strcmp(tokens[i], "<") == 0) {
                redirections[redirection_count].type = REDIRECT_STDIN;
                redirections[redirection_count].mode = REDIRECT_NONE;
            } else if (strcmp(tokens[i], ">") == 0) {
                redirections[redirection_count].type = REDIRECT_STDOUT;
                redirections[redirection_count].mode = REDIRECT_NO_OVERWRITE;
            } else if (strcmp(tokens[i], ">|") == 0) {
                redirections[redirection_count].type = REDIRECT_STDOUT;
                redirections[redirection_count].mode = REDIRECT_OVERWRITE;
            } else if (strcmp(tokens[i], ">>") == 0) {
                redirections[redirection_count].type = REDIRECT_STDOUT;
                redirections[redirection_count].mode = REDIRECT_APPEND;
            } else if (strcmp(tokens[i], "2>") == 0) {
                redirections[redirection_count].type = REDIRECT_STDERR;
                redirections[redirection_count].mode = REDIRECT_NO_OVERWRITE;
            } else if (strcmp(tokens[i], "2>|") == 0) {
                redirections[redirection_count].type = REDIRECT_STDERR;
                redirections[redirection_count].mode = REDIRECT_OVERWRITE;
            } else if (strcmp(tokens[i], "2>>") == 0) {
                redirections[redirection_count].type = REDIRECT_STDERR;
                redirections[redirection_count].mode = REDIRECT_APPEND;
            }

            if (is_redirection(tokens[i + 1])) {
                reinitialize_command(cmd);
                fprintf(stderr, "jsh: parse error near `%s'\n", tokens[i]);
                free_tokens(tokens, token_count);
                for (size_t i = 0; i < redirection_count; ++i) {
                    if (redirections[i].filename != NULL) {
                        free(redirections[i].filename);
                    }
                }
                free(redirections);
                free_command(cmd);
                return NULL;
            }

            redirections[redirection_count].filename = strdup(tokens[i + 1]);
            if (redirections[redirection_count].filename == NULL) {
                reinitialize_command(cmd);
                fprintf(stderr, "jsh: parse error near `%s'\n", tokens[i]);
                free_tokens(tokens, token_count);
                for (size_t i = 0; i < redirection_count; ++i) {
                    if (redirections[i].filename != NULL) {
                        free(redirections[i].filename);
                    }
                }
                free(redirections);
                free_command(cmd);
                return NULL;
            }

            ++redirection_count;
            i += 2;
        } else {
            cmd->argv[cmd->argc].type = ARG_SIMPLE;
            cmd->argv[cmd->argc].value.simple = strdup(tokens[i]);
            // TODO: Handle pipe substitution
            if (cmd->argv[cmd->argc].value.simple == NULL) {
                handle_parse_error(tokens, token_count, cmd);
                for (size_t i = 0; i < redirection_count; ++i) {
                    if (redirections[i].filename != NULL) {
                        free(redirections[i].filename);
                    }
                }
                free(redirections);
                return NULL;
            }
            ++cmd->argc;
            ++i;
        }

        cmd->argv[cmd->argc].type = ARG_NULL;
    }

    if (finalize_command_reallocation(tokens, token_count, cmd, redirection_count, redirections) == NULL) {
        for (size_t i = 0; i < redirection_count; ++i) {
            if (redirections[i].filename != NULL) {
                free(redirections[i].filename);
            }
        }
        free(redirections);
        return NULL;
    }
    return cmd;
}

int is_substitution(const char *token) {
    int len = strlen(token);
    return len > 2 && token[0] == '<' && token[1] == '(' && token[len - 1] == ')';
}

command *parse_command(const char *input) {

    size_t token_count = 0;
    char **tokens = tokenize_command_with_special_pipe(input, &token_count);

    assert(tokens != NULL);

    command *cmd = malloc(sizeof(command));

    assert(cmd != NULL);

    if (token_count == 0) { // If Spaces only
        cmd->name = NULL;
        cmd->argc = 0;
        cmd->argv = NULL;
        cmd->redirection_count = 0;
        cmd->redirections = NULL;
        free_tokens(tokens, token_count);
        return cmd;
    }

    if (is_redirection(tokens[0])) { // If Spaces only
        fprintf(stderr, "jsh: parse error near `%s'\n", tokens[0]);
        free_tokens(tokens, token_count);
        free(cmd);
        return NULL;
    }

    cmd->name = strdup(tokens[0]);
    cmd->argc = token_count; // Takes into account the command name
    cmd->argv = malloc(sizeof(char *) * (cmd->argc + 1));

    assert(cmd->argv != NULL);

    size_t i = 0;
    for (i = 0; i < cmd->argc; ++i) {
        if (is_redirection(tokens[i])) {
            break;
        }

        if (is_substitution(tokens[i])) {
            cmd->argv[i].type = ARG_SUBSTITUTION;
            char *substitution = strdup(tokens[i] + 2);
            substitution[strlen(substitution) - 1] = '\0';
            cmd->argv[i].value.substitution = parse_pipeline(substitution, false);
            if (cmd->argv[i].value.substitution == NULL) {
                handle_parse_error(tokens, token_count, cmd);
                return NULL;
            }
        } else {
            cmd->argv[i].type = ARG_SIMPLE;
            cmd->argv[i].value.simple = strdup(tokens[i]);
            if (cmd->argv[i].value.simple == NULL) {
                handle_parse_error(tokens, token_count, cmd);
                return NULL;
            }
        }
    }

    cmd->argv[i].type = ARG_NULL;

    cmd->argc = i;

    if (parse_redirections(tokens, token_count, cmd) == NULL) {
        return NULL;
    }

    return cmd;
}

pipeline *parse_pipeline(const char *input, bool to_job) {
    if (start_with_exception(input, TOKEN_PIPE_DELIM_WITHOUT_SPACE, TOKEN_COMMAND_DELIM_C) ||
        end_with_exception(input, TOKEN_PIPE_DELIM_WITHOUT_SPACE, TOKEN_COMMAND_DELIM_C) ||
        has_sequence_of_with_exception(input, TOKEN_PIPE_DELIM_C, TOKEN_COMMAND_DELIM_C)) {
        fprintf(stderr, "jsh: parse error near `%c'\n", TOKEN_PIPE_DELIM_C);
        return NULL;
    }
    pipeline *pip = malloc(sizeof(pipeline));
    assert(pip != NULL);

    size_t token_count = 0;
    char **tokens = tokenize_pipeline_with_special_pipe(input, &token_count);
    assert(tokens != NULL);

    if (token_count == 0) {
        free_tokens(tokens, token_count);
        pip->command_count = 0;
        pip->to_job = to_job;
        pip->commands = NULL;
        return pip;
    }

    pip->commands = malloc(sizeof(command *) * token_count);
    pip->command_count = 0;
    assert(pip->commands != NULL);

    for (size_t i = 0; i < token_count; i++) {
        pip->commands[i] = parse_command(tokens[i]);

        if (pip->commands[i] == NULL) {
            free_tokens(tokens, token_count);
            free_pipeline(pip);
            return NULL;
        }

        pip->command_count++;

        if (pip->commands[i]->name == NULL && !(token_count == 1)) {
            fprintf(stderr, "jsh: parse error near `%c'\n", TOKEN_PIPE_DELIM_C);
            free_tokens(tokens, token_count);
            free_pipeline(pip);
            return NULL;
        }
    }

    if (token_count == 1 && pip->commands[0]->name == NULL && to_job) {
        fprintf(stderr, "jsh: parse error near `%c'\n", TOKEN_PIPELINE_DELIM_C);
        free_tokens(tokens, token_count);
        free_pipeline(pip);
        return NULL;
    }

    pip->to_job = to_job;

    free_tokens(tokens, token_count);
    return pip;
}

pipeline_list *parse_pipeline_list(const char *input) {
    if (has_sequence_of(input, TOKEN_PIPELINE_DELIM_C) ||
        start_with_exception(input, TOKEN_PIPELINE_DELIM, TOKEN_COMMAND_DELIM_C)) {
        fprintf(stderr, "jsh: parse error near `%c'\n", TOKEN_PIPELINE_DELIM_C);
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
        argument arg = cmd->argv[i];
        if (arg.type == ARG_SIMPLE) {
            if (arg.value.simple != NULL)
                free(arg.value.simple);
            arg.value.simple = NULL;
        } else {
            if (arg.value.substitution != NULL)
                free_pipeline(arg.value.substitution);
            arg.value.substitution = NULL;
        }
    }

    if (cmd->argv != NULL)
        free(cmd->argv);
    cmd->argv = NULL;

    for (i = 0; i < cmd->redirection_count; ++i) {
        if (cmd->redirections[i].filename != NULL)
            free(cmd->redirections[i].filename);
        cmd->redirections[i].filename = NULL;
    }

    if (cmd->redirections != NULL)
        free(cmd->redirections);
    cmd->redirections = NULL;
    cmd->redirection_count = 0;

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
