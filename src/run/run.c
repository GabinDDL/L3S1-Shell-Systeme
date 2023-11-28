#include "run.h"

int run_command(const command *cmd) {

    if (strcmp(cmd->argv[0], "pwd") == 0) {
        return pwd(cmd);
    } else if (strcmp(cmd->argv[0], "cd") == 0) {
        int cd_output = cd(cmd);
        update_prompt();
        return cd_output;
    } else if (strcmp(cmd->argv[0], "exit") == 0) {
        return exit_jsh(cmd);
    } else if (strcmp(cmd->argv[0], "?") == 0) {
        return print_last_command_result(cmd);
    } else {
        return extern_command(cmd);
    }

    return SUCCESS;
}