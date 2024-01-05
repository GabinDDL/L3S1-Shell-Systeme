#include "run.h"
#include "../utils/signal_management.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int run_command_without_redirections(command *cmd, bool already_forked, pipeline *pip) {
    int return_value = 0;

    if (cmd->name == NULL) {
        return_value = last_command_exit_value;
    } else if (strcmp(cmd->argv[0], "pwd") == 0) {
        reset_signal_management();
        return_value = pwd(cmd);
        use_jsh_signal_management();
    } else if (strcmp(cmd->argv[0], "cd") == 0) {
        reset_signal_management();
        int cd_output = cd(cmd);
        use_jsh_signal_management();
        update_prompt();
        return_value = cd_output;
    } else if (strcmp(cmd->argv[0], "exit") == 0) {
        reset_signal_management();
        return_value = exit_jsh(cmd);
        use_jsh_signal_management();
    } else if (strcmp(cmd->argv[0], "?") == 0) {
        reset_signal_management();
        return_value = print_last_command_result(cmd);
        use_jsh_signal_management();
    } else if (strcmp(cmd->argv[0], "jobs") == 0) {
        reset_signal_management();
        return_value = print_jobs(cmd);
        use_jsh_signal_management();
    } else if (strcmp(cmd->argv[0], "kill") == 0) {
        reset_signal_management();
        return_value = jsh_kill(cmd);
        use_jsh_signal_management();
    } else {
        if (already_forked) {
            reset_signal_management();
            return_value = extern_command(cmd);
            use_jsh_signal_management();
        } else {
            int status; // status of the created process
            pid_t pid = fork();

            assert(pid != -1);

            switch (pid) {
            case 0:
                reset_signal_management();
                return_value = extern_command(cmd);
                use_jsh_signal_management();
                if (return_value < 0) {
                    perror("execvp");
                    exit(errno);
                }
                exit(SUCCESS);
                break;
            default:
                setpgid(pid, pid);
                tcsetpgrp(STDERR_FILENO, getpgid(pid));
                waitpid(pid, &status, WUNTRACED);
                if (WIFSTOPPED(status)) {
                    setpgid(pid, pid);
                    pip->to_job = true;
                    add_new_forked_process_to_jobs(pid, pip, STOPPED);
                }
                tcsetpgrp(STDERR_FILENO, getpgid(0));
                return WEXITSTATUS(status);
            }
        }
    }
    fflush(stderr);
    fflush(stdout);
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

int run_command(command *cmd, bool already_forked, pipeline *pip) {

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

    return_value = run_command_without_redirections(cmd, already_forked, pip);

    dup2(stdin_copy, STDIN_FILENO);
    dup2(stdout_copy, STDOUT_FILENO);
    dup2(stderr_copy, STDERR_FILENO);

    close(stdin_copy);
    close(stdout_copy);
    close(stderr_copy);

    return return_value;
}

int **init_tubes(size_t n) {
    int **tubes = malloc(n * sizeof(int *));
    assert(tubes != NULL);

    for (size_t i = 0; i < n; i++) {
        tubes[i] = malloc(sizeof(int) * 2);
        assert(tubes != NULL);

        assert(pipe(tubes[i]) >= 0);
    }
    return tubes;
}

void close_fd_of_tubes_except(int **tubes, size_t s, unsigned rd, unsigned wr) {
    for (size_t i = 0; i < s; i++) {
        if (i != rd) {
            close(tubes[i][0]);
        }
        if (i != wr) {
            close(tubes[i][1]);
        }
    }
}

void free_tubes(int **tubes, size_t size) {
    for (size_t i = 0; i < size; i++) {
        free(tubes[i]);
    }
    free(tubes);
}

int run_commands_of_pipeline(pipeline *pip, int fd_out) {
    int run_output = 0;

    pid_t pids[pip->command_count - 1];
    int **tubes = init_tubes(pip->command_count - 1);

    for (size_t i = 1; i < pip->command_count; i++) {
        pids[i - 1] = fork();
        if (pids[i - 1] == 0) {
            close_fd_of_tubes_except(tubes, pip->command_count - 1, i - 1, i);
            dup2(tubes[i - 1][0], STDIN_FILENO);
            close(tubes[i - 1][0]);
            if (i < pip->command_count - 1) {
                dup2(tubes[i][1], STDOUT_FILENO);
                close(tubes[i][1]);
            }
            run_output = run_command(pip->commands[i], true, pip);

            exit(run_output);
        }
    }
    if (pip->to_job) {
        int res = write(fd_out, pids, sizeof(pid_t) * (pip->command_count - 1));
        assert(res >= 0);
        close(fd_out);
    }

    close_fd_of_tubes_except(tubes, pip->command_count - 1, pip->command_count, 0);

    int stdout_copy = dup(STDOUT_FILENO);
    dup2(tubes[0][1], STDOUT_FILENO);
    close(tubes[0][1]);

    run_output = run_command(pip->commands[0], pip->to_job, pip);

    dup2(stdout_copy, STDOUT_FILENO);
    close(stdout_copy);
    for (size_t i = 0; i < pip->command_count - 1; i++) {
        waitpid(pids[i], NULL, 0);
    }
    free_tubes(tubes, pip->command_count - 1);

    return run_output;
}

int run_pipeline(pipeline *pip) {
    assert(pip != NULL);
    if (pip->command_count == 0) {
        return last_command_exit_value;
    }
    int run_output = 0;
    unsigned njob = job_number;

    if (pip->to_job) {
        int tube_process[2];

        if (pip->command_count > 1) {
            pipe(tube_process);
        }
        pid_t pid = fork();
        if (pid == -1) {
            print_error("fork: error to create a process");
        }
        if (pid == 0) {
            if (pip->command_count > 1) {
                close(tube_process[0]);
                run_commands_of_pipeline(pip, tube_process[1]);
                exit(run_output);
            } else {
                run_output = run_command(pip->commands[0], true, pip);
                exit(run_output);
            }
        } else {
            setpgid(pid, pid);
            run_output = add_new_forked_process_to_jobs(pid, pip, RUNNING);
            if (pip->command_count > 1) {
                pid_t *pids = malloc((pip->command_count - 1) * sizeof(pid_t));
                int res = read(tube_process[0], pids, sizeof(pid_t) * (pip->command_count - 1));
                assert(res >= 0);
                close(tube_process[0]);
                close(tube_process[1]);

                unsigned job_id = jobs[job_number - 1]->id;

                for (size_t i = 0; i < pip->command_count - 1; i++) {
                    setpgid(pids[i], getpgid(pid));
                    add_process_to_job(job_id, pids[i], pip->commands[i + 1], RUNNING);
                }
                free(pids);
            }
        }
    } else {
        if (pip->command_count > 1) {
            run_commands_of_pipeline(pip, -1);
        } else {
            run_output = run_command(pip->commands[0], false, pip);
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
