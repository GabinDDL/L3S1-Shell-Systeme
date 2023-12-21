#include "run.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

int max(int a, int b) {
    return (a > b) ? a : b;
}

int run_command_without_redirections(command *cmd, bool is_job) {

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
        return_value = jobs_command();
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
                waitpid(pid, &status, 0);
                assert(WIFEXITED(status));
                return WEXITSTATUS(status);
            }
        }
    }
    return return_value;
}

int handle_no_redirections(command *cmd, bool is_job) {
    return run_command_without_redirections(cmd, is_job);
}

int handle_input_redirection(command *cmd, int *stdin_copy) {
    int fd = open(cmd->input_redirection_filename, O_RDONLY);
    if (fd == -1) {
        if (errno == ENOENT) {
            perror("open");
            return COMMAND_FAILURE;
        }
    }
    *stdin_copy = dup(STDIN_FILENO);
    if (dup2(fd, STDIN_FILENO) == -1) {
        perror("dup2");
        return EXIT_FAILURE;
    }

    if (close(fd) == -1) {
        perror("close");
        return EXIT_FAILURE;
    }

    return SUCCESS;
}

int get_output_flags(const output_redirection *output_redirection) {

    int output_flags = O_WRONLY | O_CREAT;
    if (output_redirection->mode == REDIRECT_APPEND) {
        output_flags |= O_APPEND;
    } else if (output_redirection->mode == REDIRECT_NO_OVERWRITE) {
        output_flags |= O_EXCL;
    } else {
        output_flags |= O_TRUNC;
    }
    return output_flags;
}

int run_command_with_redirection(command *cmd, int *stdout_pipe, int *stderr_pipe, bool is_job) {
    int return_value = 0;
    int stdout_copy = dup(STDOUT_FILENO);
    int stderr_copy = dup(STDERR_FILENO);
    dup2(stdout_pipe[1], STDOUT_FILENO);
    dup2(stderr_pipe[1], STDERR_FILENO);
    close(stdout_pipe[1]);
    close(stderr_pipe[1]);
    return_value = run_command_without_redirections(cmd, is_job);
    dup2(stdout_copy, STDOUT_FILENO);
    dup2(stderr_copy, STDERR_FILENO);
    close(stdout_copy);
    close(stderr_copy);
    return return_value;
}

void monitor_and_handle_pipes(command *cmd, int *stdout_pipe, int *stderr_pipe, int *fd_list) {
    char stdout_buf[32];
    char stderr_buf[32];

    fd_set read_fds;
    struct timeval tv;
    int max_fd = max(stdout_pipe[0], stderr_pipe[0]);
    int stdout_closed = 0, stderr_closed = 0;

    while (!stdout_closed || !stderr_closed) {
        FD_ZERO(&read_fds);
        if (!stdout_closed) {
            FD_SET(stdout_pipe[0], &read_fds);
        }
        if (!stderr_closed) {
            FD_SET(stderr_pipe[0], &read_fds);
        }

        tv.tv_sec = 1; // 1-second timeout
        tv.tv_usec = 0;

        int activity = select(max_fd + 1, &read_fds, NULL, NULL, &tv);

        if ((activity < 0) && (errno != EINTR)) {
            perror("select error");
            exit(1);
        }

        if (!stdout_closed && FD_ISSET(stdout_pipe[0], &read_fds)) {
            int read_stdout_bytes = read(stdout_pipe[0], stdout_buf, 32);

            if (read_stdout_bytes > 0) { // if stdout is not empty after command execution
                int ran = 0;
                for (int i = 0; i < cmd->output_redirection_count; ++i) {
                    if (cmd->output_redirections[i].type == REDIRECT_STDOUT) {
                        ran = 1;
                        if (fd_list[i] == -1) {
                            continue;
                        }

                        if (write(fd_list[i], stdout_buf, read_stdout_bytes) == -1) {
                            for (int j = 0; j < cmd->output_redirection_count; ++j) {
                                if (fd_list[j] == -1) {
                                    continue;
                                }
                                if (close(fd_list[j]) == -1) {
                                    perror("close");
                                    exit(1);
                                }
                            }
                            free(fd_list);
                            perror("write");
                            exit(1);
                        }
                    }
                }
                if (!ran) {
                    if (write(STDOUT_FILENO, stdout_buf, read_stdout_bytes) == -1) {
                        perror("write");
                        exit(1);
                    }
                }
            } else if (read_stdout_bytes == 0) {
                stdout_closed = 1;
            } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
                perror("read stdout");
                exit(1);
            }
        }

        if (!stderr_closed && FD_ISSET(stderr_pipe[0], &read_fds)) {
            int read_stderr_bytes = read(stderr_pipe[0], stderr_buf, 32);

            if (read_stderr_bytes > 0) { // if stderr is not empty after command execution
                int ran = 0;
                for (int i = 0; i < cmd->output_redirection_count; ++i) {
                    if (cmd->output_redirections[i].type == REDIRECT_STDERR) {
                        ran = 1;
                        if (fd_list[i] == -1) {
                            continue;
                        }

                        if (write(fd_list[i], stderr_buf, read_stderr_bytes) == -1) {
                            for (int j = 0; j < cmd->output_redirection_count; ++j) {
                                if (fd_list[j] == -1) {
                                    continue;
                                }
                                if (close(fd_list[j]) == -1) {
                                    perror("close");
                                    exit(1);
                                }
                            }
                            free(fd_list);
                            perror("write");
                            exit(1);
                        }
                    }
                }
                if (!ran) {
                    if (write(STDERR_FILENO, stderr_buf, read_stderr_bytes) == -1) {
                        perror("write");
                        exit(1);
                    }
                }
            } else if (read_stderr_bytes == 0) {
                stderr_closed = 1;
            } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
                perror("read stderr");
                exit(1);
            }
        }
    }
}

