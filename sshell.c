#include <ctype.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#define ARGS 16
#define CMDLINE_MAX 512

// Struct for handling command line arguments
struct cmdline_args
{
    // Stores the arguments as a single string to be passed as the argument
    char *arguments;
    // Stores the command and arguments to be passed to execvp
    char *exec_args[ARGS];
    // To store the output file name in case of output redirection
    char *out_file;
    bool output_redirect;
    int out_error;
    int pipe_error;
    bool output_append;
    bool bg_jobs;
};

// Pipeline handling struct
struct pipeline
{
    // Stores the number of pipes in the input
    int count;
    // Stores the inputs from each pipe
    char *array[ARGS];
    char *array2[ARGS];
};

struct cmdline_args cmd_with_no_pipes(char cmd[CMDLINE_MAX], struct cmdline_args *cmdline_args, int counter);
struct cmdline_args out_append(char cmd[CMDLINE_MAX], struct cmdline_args *cmdline_args, int counter);
struct cmdline_args output_red(char str[512], struct cmdline_args *cmdline_args, int counter);
struct cmdline_args common_cmd(char cmd[CMDLINE_MAX], struct cmdline_args *cmdline_args, int counter);
void pipelining(int pipeline_count, struct pipeline *pipeline_cmd1, struct pipeline *pipeline_cmd2);
char *trim_whitespace(char *pointer);

// cd using chdir, with error handling
void cd(struct cmdline_args *cmdline_args, char* cmd_cd)
{
    // If directory can't be entered, display error message and print to stderr
    if (chdir(cmdline_args->exec_args[1]) != 0)
    {
        fprintf(stderr, "Error: cannot cd into directory\n");
        fprintf(stderr, "+ completed '%s' [1]\n", cmd_cd);
    }
    else
    {
        fprintf(stderr, "+ completed '%s' [0]\n", cmd_cd);
    }
}

// pwd using getcwd, with error handling
void pwd()
{
    // Buffer to store the position in the directory
    char dir_name[CMDLINE_MAX];
    getcwd(dir_name, sizeof(dir_name));
    fprintf(stdout, "%s\n", dir_name);
    fprintf(stderr, "+ completed 'pwd' [0]\n");
}

// output redirection
struct cmdline_args output_red(char cmd[CMDLINE_MAX], struct cmdline_args *cmdline_args, int counter)
{
    char* pointer = strtok(cmd, ">");
    cmdline_args -> out_file = strtok(NULL, ">");
    char* filename = strtok(cmdline_args -> out_file, " ");
    cmdline_args -> out_file = filename;
    if (cmdline_args -> out_file == NULL) {
        cmdline_args -> out_error = 1;
        return *cmdline_args;
    }
    if (cmdline_args -> out_file == NULL) {
        fprintf(stderr, "Error: no output file\n");
    }
    cmdline_args -> arguments = strtok(pointer, " ");
    while (cmdline_args -> arguments != NULL) {
        cmdline_args -> exec_args[counter] = cmdline_args -> arguments;
        cmdline_args -> arguments = strtok(NULL, " ");
        counter++;
    }
    cmdline_args -> exec_args[counter] = NULL;
    counter++;
    return *cmdline_args;
}

struct cmdline_args cmd_with_no_pipes(char cmd[CMDLINE_MAX], struct cmdline_args *cmdline_args, int counter)
{
    if (strstr(cmd, ">>"))
    {
        cmdline_args->output_redirect = false;
        cmdline_args->output_append = true;
        return out_append(cmd, cmdline_args, counter);
    }
    else if (strchr(cmd, '>') != NULL)
    {
        cmdline_args->output_redirect = true;
        cmdline_args->output_append = false;
        return output_red(cmd, cmdline_args, counter);
    }
    else
    {
        cmdline_args->output_redirect = false;
        cmdline_args->output_append = false;
        return common_cmd(cmd, cmdline_args, counter);
    }
}

struct cmdline_args common_cmd(char cmd[CMDLINE_MAX], struct cmdline_args *cmdline_args, int counter)
{
    cmdline_args->arguments = strtok(cmd, " ");
    while (cmdline_args->arguments != NULL)
    {
        strcpy(cmdline_args->exec_args[counter], cmdline_args->arguments);
        cmdline_args->arguments = strtok(NULL, " ");
        counter++;
    }
    cmdline_args->exec_args[counter] = NULL;
    counter++;
    return *cmdline_args;
}

