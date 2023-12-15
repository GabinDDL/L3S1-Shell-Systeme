#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_parser.h"

void test_parse_command_no_arguments();
void test_parse_command_two_arguments();
void test_parse_command_with_empty_input();
void test_parse_command_with_only_spaces();
void test_parse_command_with_spaces_between();
void test_parse_command_with_spaces_before();
void test_parse_command_with_more_than_max_tokens();
void test_parse_pipeline_list_with_empty_input();
void test_parse_pipeline_list_with_only_spaces();
void test_parse_pipeline_list_with_only_spaces_and_ampersand();
void test_parse_pipeline_list_with_start_ampersand();
void test_parse_pipeline_list_with_middle_ampersands();
void test_parse_pipeline_list_with_only_ampersand();
void test_parse_pipeline_list_with_middle_ampersands_and_spaces();
void test_parse_pipeline_list_with_single_pipeline_no_job();
void test_parse_pipeline_list_with_single_pipeline_with_job();
void test_parse_pipeline_list_with_correct_pipelines();
void test_parse_pipeline_list_with_correct_pipelines_with_final_ampersand();
void test_parse_pipeline_list_with_correct_pipelines_with_spaces_and_final_ampersand();

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

    printf("Test function test_parse_pipeline_list_with_empty_input\n");
    test_parse_pipeline_list_with_empty_input();
    printf("Test test_parse_pipeline_list_with_empty_input passed\n");

    printf("Test function test_parse_pipeline_list_with_only_spaces\n");
    test_parse_pipeline_list_with_only_spaces();
    printf("Test test_parse_pipeline_list_with_only_spaces passed\n");

    printf("Test function test_parse_pipeline_list_with_only_ampersand\n");
    test_parse_pipeline_list_with_only_ampersand();
    printf("Test test_parse_pipeline_list_with_only_ampersand passed\n");

    printf("Test function test_parse_pipeline_list_with_only_spaces_and_ampersand\n");
    test_parse_pipeline_list_with_only_spaces_and_ampersand();
    printf("Test test_parse_pipeline_list_with_only_spaces_and_ampersand passed\n");

    printf("Test function test_parse_pipeline_list_with_start_ampersand\n");
    test_parse_pipeline_list_with_start_ampersand();
    printf("Test test_parse_pipeline_list_with_start_ampersand passed\n");

    printf("Test function test_parse_pipeline_list_with_middle_ampersands\n");
    test_parse_pipeline_list_with_middle_ampersands();
    printf("Test test_parse_pipeline_list_with_middle_ampersands passed\n");

    printf("Test function test_parse_pipeline_list_with_middle_ampersands_and_spaces\n");
    test_parse_pipeline_list_with_middle_ampersands_and_spaces();
    printf("Test test_parse_pipeline_list_with_middle_ampersands_and_spaces passed\n");

    printf("Test function test_parse_pipeline_list_with_single_pipeline_with_job\n");
    test_parse_pipeline_list_with_single_pipeline_with_job();
    printf("Test test_parse_pipeline_list_with_single_pipeline_with_job passed\n");

    printf("Test function test_parse_pipeline_list_with_single_pipeline_no_job\n");
    test_parse_pipeline_list_with_single_pipeline_no_job();
    printf("Test test_parse_pipeline_list_with_single_pipeline_no_job passed\n");

    printf("Test function test_parse_pipeline_list_with_correct_pipelines\n");
    test_parse_pipeline_list_with_correct_pipelines();
    printf("Test test_parse_pipeline_list_with_correct_pipelines passed\n");

    printf("Test function test_parse_pipeline_list_with_correct_pipelines_with_final_ampersand\n");
    test_parse_pipeline_list_with_correct_pipelines_with_final_ampersand();
    printf("Test test_parse_pipeline_list_with_correct_pipelines_with_final_ampersand passed\n");

    printf("Test function test_parse_pipeline_list_with_correct_pipelines_with_spaces_and_final_ampersand\n");
    test_parse_pipeline_list_with_correct_pipelines_with_spaces_and_final_ampersand();
    printf("Test test_parse_pipeline_list_with_correct_pipelines_with_spaces_and_final_ampersand passed\n");
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

    // Check if the command name is NULL
    assert(cmd->name == NULL);

    // Check the correct number of arguments
    assert(cmd->argc == 0);

    // Check if the arguments are NULL
    assert(cmd->argv == NULL);

    // Clean up
    free_command(cmd);
}

