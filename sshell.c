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

struct pipe
{
    // Stores the number of pipes in the input
    int count;
    // Stores the inputs from each pipe
    char *array[ARGS];
    char *array2[ARGS];
};

struct args
{
    char *exec_args[ARGS];
};

struct args argument;
struct pipe pipeline;
void ParsingFunc(char *dup)
{
    // printf("Inside PArsing\n");
    // printf("dup:/%s/\n", dup);
    int counter = 0;
    char *word = strtok(dup, " ");
    while (word != NULL)
    {
        // printf("word:/%s/\n", word);
        argument.exec_args[counter] = word;
        // printf("/%s/\n", argument.exec_args[counter]);
        word = strtok(NULL, " ");
        counter++;
    }
    argument.exec_args[counter] = NULL;
    counter++;
    // for (int i = 0; i < counter; i++)
    // {
    //     printf("argument.exec_args[%d]:/%s/\n", i, argument.exec_args[i]);
    // }
}

int main(void)
{
    char cmd[CMDLINE_MAX];
    char cmddup1[CMDLINE_MAX];
    char cmddup2[CMDLINE_MAX];
    char cmddup3[CMDLINE_MAX];
    char cmddup4[CMDLINE_MAX];
    // char cmddup5[CMDLINE_MAX];
    char Cmdcd[CMDLINE_MAX];

    while (1)
    {
        bool output_redirect = false;
        bool output_append = false;
        char *nl;
        pipeline.count = 1;
        // int retval;

        /* Print prompt */
        printf("sshell$ ");
        fflush(stdout);

        /* Get command line */
        fgets(cmd, CMDLINE_MAX, stdin);

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
                pipeline.count++;
            }
        }
        printf("pipecount: %d\n", pipeline.count);

        strcpy(cmddup1, cmd);
        strcpy(cmddup2, cmd);
        strcpy(cmddup3, cmd);
        strcpy(cmddup4, cmd);
        strcpy(Cmdcd, cmd);
        // int fd_pipe[2];
        int fd_output = 0;

        /* Builtin command */
        if (!strcmp(cmd, "exit"))
        {
            fprintf(stderr, "Bye...\n");
            fprintf(stderr, "+ completed 'exit' [0]\n");
            break;
        }
        if (strstr(cmd, "cd") != NULL)
        {
            char *split = strtok(Cmdcd, " ");

            // printf("split1:/%s/\n", split);
            split = strtok(NULL, " ");

            // printf("split2:/%s/\n", split);

            if (chdir(split) != 0)
            {
                fprintf(stderr, "Error: cannot cd into directory\n");
                fprintf(stderr, "+ completed '%s' [1]\n", cmd);
            }
            else
            {
                fprintf(stderr, "+ completed '%s' [0]\n", cmd);
            }
            continue;
        }
        char *filename;

        // if (cargs.output_append == true)
        // {
        //     fd_output = open(cargs.out_file, O_WRONLY | O_CREAT | O_APPEND, 0664);
        //     dup2(fd_output, STDOUT_FILENO);
        //     close(fd_output);
        // }
        // if (strstr(cmd, "pwd") != NULL)
        // {
        //     // Buffer to store the position in the directory
        //     char dir_name[CMDLINE_MAX];
        //     fprintf(stdout, "%s\n", getcwd(dir_name, sizeof(dir_name)));
        //     fprintf(stderr, "+ completed 'pwd' [0]\n");
        // }

        int status = 0;
        int counter = 0;
        if (pipeline.count == 1)
        {
            pid_t pid;
            for (int iter = 0; iter < pipeline.count; iter++)
            {
                pid = fork();
                if (pid > 0)
                {
                    waitpid(pid, &status, 0);
                    /*Parent*/
                }
                else if (pid == 0)
                {
                    // printf("CMddup1:/%s/\n", cmddup1);
                    if (strstr(cmd, ">>") != NULL)
                    {
                        output_redirect = false;
                        output_append = true;

                        char *Frontcommand = strtok(cmddup3, ">>");
                        char file_name[CMDLINE_MAX];
                        char cga[CMDLINE_MAX];
                        strcpy(cga, Frontcommand);
                        Frontcommand = strtok(NULL, " ");
                        strcpy(file_name, Frontcommand);
                        filename = strtok(NULL, " ");
                        ParsingFunc(cga);

                        printf("ActualFN: /%s/\n", file_name);
                        for (int i = 0; i < 2; i++)
                        {
                            printf("argument.exec_args[%d]:/%s/\n", i, argument.exec_args[i]);
                        }

                        // printf("strstr(cmd, >> ): %ld\n", strstr(cmd, ">>") - cmd);
                        // char *arg = strstr(cmd, ">>");
                        // printf("ARG1: %s\n", arg);
                        // if (arg != NULL)
                        // {
                        //     arg++;
                        // }
                        // arg++;
                        // printf("ARG2: %s\n", arg);
                        // filename = strtok(arg, " ");
                        // char *firstCommand = NULL;
                        // for (int i = 0; i < strstr(cmd, ">>") - cmd; i++)
                        // {
                        //     firstCommand[i] = cmd[i];
                        // }
                        // printf("FC /%s/\n", firstCommand);
                        // printf(" /%s/\n", filename);

                        // arg++;
                        // char cga[CMDLINE_MAX];
                        // strcpy(cga, arg);
                        // char *Frontcommand = strtok(cmddup3, ">>");
                        // filename = strtok(arg, " ");
                        // char *ActualFN = filename;
                        // filename = strtok(NULL, " ");
                        // ParsingFunc(Frontcommand);

                        // printf("ActualFN: /%s/\n", ActualFN);
                        // for (int i = 0; i < 2; i++)
                        // {
                        //     printf("argument.exec_args[%d]:/%s/\n", i, argument.exec_args[i]);
                        // }
                    }
                    else if (strchr(cmd, '>') != NULL)
                    {
                        output_append = false;
                        output_redirect = true;
                        char *arg = strchr(cmd, '>');
                        if (arg != NULL)
                        {
                            arg++;
                        }
                        filename = strtok(arg, " ");
                        // printf("FN /%s/\n", filename);
                        char *Comm = strtok(cmddup2, ">");
                        // printf("Cmd: /%s/\n", Comm);

                        ParsingFunc(Comm);
                        // for (int i = 0; i < 2; i++)
                        // {
                        //     printf("argument.exec_args[%d]:/%s/\n", i, argument.exec_args[i]);
                        // }
                    }
                    if (output_redirect == true)
                    {
                        // printf("Inside\n");
                        // printf("filename in out_red:/%s/\n", filename);
                        fd_output = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0664);
                        dup2(fd_output, STDOUT_FILENO);
                        close(fd_output);
                    }
                    else if (output_append == true)
                    {
                        // printf("Inside\n");
                        // printf("filename in out_red:/%s/\n", filename);
                        fd_output = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0664);
                        dup2(fd_output, STDOUT_FILENO);
                        close(fd_output);
                    }
                    else
                    {
                        ParsingFunc(cmddup1);
                    }
                    /*Child*/
                    execvp(argument.exec_args[0], argument.exec_args);
                    perror("execvp");
                    exit(1);
                }
                else
                {
                }
                fprintf(stderr, "+ completed '%s' [%d]\n", cmd, status);
            }
        }
        else if (pipeline.count == 2)
        {
            char *arg = strchr(cmddup4, '|');
            if (arg != NULL)
            {
                arg++;
            }
            // char *command_2 = strtok(arg, " ");
            // printf("arg: %s\n", arg);
            char *pointer = strtok(cmddup4, "|");
            // printf("arg: %s\n", arg);
            printf("pointer in line 296:/%s/\n", pointer);
            char *trimmed; // = remove_leading_trailing_spaces(pointer);
            // printf("trimmed after:/%s/\n", trimmed);
            //  printf("command_2:/%s/\n", arg);
            //  remove_leading_trailing_spaces(arg);
            char *trimmed_2; // = remove_leading_trailing_spaces(arg); //
            // printf("trimmed_2 after:/%s/\n", trimmed_2);

            trimmed_2 = strtok(arg, " "); //
            while (trimmed_2 != NULL)     //
            {
                // printf("trimmed_2: %s\n", trimmed_2);  //
                pipeline.array2[counter] = trimmed_2; //
                trimmed_2 = strtok(NULL, " ");        //
                // pointer = strtok(pointer, " ");
                counter++;
            }
            int counter2 = 0;
            // printf("trimmed_2 line 314:/%s/\n", trimmed_2);
            trimmed = strtok(pointer, " ");
            while (trimmed != NULL)
            {
                // printf("trimmed:/%s/\n", trimmed);
                pipeline.array[counter2] = trimmed;
                trimmed = strtok(NULL, " ");
                // pointer = strtok(pointer, " ");
                counter2++;
            }
            // exit(0);
            for (int i = 0; i < ARGS; i++)
            {
                printf("Pipeline Array in line 438[%d]: %s\n", i, pipeline.array[i]);
            }
            for (int i = 0; i < ARGS; i++)
            {
                printf("Pipeline Array2 in line 478[%d]: %s\n", i, pipeline.array2[i]);
            }
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
                execvp(pipeline.array[0], pipeline.array);
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
                execvp(pipeline.array2[0], pipeline.array2);
                // perror("Second execvp() failed");
                //  return -1;
            }

            close(fd_pipe[0]);
            close(fd_pipe[1]);
            /* Wait for the children to finish, then exit. */
            for (int i = 0; i < 2; i++)
            {
                int status;

                waitpid(pidARray[i], &status, 0);
            }
        }
    }

    return EXIT_SUCCESS;
}
