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

command_without_substitution *prepare_command(command *, job *);
int **init_tubes(size_t);
void close_fd_of_tubes_except(int **, size_t, int, int);
void free_tubes(int **, size_t);

char *fd_to_proc_path(int fd) {
    static char proc_path[256];
    sprintf(proc_path, "/proc/self/fd/%d", fd);
    return proc_path;
}

process_substitution_output fd_from_subtitution_arg_with_pipe(argument *sub_arg, job *j) {
    assert(sub_arg != NULL);
    assert(sub_arg->type == ARG_SUBSTITUTION);
    pipeline *pip = sub_arg->value.substitution;
    assert(pip != NULL);
    assert(pip->command_count > 0);
    assert(pip->commands[0] != NULL);

    if (pip->command_count == 1) {
        command_without_substitution *cmd_without_subst = prepare_command(pip->commands[0], j);
        int tube[2];
        assert(pipe(tube) >= 0);

        pid_t pid = fork();
        assert(pid != -1);

        switch (pid) {
        case 0:
            close(tube[0]);
            dup2(tube[1], STDOUT_FILENO);
            close(tube[1]);

            run_command(cmd_without_subst, true, pip, j, false);

            exit(SUCCESS);
            break;
        default:
            close(tube[1]);
            process_substitution_output output = {pid, tube[0]};
            if (j->pgid == -1) {
                setpgid(pid, pid);
                pid_t pgid = getpgid(pid);

                j->pid_leader = pid;
                j->pgid = pgid;
                j->status = RUNNING;
            } else {
                setpgid(pid, j->pgid);
            }
            add_process_to_job(j, pid, pip->commands[0], cmd_without_subst, RUNNING);

            return output;
        }
    }

    pid_t pids[pip->command_count - 1];
    int **tubes = init_tubes(pip->command_count - 1);

    command_without_substitution **cmds_without_subst =
        malloc(sizeof(command_without_substitution *) * pip->command_count);
    assert(cmds_without_subst != NULL);

    for (int i = 0; i < pip->command_count - 1; i++) {
        cmds_without_subst[i] = prepare_command(pip->commands[i], j);

        pids[i] = fork();
        if (pids[i] == 0) {
            close_fd_of_tubes_except(tubes, pip->command_count - 1, i - 1, i);
            dup2(tubes[i][1], STDOUT_FILENO);
            close(tubes[i][1]);
            if (i != 0) {
                dup2(tubes[i - 1][0], STDIN_FILENO);
                close(tubes[i - 1][0]);
            }
            run_command(cmds_without_subst[i], true, pip, j, false);

            close(tubes[i][0]);
            exit(SUCCESS);
        } else {
            if (j->pgid == -1) {
                setpgid(pids[i], pids[i]);
                pid_t pgid = getpgid(pids[i]);

                j->pid_leader = pids[i];
                j->pgid = pgid;
                j->status = RUNNING;
            } else {
                setpgid(pids[i], j->pgid);
            }
            add_process_to_job(j, pids[i], pip->commands[i], cmds_without_subst[i], RUNNING);
        }
    }

    close_fd_of_tubes_except(tubes, pip->command_count - 1, pip->command_count - 2, pip->command_count);

    int stdin_copy = dup(STDIN_FILENO);
    dup2(tubes[pip->command_count - 2][0], STDIN_FILENO);
    close(tubes[pip->command_count - 2][0]);

    free_tubes(tubes, pip->command_count - 1);

    cmds_without_subst[pip->command_count - 1] = prepare_command(pip->commands[pip->command_count - 1], j);

    int tube[2];
    assert(pipe(tube) >= 0);

    pid_t pid = fork();
    assert(pid != -1);

    switch (pid) {
    case 0:
        close(tube[0]);
        dup2(tube[1], STDOUT_FILENO);
        close(tube[1]);

        run_command(cmds_without_subst[pip->command_count - 1], false, pip, j, true);

        exit(SUCCESS);
        break;
    default:
        close(tube[1]);
        process_substitution_output output = {pid, tube[0]};

        if (j->pgid == -1) {
            setpgid(pid, pid);
            pid_t pgid = getpgid(pid);

            j->pid_leader = pid;
            j->pgid = pgid;
            j->status = RUNNING;
        } else {
            setpgid(pid, j->pgid);
        }
        add_process_to_job(j, pid, pip->commands[pip->command_count - 1], cmds_without_subst[pip->command_count - 1],
                           RUNNING);

        for (size_t i = 0; i < pip->command_count; i++) {
            cmds_without_subst[i] = NULL;
        }

        free(cmds_without_subst);

        dup2(stdin_copy, STDIN_FILENO);
        close(stdin_copy);

        return output;
    }
}

