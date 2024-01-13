#include "run.h"
#include "../utils/signal_management.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

char *fd_to_proc_path(int fd) {
    static char proc_path[256];
    sprintf(proc_path, "/proc/self/fd/%d", fd);
    return proc_path;
}

int fd_from_subtitution_arg_with_pipe(argument *sub_arg) {
    assert(sub_arg != NULL);
    assert(sub_arg->type == ARG_SUBSTITUTION);
    pipeline *pip = sub_arg->value.substitution;
    assert(pip != NULL);
    assert(pip->command_count > 0);
    assert(pip->commands[0] != NULL);

    int tube[2];
    assert(pipe(tube) >= 0);

    pid_t pid = fork();
    assert(pid != -1);

    switch (pid) {
    case 0:
        close(tube[0]);
        dup2(tube[1], STDOUT_FILENO);
        close(tube[1]);

        run_pipeline(pip);
        exit(SUCCESS);
        break;
    default:
        close(tube[1]);
        return tube[0];
    }
}

command_without_substitution *prepare_command(command *cmd) {
    assert(cmd != NULL);

    if (cmd->name == NULL) {
        command_without_substitution *cmd_without_substitution = malloc(sizeof(command_without_substitution));
        assert(cmd_without_substitution != NULL);

        cmd_without_substitution->name = NULL;
        cmd_without_substitution->argc = 0;
        cmd_without_substitution->argv = NULL;
        cmd_without_substitution->redirection_count = 0;
        cmd_without_substitution->redirections = NULL;

        return cmd_without_substitution;
    }

    assert(cmd->argv != NULL);
    assert(cmd->argv[0] != NULL);
    assert(cmd->argv[0]->type == ARG_SIMPLE);

    command_without_substitution *cmd_without_substitution = malloc(sizeof(command_without_substitution));
    assert(cmd_without_substitution != NULL);

    cmd_without_substitution->name = strdup(cmd->name);
    cmd_without_substitution->argc = cmd->argc;
    cmd_without_substitution->argv = malloc(sizeof(char *) * (cmd->argc + 1));
    assert(cmd_without_substitution->argv != NULL);

    for (size_t i = 0; i < cmd->argc; ++i) {
        if (cmd->argv[i]->type == ARG_SIMPLE) {
            cmd_without_substitution->argv[i] = strdup(cmd->argv[i]->value.simple);
        } else if (cmd->argv[i]->type == ARG_SUBSTITUTION) {
            int fd = fd_from_subtitution_arg_with_pipe(cmd->argv[i]);
            cmd_without_substitution->argv[i] = strdup(fd_to_proc_path(fd));
        }
    }
    cmd_without_substitution->argv[cmd->argc] = NULL;

    cmd_without_substitution->redirection_count = cmd->redirection_count;
    cmd_without_substitution->redirections = malloc(sizeof(redirection) * cmd->redirection_count);
    assert(cmd_without_substitution->redirections != NULL);

    for (size_t i = 0; i < cmd->redirection_count; i++) {
        cmd_without_substitution->redirections[i].type = cmd->redirections[i].type;
        cmd_without_substitution->redirections[i].mode = cmd->redirections[i].mode;
        cmd_without_substitution->redirections[i].filename = strdup(cmd->redirections[i].filename);
    }

    return cmd_without_substitution;
}

