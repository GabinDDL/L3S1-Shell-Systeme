#include <assert.h>

#include "test_parser.h"

void test_parse_command_no_arguments();
void test_parse_command_two_arguments();
void test_parse_command_with_empty_input();
void test_parse_command_with_only_spaces();
void test_parse_command_with_spaces_between();
void test_parse_command_with_spaces_before();

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
}

void test_parse_command_no_arguments() {
    char *input = "ls";

    // Call the function to test
    command *cmd = parse_command(input);

    // Check if the command name is correct
    assert(strcmp(cmd->name, "ls") == 0);

    // Check the correct number of arguments
    assert(cmd->argc == 0);

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
    assert(cmd->argc == 2);

    // Check if the arguments are correct
    assert(strcmp(cmd->argv[0], "-l") == 0 && strcmp(cmd->argv[1], "/home") == 0);

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
    assert(cmd->argc == 2);

    // Check if the arguments are correct
    assert(strcmp(cmd->argv[0], "-l") == 0 && strcmp(cmd->argv[1], "/home") == 0);

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
    assert(cmd->argc == 2);

    // Check if the arguments are correct
    assert(strcmp(cmd->argv[0], "-l") == 0 && strcmp(cmd->argv[1], "/home") == 0);

    // Clean up
    free_command(cmd);
}