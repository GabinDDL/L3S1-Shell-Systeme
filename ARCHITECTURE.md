# Architecure JSH

JSH is an interactive command interpreter that incorporates a number of classic shell
functions, in particular job control, i.e. management of tasks launched from the shell.
It can execute all external commands, and offers some internal commands, as well as redirection
of standard streams and pipe combinations.

## Table of Contents
1. [Project structure](#project-structure)
2. [Internal structures](#internal-structures)
    1. [Core](#core)
    2. [Jobs core](#jobs-core)
    3. [Parsing](#parsing)
    4. [Jobs](#jobs)
3. [Examples](#examples)
    1. [About everthing besides jobs](#about-everthing-besides-jobs)
    2. [About jobs](#about-jobs)

## Project structure 
*(definition inside `src`)*


The project is first divided into 4 directories, with the `main` at the top.
The 4 files and their programs are :
- `builtins`
    `builtins` contains all `jsh's` internal `command` programs.
    The programs are as follows:
    - `pwd` which used to display the absolute physical reference of the current working directory directory.
    - `cd` which used to change the current working directory to the ref directory (if a valid
    is a valid reference), the previous working directory if the parameter is -, or $HOME if no parameter is specified.
    - `print_last_command_result` which used to display the return value of the last command executed.
    - `exit` which used to display a warning message if one or more jobs are running or suspended. 
    The return value is 1 in this case.
    Otherwise, it terminates the `jsh` process with the return value val (if a argument is supplied),
    or by default the return value of the last command executed.
    - `jobs` which, if no argument is given, is used to display the list of current jobs, specifying
    the job number in square brackets, the process group identifier, the job status (Running, Stopped, Detached, Killed or Done) and the command line it is executing.
    With the -t option, it lists the process tree for each job,
    indicating its pid, status and the command it is executing; if a job number is passed as an argument to jobs,
    the list is restricted to the job in question.
    - `bg` which is used to restart execution of the job specified in the argument in the background.
    - `fg` which is used brings the execution of the job specified in the argument back to the foreground.
    - `kill` which is used to send the sig signal (or SIGTERM by default) to all processes of the job number job, or to the process of identifier pid.
- `parser`
    parser contains the parser of the commands to be executed.
    The program is as follows:
    - `parser` which parses a command into a structure explained below.
- `run`
    run contains the program for executing command lines.
    The program is as follows:
    - `run` which executes the given line from a line parsed by the parser.
- `utils`
    `utils` contains cores, and the useful functions needed for strings, for example.
    The programs is as follows
    - `constants` which contains all jsh constant variables, as well as the functions to initialize them.
    - `core` which contains the majority of global variables (all except those for jobs) as well as their initialization, update and free functions. 
    - `int_utils` which is used to have functions concerning integers.
    - `jobs_core`which contains all global job variables and their related functions.
    - `signal_management` which is used to manage signal handlers, to remove and reinsert them.
    - `string_utils` which is used to have functions concerning integers.
    
## Internal structures 

### Core 
*(definition inside `src/utils/core.h`)*


The core represents the jsh's internal structure, where the project's global variables are located.
Its variables are :

- `current_folder` : current user position, initialized with `PWD` from constant.
- `prompt` : `command` readout prompt.
- `last_command_exit_value` : last user command exit value.
- `last_reference_position` : last user location, initialized with `PWD` from constant.
- `last_line_read` : last line typed by the user.
- `current_pipeline_list` : current_pipeline run.

### Jobs core 
*(definition inside `src/utils/jobs_core.h`)*


The core represents the jsh's internal jobs structure, where the project's global jobs variables are located.
Its variables are :

- `job_number` : the number of `job` monitored by `jsh`.
- `jobs` : the `job` list, monitored by `jsh`.

### Parsing 
*(definition inside `src/parser/parser.h`)*


One of the main features of `jsh` is its parsing management.
To achieve this, we use several structures to represent a `command` line:

- **pipeline_list** *(contains `pipelines` structure)*
It is used to delimit `&` and thus what is considered a future `job` or not.
Each `pipeline` is then considered as a line in its own right.

- **pipeline** *(contains `command` structures and a boolean `to_job`)*
This structure is used to delimit the `|` between `commands`. The `to_job` boolean
is used to determine whether or not the `pipeline` should be monitored during execution.

- **command** *(contains a `name`, `argument` structures and `redirect` structures)*
This structure represents a `command` with its name and arguments, which can be substitutions,
and which may or may not contain `redirects`.

- **command_without_substitution** *(contains a `name`, `argument` strings, `redirects` structures and
the `pids` of these substitutions)*
This structure represents a `command` with its name and arguments. Unlike a `command`,
there is no substitution in the arguments, which have been replaced by the `command` pipe.
After waiting for the end of these substitutions with their `pids`, the `command` is now ready
to be executed.

- **argument** *(contains a `type` and a `value`)*
This structure is represented by its `type` to determine whether it is a substitution or not.
The `value` will contain the `command` string and the substitution `pipeline`.

- **redirection** *(contains a `type`, a `mode` and a `file's reference string`)*
The `redirection type` is used to determine which descriptor from `standard input`, `standard output` 
or `error output` should be redirected to the `redirection file`. The `mode` represents the `redirection file's`
opening modes, including `overwrite`, `append`, `no overwrite` and `none`.

### Jobs 
*(definition inside `src/utils/jobs_core.h`)*


Another important part of `jsh` is its `job` management. For this, 2 different structures are used:

- **job** *(contains an `id`, a `pgid`, the `pid` of the leader of the group, a `status`, a `pipeline` and its `processes`)*
The structure is represented by the `id`, which differentiates the `job` from other `jobs`, and the `pgid`
and pid of the leader. The job's pipeline represents the line that created it, its status is an enumeration
of `Running`, `Stopped`, `Deprecated`, `Killed` and `Done`, and finally its `processes` structures.
This whole structure represents a group of `processes`, monitored by `jsh`.

- **process** *(contains a `pid`, a `status`, a `command` and a `command_without_substitution`)*
The structure is represented by the `pid`, which differentiates the `process` from others. The `process's command`
represent the executed `command` of it, and its `status` is the same enumeration as `job`.
A `process` is used to represent the various forks created by `jsh` that are monitored by the `job` created.

## Examples

A given pipeline is part of the same job if it is not separated from the previous one by a `&`. Each processes part of this job shares the same process group identifier (`PGID`), which is the `PID` of the first process in the pipeline. The `PGID` is used to send signals to all processes in the job.

### About everthing besides jobs

```bash
cat <( echo grosminet | cat ) <( echo titi | cat | cat ) <( ls -l > redirected.txt ) | wc -l > 2.txt 
```

#### Understanding the command

This bash command is an interesting combination of several elements: process substitutions, redirections, pipes, and commands.

1. **Process substitution with `<()`**:
   - `<(...)`: This syntax creates what's called a "substitution," allowing you to execute a command as if it were a file. Here, you have three subshells:
     - `<( echo grosminet | cat )`: This substitution runs `echo grosminet` and then passes its output to `cat`, which simply displays it.
     - `<( echo titi | cat | cat )`: Similarly, `echo titi` is passed to `cat` and then to another `cat`.
     - `<( ls -l > redirected.txt )`: This substitution lists the contents of the current directory with details (`ls -l`), but instead of displaying the result, it redirects the output to the file `redirected.txt`.
   
2. **Concatenation of outputs with `cat`**:
   - The main `cat` command (at the beginning of the line) takes the outputs of the three substitutions and concatenates them. Since the third substitution redirects its output to a file, it doesn't add anything to the `cat` output.

3. **Piping to `wc -l`**:
   - `| wc -l`: The result of `cat` (which is now the combination of outputs from "grosminet" and "titi") is piped to `wc -l`, which counts the number of lines.

4. **Final redirection to `2.txt`**:
   - `> 2.txt`: The output of `wc -l` (the line count) is then redirected to a file named `2.txt`.

#### How does it work in JSH (Under the hood)?

##### Parsing

Once the command is entered, it is parsed by the parser, which will create a `pipeline_list` structure. This structure will contain one `pipeline` structure which will contain two `command` structures separated by `|`. The first `command` structure will contain three `argument` structures, which are all of type `substitution` structure. The second `command` structure will contain one `argument` structure, of type `string` and one `redirect` structure. The `redirection` structure will contain the `RedirectionType` `REDIRECT_STDOUT`, the `RedirectionMode` `REDIRECT_NO_OVERWRITE` and the `file's name` `2.txt`.

##### Running

After the parsing stage, the run module takes over. This module is responsible for executing the parsed command structures.

1. Executing Pipeline :

`run_pipeline_list` is the function that executes the pipeline list. It takes the pipeline list structure as an argument and executes each pipeline in the list using `run_pipeline`. In this case, there is only one pipeline in the list, so it is executed directly.

In `run_commands_of_pipeline`, the STDOUT of the first command is redirected to the write end of the pipe, and the STDIN of the second command is redirected to the read end of the pipe. This allows the output of the first command to be used as the input for the second command.

2. Executing Substitutions:

For each substitution structure in the command structure, the run module executes the command inside the substitution. This is done by `fd_from_subtitution_arg_with_pipe` by creating a new process using `fork()`, executing the command in the child process, and capturing the output in the parent process. The output is then used as an argument for the main command.

3. Handling Redirections:

If there are any redirections in the command structure, the `run_command` handles them before executing the command. For output redirections (like `> 2.txt`), the run module changes the file descriptor of STDOUT to point to the specified file. This means that any output from the command will be written to the file instead of the terminal.

4. Executing the Commands:

The run module then executes the first command in the pipeline using `run_command_without_redirections`. If the command is a builtin (such as `pwd` or `?`), it is executed directly in the current process. If it is an external command, a new process is created using `fork()`, and the command is executed in the child process. The input and output of the command are redirected to the read and write ends of the pipe, respectively. This allows the output of the command to be used as the input for the next command in the pipeline.

### About jobs

```bash
sleep 3 & sleep 2 & sleep 5 
```

#### Understanding the command

The `sleep 3 & sleep 2 & sleep 5` command in Bash utilizes a combination of `sleep` and the `&` character. Here's what happens:

1. **`sleep 3`**: This command instructs the shell to pause for 3 seconds.
2. **`&`**: The `&` symbol at the end of a command in a Bash shell means that the command should be executed in the background. This allows the shell to continue running other commands without waiting for the `sleep` command to finish.
3. **`sleep 2`**: Right after that, the `sleep 2` command is launched. It instructs the shell to pause for 2 seconds. Like the previous command, it's executed in the background.
4. **`sleep 5`**: Finally, the `sleep 5` command is executed, requesting a pause of 5 seconds.

All these `sleep` commands are executed almost simultaneously in the background, except for the last one. They do not block each other. The shell does not wait for each `sleep` to finish before moving on to the next one.

So, even though the `sleep 3` and `sleep 2` commands complete after 3 and 2 seconds, respectively, the `sleep 5` command continues running in the foreground, and the shell waits for it to finish before giving you back control.

#### How does it work in JSH (Under the hood)?

##### Parsing

After parsing, the `pipeline_list` structure will contain three `pipeline` structures, each containing one `command` structure.

##### Running

1. When the command `sleep 3 & sleep 2 & sleep 5` is run, the `run_pipeline_list` will start three separate processes for each sleep command. Because of the & symbol, the first two sleep commands will be run in the background, allowing the shell to immediately start the next command.

2. When a sleep command is started in the background, a job is created with `init_job_to_add` and added to the jobs list using the `add_job_to_jobs` function from `src/utils/jobs_core.c`. The shell can then continue executing other commands without waiting for these background tasks to finish.

3. When the last sleep command is started, the shell will wait for it to finish before giving you back control. This is because the last sleep command is not started in the background, so the shell will wait for it to finish before continuing.