int run_command_without_redirections(const command_without_substitution *cmd_without_subst, bool already_forked,
                                     pipeline *pip) {
    int return_value = 0;

    if (cmd_without_subst->name == NULL) {
        return_value = last_command_exit_value;

        fflush(stderr);
        fflush(stdout);
        return return_value;
    }

    if (strcmp(cmd_without_subst->argv[0], "pwd") == 0) {
        return_value = pwd(cmd_without_subst);
    } else if (strcmp(cmd_without_subst->argv[0], "cd") == 0) {
        int cd_output = cd(cmd_without_subst);
        update_prompt();
        return_value = cd_output;
    } else if (strcmp(cmd_without_subst->argv[0], "exit") == 0) {
        return_value = exit_jsh(cmd_without_subst);
    } else if (strcmp(cmd_without_subst->argv[0], "?") == 0) {
        return_value = print_last_command_result(cmd_without_subst);
    } else if (strcmp(cmd_without_subst->argv[0], "jobs") == 0) {
        return_value = print_jobs(cmd_without_subst);
    } else if (strcmp(cmd_without_subst->argv[0], "kill") == 0) {
        return_value = jsh_kill(cmd_without_subst);
    } else if (strcmp(cmd_without_subst->argv[0], "bg") == 0) {
        return_value = bg(cmd_without_subst);
    } else if (strcmp(cmd_without_subst->argv[0], "fg") == 0) {
        return_value = fg(cmd_without_subst);
    } else {
        if (already_forked) {
            return_value = extern_command(cmd_without_subst);
        } else {
            int status; // status of the created process
            pid_t pid = fork();

            assert(pid != -1);

            switch (pid) {
            case 0:
                reset_signal_management();
                return_value = extern_command(cmd_without_subst);
                use_jsh_signal_management();
                if (return_value < 0) {
                    perror("execvp");
                    exit(errno);
                }
                exit(SUCCESS);
                break;
            default:
                setpgid(pid, pid);
                pid_t pgid = getpgid(pid);
                tcsetpgrp(STDERR_FILENO, getpgid(pid));
                waitpid(pid, &status, WUNTRACED);
                if (WIFSTOPPED(status)) {
                    pip->to_job = true;
                    add_new_forked_process_to_jobs(pgid, pid, pip, STOPPED);
                }
                tcsetpgrp(STDERR_FILENO, getpgrp());
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
            char *filename = cmd->redirections[i].filename;

            if (is_substitution(filename)) {
                char *substitution = strdup(filename + 2);
                substitution[strlen(substitution) - 1] = '\0';
                pipeline *pip = parse_pipeline(substitution, false);
                if (pip == NULL) {
                    fprintf(stderr, "jsh: %s: invalid substitution\n", substitution);
                    return COMMAND_FAILURE;
                }
                argument *sub_arg = malloc(sizeof(argument));
                assert(sub_arg != NULL);
                sub_arg->type = ARG_SUBSTITUTION;
                sub_arg->value.substitution = pip;
                fd_in = fd_from_subtitution_arg_with_pipe(sub_arg);
                free(substitution);
                free(sub_arg);
            } else {
                fd_in = open(cmd->redirections[i].filename, O_RDONLY);
            }

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

    command_without_substitution *cmd_without_subst = prepare_command(cmd);
    return_value = run_command_without_redirections(cmd_without_subst, already_forked, pip);

    if (cmd_without_subst != NULL) {
        free_command_without_substitution(cmd_without_subst);
    }

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

void close_fd_of_tubes_except(int **tubes, size_t s, int rd, int wr) {
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

    for (int i = 0; i < pip->command_count - 1; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            close_fd_of_tubes_except(tubes, pip->command_count - 1, i - 1, i);
            dup2(tubes[i][1], STDOUT_FILENO);
            close(tubes[i][1]);
            if (i != 0) {
                dup2(tubes[i - 1][0], STDIN_FILENO);
                close(tubes[i - 1][0]);
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

    close_fd_of_tubes_except(tubes, pip->command_count - 1, pip->command_count - 2, pip->command_count);

    int stdin_copy = dup(STDIN_FILENO);
    dup2(tubes[pip->command_count - 2][0], STDIN_FILENO);
    close(tubes[pip->command_count - 2][0]);

    free_tubes(tubes, pip->command_count - 1);
    run_output = run_command(pip->commands[pip->command_count - 1], pip->to_job, pip);

    dup2(stdin_copy, STDIN_FILENO);
    close(stdin_copy);
    for (size_t i = 0; i < pip->command_count - 1; i++) {
        waitpid(pids[i], NULL, 0);
    }

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
            pid_t pgid = getpgid(pid);
            run_output = add_new_forked_process_to_jobs(pgid, pid, pip, RUNNING);
            if (pip->command_count > 1) {
                pid_t *pids = malloc((pip->command_count - 1) * sizeof(pid_t));
                int res = read(tube_process[0], pids, sizeof(pid_t) * (pip->command_count - 1));
                assert(res >= 0);
                close(tube_process[0]);
                close(tube_process[1]);

                unsigned job_id = jobs[job_number - 1]->id;

                for (size_t i = 0; i < pip->command_count - 1; i++) {
                    setpgid(pids[i], pgid);
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
