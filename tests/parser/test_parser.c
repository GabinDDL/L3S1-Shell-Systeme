#include "test_parser.h"

int test_parse_command() {
    char* input = "ls -l /home";
    int test_failed = 0;

    // Call the function to test
    command *cmd = parse_command(input);

    // Check if the command name is correct
    if (strcmp(cmd->name, "ls") != 0) {
        printf("Test failed: Command name is incorrect.\n");
        test_failed = 1;
    }

    // Check the correct number of arguments
    if (cmd->argc != 2) {
        printf("Test failed: Incorrect number of arguments.\n");
        test_failed = 1;
    }

    // Check if the arguments are correct
    if (strcmp(cmd->argv[0], "-l") != 0 || strcmp(cmd->argv[1], "/home") != 0) {
        printf("Test failed: Arguments are incorrect.\n");
        test_failed = 1;
    }

    // Clean up
    for (int i = 0; i < cmd->argc; i++) {
        free(cmd->argv[i]);
    }
    free(cmd->argv);
    free(cmd->name);
    free(cmd);

    if (!test_failed) {
        printf("parse_command test passed.\n");
    }

    return test_failed;
}