command_without_substitution *prepare_command(command *cmd, job *j) {
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
    assert(cmd_without_substitution->name != NULL);

    cmd_without_substitution->argc = cmd->argc;
    cmd_without_substitution->argv = malloc(sizeof(char *) * (cmd->argc + 1));
    assert(cmd_without_substitution->argv != NULL);

    cmd_without_substitution->pids = malloc(sizeof(pid_t) * MAX_TOKENS);
    assert(cmd_without_substitution->pids != NULL);
    cmd_without_substitution->pid_count = 0;

    for (size_t i = 0; i < cmd->argc; ++i) {
        if (cmd->argv[i]->type == ARG_SIMPLE) {
            cmd_without_substitution->argv[i] = strdup(cmd->argv[i]->value.simple);
            assert(cmd_without_substitution->argv[i] != NULL);
        } else if (cmd->argv[i]->type == ARG_SUBSTITUTION) {
            process_substitution_output output = fd_from_subtitution_arg_with_pipe(cmd->argv[i], j);
            cmd_without_substitution->argv[i] = strdup(fd_to_proc_path(output.fd));
            assert(cmd_without_substitution->argv[i] != NULL);
            cmd_without_substitution->pids[cmd_without_substitution->pid_count] = output.pid;
            cmd_without_substitution->pid_count++;
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

bool is_intern_command(const char *cmd_name) {
    return strcmp(cmd_name, "pwd") == 0 || strcmp(cmd_name, "cd") == 0 || strcmp(cmd_name, "exit") == 0 ||
           strcmp(cmd_name, "?") == 0 || strcmp(cmd_name, "jobs") == 0 || strcmp(cmd_name, "kill") == 0 ||
           strcmp(cmd_name, "bg") == 0 || strcmp(cmd_name, "fg") == 0;
}

int run_intern_command(command_without_substitution *cmd_without_subst) {
    int return_value;
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
    }
    return return_value;
}

int run_command_without_redirections(command_without_substitution *cmd_without_subst, bool already_forked,
                                     pipeline *pip, job *j, bool is_leader) {
    int return_value = 0;

    if (cmd_without_subst->name == NULL) {
        return_value = last_command_exit_value;

        fflush(stderr);
        fflush(stdout);

        free_command_without_substitution(cmd_without_subst);

        return return_value;
    }
    if (already_forked) {
        if (is_intern_command(cmd_without_subst->argv[0])) {
            return_value = run_intern_command(cmd_without_subst);
        } else {
            return_value = extern_command(cmd_without_subst);
        }
        return return_value;
    }

    if (!pip->to_job && is_intern_command(cmd_without_subst->argv[0]) && is_leader) {
        j->pipeline = NULL;
        free_job(j);
        return_value = run_intern_command(cmd_without_subst);
        free_command_without_substitution(cmd_without_subst);
        return return_value;
    }

    int status; // status of the created process
    pid_t pid = fork();

    assert(pid != -1);

    switch (pid) {
    case 0:
        for (size_t i = 0; i < cmd_without_subst->pid_count; i++) {
            waitpid(cmd_without_subst->pids[i], NULL, 0);
        }
        if (is_intern_command(cmd_without_subst->argv[0])) {
            return_value = run_intern_command(cmd_without_subst);
            exit(return_value);
        }
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
        if (j->pgid == -1) {
            setpgid(pid, pid);
            pid_t pgid = getpgid(pid);

            j->pid_leader = pid;
            j->pgid = pgid;
            j->status = RUNNING;
        } else {
            setpgid(pid, j->pgid);
        }

        add_process_to_job(j, pid, pip->commands[0], cmd_without_subst, RUNNING);

        if (is_leader) {
            if (pip->to_job) {
                add_job_to_jobs(j);
                print_job(j, true);
                return SUCCESS;
            } else {

                tcsetpgrp(STDERR_FILENO, getpgid(pid));

                waitpid(pid, &status, WUNTRACED);
                if (WIFSTOPPED(status)) {
                    pip->to_job = true;
                    j->status = STOPPED;
                    add_job_to_jobs(j);
                    print_job(j, true);
                    j->pipeline->to_job = true;
                    j->job_process[j->process_number - 1]->status = STOPPED;
                } else if (WIFSIGNALED(status)) {
                    j->job_process[j->process_number - 1]->status = KILLED;

                    j->pipeline->to_job = false;
                    j->pipeline = NULL;

                    free_job(j);
                } else if (WIFEXITED(status)) {
                    j->job_process[j->process_number - 1]->status = DONE;

                    j->pipeline->to_job = false;
                    j->pipeline = NULL;

                    free_job(j);
                }

                tcsetpgrp(STDERR_FILENO, getpgrp());

                fflush(stderr);
                fflush(stdout);
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

int run_command(command_without_substitution *cmd_without_subst, bool already_forked, pipeline *pip, job *j,
                bool is_leader) {

    int return_value = 0;

    int stdin_copy = dup(STDIN_FILENO);
    int stdout_copy = dup(STDOUT_FILENO);
    int stderr_copy = dup(STDERR_FILENO);

    int fd_in, fd_out, fd_err;
    for (size_t i = 0; i < cmd_without_subst->redirection_count; ++i) {
        if (cmd_without_subst->redirections[i].type == REDIRECT_STDIN) {
            char *filename = cmd_without_subst->redirections[i].filename;

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
                process_substitution_output output = fd_from_subtitution_arg_with_pipe(sub_arg, j);
                fd_in = output.fd;
                cmd_without_subst->pids[cmd_without_subst->pid_count] = output.pid;
                cmd_without_subst->pid_count++;
                free(substitution);
                free(sub_arg);
            } else {
                fd_in = open(cmd_without_subst->redirections[i].filename, O_RDONLY);
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
        } else if (cmd_without_subst->redirections[i].type == REDIRECT_STDOUT) {
            fd_out =
                open(cmd_without_subst->redirections[i].filename, get_flags(&cmd_without_subst->redirections[i]), 0666);
            if (fd_out == -1) {
                if (errno == EEXIST) {
                    fprintf(stderr, "jsh: %s: cannot overwrite existing file\n",
                            cmd_without_subst->redirections[i].filename);
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
        } else if (cmd_without_subst->redirections[i].type == REDIRECT_STDERR) {
            fd_err =
                open(cmd_without_subst->redirections[i].filename, get_flags(&cmd_without_subst->redirections[i]), 0666);
            if (fd_err == -1) {
                if (errno == EEXIST) {
                    fprintf(stderr, "jsh: %s: cannot overwrite existing file\n",
                            cmd_without_subst->redirections[i].filename);
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

    return_value = run_command_without_redirections(cmd_without_subst, already_forked, pip, j, is_leader);

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

int run_commands_of_pipeline(pipeline *pip, job *j) {
    int run_output = 0;

    pid_t pids[pip->command_count - 1];
    int **tubes = init_tubes(pip->command_count - 1);

    command_without_substitution **cmds_without_subst =
        malloc(sizeof(command_without_substitution *) * pip->command_count);
    assert(cmds_without_subst != NULL);

    for (int i = 0; i < pip->command_count - 1; i++) {
        cmds_without_subst[i] = prepare_command(pip->commands[i], j);

        pids[i] = fork();
        if (pids[i] == 0) {
            close_fd_of_tubes_except(tubes, pip->command_count - 1, i - 1, i);
            dup2(tubes[i][1], STDOUT_FILENO);
            close(tubes[i][1]);
            if (i != 0) {
                dup2(tubes[i - 1][0], STDIN_FILENO);
                close(tubes[i - 1][0]);
            }
            run_output = run_command(cmds_without_subst[i], true, pip, j, false);

            close(tubes[i][0]);
            exit(run_output);
        } else {
            if (j->pgid == -1) {
                setpgid(pids[i], pids[i]);
                pid_t pgid = getpgid(pids[i]);

                j->pid_leader = pids[i];
                j->pgid = pgid;
                j->status = RUNNING;
            } else {
                setpgid(pids[i], j->pgid);
            }
            add_process_to_job(j, pids[i], pip->commands[i], cmds_without_subst[i], RUNNING);
        }
    }

    close_fd_of_tubes_except(tubes, pip->command_count - 1, pip->command_count - 2, pip->command_count);

    int stdin_copy = dup(STDIN_FILENO);
    dup2(tubes[pip->command_count - 2][0], STDIN_FILENO);
    close(tubes[pip->command_count - 2][0]);

    free_tubes(tubes, pip->command_count - 1);

    cmds_without_subst[pip->command_count - 1] = prepare_command(pip->commands[pip->command_count - 1], j);

    run_output = run_command(cmds_without_subst[pip->command_count - 1], false, pip, j, true);

    dup2(stdin_copy, STDIN_FILENO);
    close(stdin_copy);

    for (size_t i = 0; i < pip->command_count; i++) {
        cmds_without_subst[i] = NULL;
    }

    free(cmds_without_subst);

    return run_output;
}

int run_pipeline(pipeline *pip, job *j, bool is_leader) {
    assert(pip != NULL);
    if (pip->command_count == 0) {
        return last_command_exit_value;
    }
    int run_output = 0;
    unsigned njob = job_number;

    if (pip->command_count > 1) {
        run_commands_of_pipeline(pip, j);
    } else {
        command_without_substitution *cmd_without_subst = prepare_command(pip->commands[0], j);

        run_output = run_command(cmd_without_subst, false, pip, j, is_leader);
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
        job *j = init_job_to_add(-1, -1, pips->pipelines[i], RUNNING);
        run_output = run_pipeline(pips->pipelines[i], j, true);
    }
    return run_output;
}