void close_fd_list(command *cmd, int *fd_list, size_t fd_list_size) {
    for (int i = 0; i < fd_list_size; ++i) {
        if (fd_list[i] == -1) {
            continue;
        }
        if (close(fd_list[i]) == -1) {
            free(fd_list);
            perror("close");
            exit(1);
        }
    }
    free(fd_list);
}

int handle_output_redirection(command *cmd, int stdin_copy, bool is_job) {
    int return_value = 0;

    int stdout_pipe[2];
    if (pipe(stdout_pipe) == -1) {
        perror("pipe");
        exit(1);
    }

    int stderr_pipe[2];
    if (pipe(stderr_pipe) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        close(stdout_pipe[0]);
        close(stderr_pipe[0]);
        exit(run_command_with_redirection(cmd, stdout_pipe, stderr_pipe,
                                          is_job)); // child process execute the command
    } else {
        close(stdout_pipe[1]);
        close(stderr_pipe[1]);

        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return_value = WEXITSTATUS(status);
        }

        /* open without O_CREAT flag : if the file exist and tries to overwrite it, it will fail
        in that case, we remove all previously created files and return 1 */
        int *test_open = malloc(sizeof(int) * cmd->output_redirection_count);
        for (int i = 0; i < cmd->output_redirection_count; ++i) {
            int flags = get_output_flags(&cmd->output_redirections[i]);
            if (flags & O_EXCL) {
                test_open[i] = open(cmd->output_redirections[i].filename, flags, 0666);
                if (test_open[i] == -1) {
                    if (errno == EEXIST) {
                        fprintf(stderr, "jsh: %s: cannot overwrite existing file\n",
                                cmd->output_redirections[i].filename);
                        for (int j = 0; j < i; ++j) {
                            remove(cmd->output_redirections[i].filename);
                        }
                        close_fd_list(cmd, test_open, i);
                        return 1;
                    }
                }
            } else {
                test_open[i] = -1;
            }
        }

        for (int i = 0; i < cmd->output_redirection_count; ++i) {
            if (test_open[i] != -1) {
                remove(cmd->output_redirections[i].filename);
            }
        }
        close_fd_list(cmd, test_open, cmd->output_redirection_count);

        int *fd_list = malloc(sizeof(int) * cmd->output_redirection_count);
        for (int i = 0; i < cmd->output_redirection_count; ++i) {
            fd_list[i] =
                open(cmd->output_redirections[i].filename, get_output_flags(&cmd->output_redirections[i]), 0666);
            if (fd_list[i] == -1) {
                if (errno == EEXIST) {
                    fprintf(stderr, "jsh: %s: cannot overwrite existing file\n", cmd->output_redirections[i].filename);
                    close_fd_list(cmd, fd_list, i);
                    return 1;
                } else {
                    perror("open");
                    close_fd_list(cmd, fd_list, i);
                    exit(1);
                }
            }
        }

        // Reads from stdout and stderr pipes and writes to the output files
        monitor_and_handle_pipes(cmd, stdout_pipe, stderr_pipe, fd_list);

        close_fd_list(cmd, fd_list, cmd->output_redirection_count);

        if (close(stdout_pipe[0]) == -1) {
            perror("close");
            exit(1);
        }
        if (close(stderr_pipe[0]) == -1) {
            perror("close");
            exit(1);
        }

        dup2(stdin_copy, STDIN_FILENO);
        return return_value;
    }
}

int run_command(command *cmd, bool is_job) {

    int return_value = 0;

    // No redirections
    if (cmd->input_redirection_filename == NULL) {
        if (cmd->output_redirection_count == 0) {
            return handle_no_redirections(cmd, is_job);
        } else {
            return handle_output_redirection(cmd, STDIN_FILENO, is_job);
        }
    }

    // Input redirection
    int stdin_copy;
    if (cmd->input_redirection_filename != NULL) {
        return_value = handle_input_redirection(cmd, &stdin_copy);
        if (return_value != 0) {
            return return_value;
        }
    }

    // Input redirection but no output redirection
    if (cmd->output_redirection_count == 0) {
        return_value = run_command_without_redirections(cmd, is_job);
        dup2(stdin_copy, STDIN_FILENO);
        return return_value;
    }

    return handle_output_redirection(cmd, stdin_copy, is_job);
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
                run_output = run_command(pip->commands[i], true);
                exit(run_output);
            } else {
                run_output = add_new_forked_process_to_jobs(pid, pip);
            }
        } else {
            run_output = run_command(pip->commands[i], false);
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
