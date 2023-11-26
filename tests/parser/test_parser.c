#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "test_parser.h"

void test_parse_command_no_arguments();
void test_parse_command_two_arguments();
void test_parse_command_with_empty_input();
void test_parse_command_with_only_spaces();
void test_parse_command_with_spaces_between();
void test_parse_command_with_spaces_before();
void test_parse_command_with_more_than_max_tokens();

void test_parser_utils() {

    printf("Test function test_parse_command_no_arguments\n");
    test_parse_command_no_arguments();
    printf("Test test_parse_command_no_arguments passed\n");

    printf("Test function test_parse_command_two_arguments\n");
    test_parse_command_two_arguments();
    printf("Test test_parse_command_two_arguments passed\n");

    printf("Test function parse_command_with_empty_input\n");
    test_parse_command_with_empty_input();
    printf("Test parse_command_with_empty_input passed\n");

    printf("Test function parse_command_with_only_spaces\n");
    test_parse_command_with_only_spaces();
    printf("Test parse_command_with_only_spaces passed\n");

    printf("Test function parse_command_with_spaces_between\n");
    test_parse_command_with_spaces_between();
    printf("Test parse_command_with_spaces_between passed\n");

    printf("Test function parse_command_with_spaces_before\n");
    test_parse_command_with_spaces_before();
    printf("Test parse_command_with_spaces_before passed\n");

    printf("Test function parse_command_with_more_than_max_tokens\n");
    test_parse_command_with_more_than_max_tokens();
    printf("Test parse_command_with_more_than_max_tokens passed\n");
}

void test_parse_command_no_arguments() {
    char *input = "ls";

    // Call the function to test
    command *cmd = parse_command(input);

    // Check if the command name is correct
    assert(strcmp(cmd->name, "ls") == 0);

    // Check the correct number of arguments
    assert(cmd->argc == 1);

    // Check if the arguments are correct
    assert(strcmp(cmd->argv[0], "ls") == 0);
    assert(cmd->argv[1] == NULL);

    // Clean up
    free_command(cmd);
}

void test_parse_command_two_arguments() {
    char *input = "ls -l /home";

    // Call the function to test
    command *cmd = parse_command(input);

    // Check if the command name is correct
    assert(strcmp(cmd->name, "ls") == 0);

    // Check the correct number of arguments
    assert(cmd->argc == 3);

    // Check if the arguments are correct
    assert(strcmp(cmd->argv[0], "ls") == 0);
    assert(strcmp(cmd->argv[1], "-l") == 0);
    assert(strcmp(cmd->argv[2], "/home") == 0);
    assert(cmd->argv[3] == NULL);

    // Clean up
    free_command(cmd);
}

void test_parse_command_with_empty_input() {
    char *input = "";

    // Call the function to test
    command *cmd = parse_command(input);

    // Check if the command is NULL (invalid input)
    assert(cmd == NULL);
}

void test_parse_command_with_only_spaces() {
    char *input = "     ";

    // Call the function to test
    command *cmd = parse_command(input);

    // Check if the command is NULL (invalid input)
    assert(cmd == NULL);
}

void test_parse_command_with_spaces_between() {
    char *input = "ls     -l     /home";

    // Call the function to test
    command *cmd = parse_command(input);

    // Check if the command name is correct
    assert(strcmp(cmd->name, "ls") == 0);

    // Check the correct number of arguments
    assert(cmd->argc == 3);

    // Check if the arguments are correct
    assert(strcmp(cmd->argv[0], "ls") == 0);
    assert(strcmp(cmd->argv[1], "-l") == 0);
    assert(strcmp(cmd->argv[2], "/home") == 0);
    assert(cmd->argv[3] == NULL);

    // Clean up
    free_command(cmd);
}

void test_parse_command_with_spaces_before() {
    char *input = "     ls -l /home";

    // Call the function to test
    command *cmd = parse_command(input);

    // Check if the command name is correct
    assert(strcmp(cmd->name, "ls") == 0);

    // Check the correct number of arguments
    assert(cmd->argc == 3);

    // Check if the arguments are correct
    assert(strcmp(cmd->argv[0], "ls") == 0);
    assert(strcmp(cmd->argv[1], "-l") == 0);
    assert(strcmp(cmd->argv[2], "/home") == 0);
    assert(cmd->argv[3] == NULL);

    // Clean up
    free_command(cmd);
}

void test_parse_command_with_more_than_max_tokens() {
    // Create a string with more than MAX_TOKENS tokens dynamically
    size_t token_count = 2 * MAX_TOKENS;
    char *input = malloc(sizeof(char) * (token_count * 2 + 1));
    if (input == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        exit(EXIT_FAILURE);
    }

    // Fill the input with "a " token_count times
    size_t i = 0;
    for (i = 0; i < token_count; ++i) {
        input[i * 2] = 'a';
        input[i * 2 + 1] = ' ';
    }
    input[token_count * 2] = '\0';

    // Call the function to test
    command *cmd = parse_command(input);

    // Check if the command name is correct
    assert(strcmp(cmd->name, "a") == 0);

    // Check the correct number of arguments
    assert(cmd->argc == MAX_TOKENS);

    // Check if the arguments are correct
    for (i = 0; i < MAX_TOKENS; ++i) {
        assert(strcmp(cmd->argv[i], "a") == 0);
    }

    assert(cmd->argv[MAX_TOKENS] == NULL);

    // Clean up
    free_command(cmd);
    free(input);
}