struct cmdline_args out_append(char cmd[CMDLINE_MAX], struct cmdline_args *cmdline_args, int counter)
{
    char* pointer = strtok(cmd, ">>");
    cmdline_args -> out_file = strtok(NULL, ">>");
    char* filename = strtok(cmdline_args -> out_file, " ");
    cmdline_args -> out_file = filename;
    if (cmdline_args -> out_file == NULL) {
        cmdline_args -> out_error = 1;
        return *cmdline_args;
    }
    cmdline_args -> arguments = strtok(pointer, " ");
    while (cmdline_args -> arguments != NULL) {
        strcpy(cmdline_args -> exec_args[counter], cmdline_args -> arguments);
        cmdline_args -> arguments = strtok(NULL, " ");
        counter++;
    }
    cmdline_args -> exec_args[counter] = NULL;
    counter++;
    return *cmdline_args;
}

int main(void)
{
    char cmd[CMDLINE_MAX];
    //char cmd[CMDLINE_MAX];
    char cmd_dup[CMDLINE_MAX];
    char cmd_dup2[CMDLINE_MAX];
    char cmd_dup3[CMDLINE_MAX];
    char cmd_cd[CMDLINE_MAX];

    while (1)
    {
        struct cmdline_args *cmdline_args = malloc(sizeof(*cmdline_args));
        struct cmdline_args cargs;
        struct pipeline *pipeline = malloc(sizeof(*pipeline));
        // bool output_redirect = false;
        // bool output_append = false;
        for (size_t i = 0; i < ARGS; i++)
        {
            cmdline_args->exec_args[i] = NULL;
        }

        char *nl;
        // For the file descriptor variable and count for pipeline
        int fd_pipe[2];
        pipeline->count = 1;
        // For the file descriptor variable for output redirection
        int fd_output = 0;

        /* Print prompt */
        printf("sshell@ucd$ ");
        fflush(stdout);

        /* Get command line */
        fgets(cmd, CMDLINE_MAX, stdin);

//        while(cmd[0]==0x0A){
//            printf("sshell@ucd$ ");
//            fflush(stdout);
//            fgets(cmd, CMDLINE_MAX, stdin);
//        }
        char* temp = malloc(sizeof(char*));
        strcpy(temp, trim_whitespace(cmd));
        strcpy(cmd, temp);
        strcpy(cmd_dup, cmd);
        strcpy(cmd_dup2, cmd);
        strcpy(cmd_dup3, cmd);
        strcpy(cmd_cd, cmd);
        free(temp);

        int iterator = 0;
        cmdline_args -> arguments = strtok(cmd_dup, " ");
        while (cmdline_args -> arguments != NULL) {
            cmdline_args -> exec_args[iterator] = cmdline_args -> arguments;
            cmdline_args -> arguments = strtok(NULL, " ");
            iterator++;
        }
        cmdline_args -> exec_args[iterator] = NULL;
        iterator++;

        /* Builtin command */
        if (!strcmp(cmd, "exit")) {
            fprintf(stderr, "Bye...\n");
            fprintf(stderr, "+ completed '%s' [0]\n", cmd);
            break;
        } else if (!strcmp(cmdline_args -> exec_args[0], "cd")) {
            cd(cmdline_args, cmd);
            continue;
        } else if (!strcmp(cmd, "pwd")) {
            pwd();
            continue;
        }



        if ((strchr(cmd, '>') && strchr(cmd, '|')) || (strstr(cmd, ">>") && strchr(cmd, '|'))) {
            if (strcspn(cmd, ">") < strcspn(cmd, "|") || strcspn(cmd, ">>") < strcspn(cmd, "|")) {
                fprintf(stderr, "Error: mislocated output redirection\n");
                continue;
            }
        }

        if (strchr(cmd, '>') || strchr(cmd, '|') || strstr(cmd, ">>")) {
            if (cmd[0] == '>' || cmd[0] == '|') {
                fprintf(stderr, "Error: missing command\n");
                continue;
            }
        }

        if (strchr(cmd_cd, '&')) {
            if (strcspn(cmd_cd, "&") != (strlen(cmd_cd) - 1)) {
                fprintf(stderr, "Error: mislocated background sign\n");
                continue;
            }
            cmdline_args -> bg_jobs = true;
        }

        /* Print command line if stdin is not provided by terminal */
        if (!isatty(STDIN_FILENO))
        {
            printf("%s", cmd);
            fflush(stdout);
        }

        /* Remove trailing newline from command line */
        nl = strchr(cmd, '\n');
        if (nl)
            *nl = '\0';

        // Check the number of pipes present
        for (size_t i = 0; i < strlen(cmd); i++)
        {
            if (cmd[i] == '|')
            {
                pipeline->count++;
            }
        }

        int counter = 0;
        if (pipeline->count == 1)
        {
            cargs = cmd_with_no_pipes(cmd, cmdline_args, counter);
            if (cmdline_args -> out_error == 1) {
                fprintf(stderr, "Error: missing command\n");
                continue;
            }
            // exit(0);
            // int x = 0;
            pid_t pid;
            pipe(fd_pipe);
            for (int iter = 0; iter < pipeline->count; iter++)
            {
                pid = fork();
                if (pid == 0)
                {
                    /* Child */
                    if (cargs.output_redirect == true)
                    {
                        fd_output = open(cargs.out_file, O_WRONLY | O_CREAT | O_TRUNC, 0664);
                        if (fd_output == -1) {
                            fprintf(stderr, "Error: cannot open output file\n");
                            continue;
                        }
                        dup2(fd_output, STDOUT_FILENO);
                        close(fd_output);
                    }
                    if (cargs.output_append == true)
                    {
                        fd_output = open(cargs.out_file, O_WRONLY | O_CREAT | O_APPEND, 0664);
                        dup2(fd_output, STDOUT_FILENO);
                        close(fd_output);
                    }
                    execvp(cargs.exec_args[0], cargs.exec_args);
                    exit(1);
                }
                else if (pid > 0)
                {
                    /* Parent */
                    int status;
                    waitpid(pid, &status, 0);
                    // printf("Child returned %d\n",
                    //        WEXITSTATUS(status));
                    if (status != 0)
                    {
                        fprintf(stderr, "Error: command not found\n");
                        status = 1;
                    }
                    fprintf(stderr, "+ completed '%s' [%d]\n", cmd_cd, status);
                }
                else
                {
                    perror("fork");
                    exit(1);
                }
            }
        }
        else if (pipeline->count == 2)
        {
            char *arg = strchr(cmd_dup2, '|');
            if (arg != NULL)
            {
                arg++;
            }
            // char *command_2 = strtok(arg, " ");
            char *pointer = strtok(cmd_dup2, "|");
            // printf("pointer:/%s/\n", pointer);
            char *trimmed = trim_whitespace(pointer);
            // printf("trimmed after:/%s/\n", trimmed);
            // printf("command_2:/%s/\n", arg);
            // remove_leading_trailing_spaces(arg);
            char *trimmed_2 = trim_whitespace(arg);
            // printf("trimmed_2 after:/%s/\n", trimmed_2);           //
            // exit(0);
            //  while (pointer != NULL)
            //  {
            //      pipeline->array[counter] = pointer;
            //      pointer = strtok(NULL, "|");
            //      // pointer = strtok(pointer, " ");
            //      counter++;
            //  }
            trimmed_2 = strtok(NULL, " "); //
            while (trimmed_2 != NULL)      //
            {
                // printf("trimmed_2: %s\n", trimmed_2);  //
                pipeline->array2[counter] = trimmed_2; //
                trimmed_2 = strtok(NULL, " ");         //
                // pointer = strtok(pointer, " ");
                counter++;
            }
            int counter2 = 0;
            trimmed = strtok(trimmed, " ");
            while (trimmed != NULL)
            {
                // printf("trimmed:/%s/\n", trimmed);
                pipeline->array[counter2] = trimmed;
                trimmed = strtok(NULL, " ");
                // pointer = strtok(pointer, " ");
                counter2++;
            }
            // exit(0);
            // for (int i = 0; i < ARGS; i++)
            // {
            //     printf("Pipeline Array in line 438[%d]: %s\n", i, pipeline->array[i]);
            // }
            // for (int i = 0; i < ARGS; i++)
            // {
            //     printf("Pipeline Array2 in line 478[%d]: %s\n", i, pipeline->array2[i]);
            // }
            // pipelining(pipeline->count, pipeline, pipeline);
            // printf("Inside pipelining\n");
            // printf("pipeline->count: %d\n", pipeline->count);
            int fd_pipe[2];
            // pid_t pid1, pid2;
            pid_t pid1;
            pipe(fd_pipe);
            pid_t pidARray[2];
            pid1 = fork();

            pidARray[0] = pid1;
            if (pid1 > 0)
            {
            }
            if (pid1 == 0)
            {
                /* Set the process output to the input of the pipe. */
                // close(1);
                close(fd_pipe[0]);
                dup2(fd_pipe[1], STDOUT_FILENO);
                close(fd_pipe[1]);
                // argv[0] = (char *)malloc(5 * sizeof(char));
                // argv[1] = (char *)malloc(5 * sizeof(char));
                // strcpy(argv[0], "ls");
                // strcpy(argv[1], "-l");
                // argv[2] = NULL;
                // fprintf(stderr, "************* Running ls -l *************\n");
                // printf("pipeline_cmd1->array[0]: %s\n", pipeline_cmd1->array[0]);
                // for (size_t i = 0; i < sizeof(pipeline->array); i++)
                // {
                //     printf("pipeline->array[%zu]: %s\n", i, pipeline->array[i]);
                // }
                execvp(pipeline->array[0], pipeline->array);
                // perror("First execvp() failed");
                //  return -1;
            }
            // pid2 = fork();
            pid1 = fork();
            pidARray[1] = pid1;
            if (pid1 > 0)
            {
            }
            if (pid1 == 0)
            {
                /* Set the process input to the output of the pipe. */
                close(fd_pipe[1]);
                dup2(fd_pipe[0], STDIN_FILENO);
                close(fd_pipe[0]);

                // argv[0] = (char *)malloc(5 * sizeof(char));
                // argv[1] = (char *)malloc(5 * sizeof(char));
                // strcpy(argv[0], "grep");
                // strcpy(argv[1], "pipe");
                // argv[2] = NULL;
                // fprintf(stderr, "************* Running grep pipe *************\n");
                // for (size_t i = 0; i < sizeof(pipeline->array2); i++)
                // {
                //     printf("pipeline->array2[%zu]: %s\n", i, pipeline->array2[i]);
                // }
                execvp(pipeline->array2[0], pipeline->array2);
                // perror("Second execvp() failed");
                //  return -1;
            }
            close(fd_pipe[0]);
            close(fd_pipe[1]);
            /* Wait for the children to finish, then exit. */
            int status;
            int status_arr[2];
            for (int i = 0; i < 2; i++)
            {
                waitpid(pidARray[i], &status, 0);
                status_arr[i] = status;
            }

            fprintf(stderr, "+ completed '%s' ", cmd_cd);
            for (int i = 0; i < 2; i++) {
                if (status_arr[i] != 0)
                {
                    fprintf(stderr, "Error: command not found\n");
                    status = 1;
                }
                fprintf(stderr, "[%d]", status_arr[i]);
            }
            fprintf(stderr, "\n");
            // waitpid(pid2, NULL, 0);
        }
    }
}

void remove_garbage(char* cmd) {
    int read = 0, write = 0;
    while (cmd[read]) {
        if ((cmd[read]) != '!') {
            cmd[write++] = cmd[read];
        }
        read++;
    }
    cmd[write] = 0;
}
char* trim_whitespace(char* cmd) {
    while(isspace((unsigned char)* cmd)) {
        *cmd = *cmd + 1;
        remove_garbage(cmd);
    }
    char* end;
    end = cmd + strlen(cmd) - 1;
    while (end > cmd && (isspace((unsigned char)* end))) {
        end--;
    }
    end[1] = '\0';
    return cmd;
}
