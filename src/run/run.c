#include "run.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int run_command_without_redirections(command *cmd, bool is_job, pipeline *pip) {

    int return_value = 0;

    if (cmd->name == NULL) {
        return_value = last_command_exit_value;
    } else if (strcmp(cmd->argv[0], "pwd") == 0) {
        return_value = pwd(cmd);
    } else if (strcmp(cmd->argv[0], "cd") == 0) {
        int cd_output = cd(cmd);
        update_prompt();
        return_value = cd_output;
    } else if (strcmp(cmd->argv[0], "exit") == 0) {
        return_value = exit_jsh(cmd);
    } else if (strcmp(cmd->argv[0], "?") == 0) {
        return_value = print_last_command_result(cmd);
    } else if (strcmp(cmd->argv[0], "jobs") == 0) {
        return_value = print_jobs(cmd);
    } else if (strcmp(cmd->argv[0], "kill") == 0) {
        return_value = jsh_kill(cmd);
    } else {
        if (is_job) {
            return_value = extern_command(cmd);
        } else {
            int status; // status of the created process
            pid_t pid = fork();

            assert(pid != -1);

            switch (pid) {
            case 0:
                return_value = extern_command(cmd);
                if (return_value < 0) {
                    perror("execvp");
                    exit(errno);
                }
                exit(SUCCESS);
                break;
            default:
                setpgid(pid, pid);
                waitpid(pid, &status, WUNTRACED);
                if (WIFSTOPPED(status)) {
                    pip->to_job = true;
                    add_new_forked_process_to_jobs(pid, pip, STOPPED);
                }
                return WEXITSTATUS(status);
            }
        }
    }
    return return_value;
}

int get_flags(const redirection *redirection) {

    if (redirection->type == REDIRECT_STDIN) {
        return O_RDONLY;
    }

    // STDOUT or STDERR
    int flags = O_WRONLY | O_CREAT;
    if (redirection->mode == REDIRECT_APPEND) {
        flags |= O_APPEND;
    } else if (redirection->mode == REDIRECT_NO_OVERWRITE) {
        flags |= O_EXCL;
    } else {
        flags |= O_TRUNC;
    }
    return flags;
}

int run_command(command *cmd, bool is_job, pipeline *pip) {

    int return_value = 0;

    int stdin_copy = dup(STDIN_FILENO);
    int stdout_copy = dup(STDOUT_FILENO);
    int stderr_copy = dup(STDERR_FILENO);

    int fd_in, fd_out, fd_err;
    for (size_t i = 0; i < cmd->redirection_count; ++i) {
        if (cmd->redirections[i].type == REDIRECT_STDIN) {
            fd_in = open(cmd->redirections[i].filename, O_RDONLY);
            if (fd_in == -1) {
                if (errno == ENOENT) {
                    perror("open");
                    return COMMAND_FAILURE;
                }
            }
            if (dup2(fd_in, STDIN_FILENO) == -1) {
                perror("dup2");
                return EXIT_FAILURE;
            }
            if (close(fd_in) == -1) {
                perror("close");
                return EXIT_FAILURE;
            }
        } else if (cmd->redirections[i].type == REDIRECT_STDOUT) {
            fd_out = open(cmd->redirections[i].filename, get_flags(&cmd->redirections[i]), 0666);
            if (fd_out == -1) {
                if (errno == EEXIST) {
                    fprintf(stderr, "jsh: %s: cannot overwrite existing file\n", cmd->redirections[i].filename);
                    return 1;
                } else {
                    perror("open");
                    return EXIT_FAILURE;
                }
            }
            if (dup2(fd_out, STDOUT_FILENO) == -1) {
                perror("dup2");
                return EXIT_FAILURE;
            }
            if (close(fd_out) == -1) {
                perror("close");
                return EXIT_FAILURE;
            }
        } else if (cmd->redirections[i].type == REDIRECT_STDERR) {
            fd_err = open(cmd->redirections[i].filename, get_flags(&cmd->redirections[i]), 0666);
            if (fd_err == -1) {
                if (errno == EEXIST) {
                    fprintf(stderr, "jsh: %s: cannot overwrite existing file\n", cmd->redirections[i].filename);
                    return 1;
                } else {
                    perror("open");
                    return EXIT_FAILURE;
                }
            }
            if (dup2(fd_err, STDERR_FILENO) == -1) {
                perror("dup2");
                return EXIT_FAILURE;
            }
            if (close(fd_err) == -1) {
                perror("close");
                return EXIT_FAILURE;
            }
        }
    }

    return_value = run_command_without_redirections(cmd, is_job, pip);

    dup2(stdin_copy, STDIN_FILENO);
    dup2(stdout_copy, STDOUT_FILENO);
    dup2(stderr_copy, STDERR_FILENO);

    close(stdin_copy);
    close(stdout_copy);
    close(stderr_copy);

    return return_value;
}

int run_pipeline(pipeline *pip) {
    assert(pip != NULL);
    int run_output = 0;
    unsigned njob = job_number;
    for (size_t i = 0; i < pip->command_count; i++) {

        // TODO: do the case of few commands (pipe)
        if (pip->to_job) {
            pid_t pid = fork();
            if (pid == -1) {
                print_error("fork: error to create a process");
            }
            if (pid == 0) {
                run_output = run_command(pip->commands[i], true, pip);
                exit(run_output);
            } else {
                setpgid(pid, pid);
                run_output = add_new_forked_process_to_jobs(pid, pip, RUNNING);
            }
        } else {
            run_output = run_command(pip->commands[i], false, pip);
        }
    }
    if (njob != job_number) {
        update_prompt();
    }
    return run_output;
}

int run_pipeline_list(pipeline_list *pips) {
    assert(pips != NULL);

    if (pips->pipeline_count == 0) {
        return last_command_exit_value;
    }
    int run_output = 0;
    for (size_t i = 0; i < pips->pipeline_count; i++) {
        run_output = run_pipeline(pips->pipelines[i]);
    }
    return run_output;
}