void test_parse_command_with_only_spaces() {
    char *input = "     ";

    // Call the function to test
    command *cmd = parse_command(input);

    // Check if the command name is NULL
    assert(cmd->name == NULL);

    // Check the correct number of arguments
    assert(cmd->argc == 0);

    // Check if the arguments are NULL
    assert(cmd->argv == NULL);

    // Clean up
    free_command(cmd);
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

void test_parse_pipeline_list_with_empty_input() {
    char *input = "";

    // Call the function to test
    pipeline_list *pips = parse_pipeline_list(input);

    // Check if the pipelines of pipeline list is NULL
    assert(pips->pipelines == NULL);

    // Check the correct number of pipelines
    assert(pips->pipeline_count == 0);

    // Clean up
    free_pipeline_list(pips);
}

void test_parse_pipeline_list_with_only_spaces() {
    char *input = "     ";

    // Call the function to test
    pipeline_list *pips = parse_pipeline_list(input);

    // Check if the pipelines of the list is not NULL
    assert(pips->pipelines != NULL);

    // Check the correct number of pipelines
    assert(pips->pipeline_count == 1);

    // Check if the pipeline are NULL
    assert(pips->pipelines[0] == NULL);

    // Clean up
    free_pipeline_list(pips);
}

void test_parse_pipeline_list_with_only_ampersand() {
    char *input = "&";

    // Call the function to test
    pipeline_list *pips = parse_pipeline_list(input);

    // Check if the pipeline_list name is NULL
    assert(pips == NULL);
}

void test_parse_pipeline_list_with_only_spaces_and_ampersand() {
    char *input = "   &    ";

    // Call the function to test
    pipeline_list *pips = parse_pipeline_list(input);

    // Check if the pipeline_list name is NULL
    assert(pips == NULL);
}

void test_parse_pipeline_list_with_start_ampersand() {
    char *input = " & ls";

    // Call the function to test
    pipeline_list *pips = parse_pipeline_list(input);

    // Check if the pipeline_list name is NULL
    assert(pips == NULL);
}

void test_parse_pipeline_list_with_middle_ampersands() {
    char *input = "ls && ./test";

    // Call the function to test
    pipeline_list *pips = parse_pipeline_list(input);

    // Check if the pipeline_list is NULL
    assert(pips == NULL);
}

void test_parse_pipeline_list_with_middle_ampersands_and_spaces() {
    char *input = "ls &    & ./test";

    // Call the function to test
    pipeline_list *pips = parse_pipeline_list(input);

    // Check if the pipeline_list is NULL
    assert(pips == NULL);
}

void test_parse_pipeline_list_with_single_pipeline_no_job() {
    char *input = "ls -l";

    // Call the function to test
    pipeline_list *pips = parse_pipeline_list(input);

    // Check if the pipelines of the list is not NULL
    assert(pips->pipelines != NULL);

    // Check the correct number of pipelines
    assert(pips->pipeline_count == 1);

    // Call the pipeline to test
    pipeline *pip1 = pips->pipelines[0];

    // Check if the pipeline has not to_job
    assert(!pip1->to_job);

    // Check the correct number of command in pipeline
    assert(pip1->command_count == 1);

    // Call the command to test
    command *cmd1 = pip1->commands[0];

    // Check the correct command name
    assert(strcmp(cmd1->name, "ls") == 0);

    // Check the correct number of args of command
    assert(cmd1->argc == 2);

    // Cleanup
    free_pipeline_list(pips);
}

void test_parse_pipeline_list_with_single_pipeline_with_job() {
    char *input = "ls -l &";

    // Call the function to test
    pipeline_list *pips = parse_pipeline_list(input);

    // Check if the pipelines of the list is not NULL
    assert(pips->pipelines != NULL);

    // Check the correct number of pipelines
    assert(pips->pipeline_count == 1);

    // Call the pipeline to test
    pipeline *pip1 = pips->pipelines[0];

    // Check if the pipeline has to_job
    assert(pip1->to_job);

    // Check the correct number of command in pipeline
    assert(pip1->command_count == 1);

    // Call the command to test
    command *cmd1 = pip1->commands[0];

    // Check the correct command name
    assert(strcmp(cmd1->name, "ls") == 0);

    // Check the correct number of args of command
    assert(cmd1->argc == 2);

    // Cleanup
    free_pipeline_list(pips);
}

void test_parse_pipeline_list_with_correct_pipelines() {
    char *input = "ls & ./test & ./a.out";

    // Call the function to test
    pipeline_list *pips = parse_pipeline_list(input);

    // Check if the pipelines of the list is not NULL
    assert(pips->pipelines != NULL);

    // Check the correct number of pipelines
    assert(pips->pipeline_count == 3);

    // Call the pipelines to test
    pipeline *pip1 = pips->pipelines[0];
    pipeline *pip2 = pips->pipelines[1];
    pipeline *pip3 = pips->pipelines[2];

    // Check if the pipelines have to_job or not
    assert(pip1->to_job);
    assert(pip2->to_job);
    assert(!pip3->to_job);

    // Check the correct number of command in pipelines
    assert(pip1->command_count == 1);
    assert(pip2->command_count == 1);
    assert(pip3->command_count == 1);

    // Call the commands to test
    command *cmd1 = pip1->commands[0];
    command *cmd2 = pip2->commands[0];
    command *cmd3 = pip3->commands[0];

    // Check the correct commands name
    assert(strcmp(cmd1->name, "ls") == 0);
    assert(strcmp(cmd2->name, "./test") == 0);
    assert(strcmp(cmd3->name, "./a.out") == 0);

    // Check the correct number of args of commands
    assert(cmd1->argc == 1);
    assert(cmd2->argc == 1);
    assert(cmd3->argc == 1);

    // Cleanup
    free_pipeline_list(pips);
}

void test_parse_pipeline_list_with_correct_pipelines_with_final_ampersand() {
    char *input = "./test t a & ls -l & ./a.out & mkdir t &";

    // Call the function to test
    pipeline_list *pips = parse_pipeline_list(input);

    // Check if the pipelines of the list is not NULL
    assert(pips->pipelines != NULL);

    // Check the correct number of pipelines
    assert(pips->pipeline_count == 4);

    // Call the pipelines to test
    pipeline *pip1 = pips->pipelines[0];
    pipeline *pip2 = pips->pipelines[1];
    pipeline *pip3 = pips->pipelines[2];
    pipeline *pip4 = pips->pipelines[3];

    // Check if the pipelines have to_job
    assert(pip1->to_job);
    assert(pip2->to_job);
    assert(pip3->to_job);
    assert(pip4->to_job);

    // Check the correct number of command in pipelines
    assert(pip1->command_count == 1);
    assert(pip2->command_count == 1);
    assert(pip3->command_count == 1);
    assert(pip4->command_count == 1);

    // Call the commands to test
    command *cmd1 = pip1->commands[0];
    command *cmd2 = pip2->commands[0];
    command *cmd3 = pip3->commands[0];
    command *cmd4 = pip4->commands[0];

    // Check the correct commands name
    assert(strcmp(cmd1->name, "./test") == 0);
    assert(strcmp(cmd2->name, "ls") == 0);
    assert(strcmp(cmd3->name, "./a.out") == 0);
    assert(strcmp(cmd4->name, "mkdir") == 0);

    // Check the correct number of args of commands
    assert(cmd1->argc == 3);
    assert(cmd2->argc == 2);
    assert(cmd3->argc == 1);
    assert(cmd4->argc == 2);

    // Cleanup
    free_pipeline_list(pips);
}

void test_parse_pipeline_list_with_correct_pipelines_with_spaces_and_final_ampersand() {
    char *input = "  ./test   t  a  &    ls    -l     & ./a.out&mkdir          t    &           ";

    // Call the function to test
    pipeline_list *pips = parse_pipeline_list(input);

    // Check if the pipelines of the list is not NULL
    assert(pips->pipelines != NULL);

    // Check the correct number of pipelines
    assert(pips->pipeline_count == 5);

    // Call the pipelines to test
    pipeline *pip1 = pips->pipelines[0];
    pipeline *pip2 = pips->pipelines[1];
    pipeline *pip3 = pips->pipelines[2];
    pipeline *pip4 = pips->pipelines[3];
    pipeline *pip5 = pips->pipelines[4];

    // Check if the pipelines have to_job
    assert(pip1->to_job);
    assert(pip2->to_job);
    assert(pip3->to_job);
    assert(pip4->to_job);
    assert(pip5 == NULL);

    // Check the correct number of command in pipelines
    assert(pip1->command_count == 1);
    assert(pip2->command_count == 1);
    assert(pip3->command_count == 1);
    assert(pip4->command_count == 1);

    // Call the commands to test
    command *cmd1 = pip1->commands[0];
    command *cmd2 = pip2->commands[0];
    command *cmd3 = pip3->commands[0];
    command *cmd4 = pip4->commands[0];

    // Check the correct commands name
    assert(strcmp(cmd1->name, "./test") == 0);
    assert(strcmp(cmd2->name, "ls") == 0);
    assert(strcmp(cmd3->name, "./a.out") == 0);
    assert(strcmp(cmd4->name, "mkdir") == 0);

    // Check the correct number of args of commands
    assert(cmd1->argc == 3);
    assert(cmd2->argc == 2);
    assert(cmd3->argc == 1);
    assert(cmd4->argc == 2);

    // Cleanup
    free_pipeline_list(pips);
}
