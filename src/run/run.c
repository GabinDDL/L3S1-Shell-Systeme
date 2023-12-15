#include "run.h"

int run_command(const command *cmd) {
    if (cmd->name == NULL) {
        return last_command_exit_value;
    } else if (strcmp(cmd->argv[0], "pwd") == 0) {
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
}

int run_pipeline(const pipeline *pip) {
    if (pip == NULL) {
        return FATAL_ERROR;
    }
    int run_output = 0;
    for (size_t i = 0; i < pip->command_count; i++) {
        run_output = run_command(pip->commands[i]);
        if (run_output == FATAL_ERROR) {
            return FATAL_ERROR;
        }
    }
    return run_output;
}

int run_pipeline_list(const pipeline_list *pips) {
    if (pips == NULL) {
        return FATAL_ERROR;
    }
    if (pips->pipeline_count == 0) {
        return last_command_exit_value;
    }
    int run_output = 0;
    for (size_t i = 0; i < pips->pipeline_count; i++) {
        run_output = run_pipeline(pips->pipelines[i]);
        if (run_output == FATAL_ERROR) {
            return FATAL_ERROR;
        }
    }
    return run_output;
}
