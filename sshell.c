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

struct args
{
    char *exec_args[ARGS];
};

struct args argument;
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
    char Cmdcd[CMDLINE_MAX];

    while (1)
    {
        bool output_redirect = false;
        char *nl;
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

        strcpy(cmddup1, cmd);
        strcpy(cmddup2, cmd);
        strcpy(cmddup3, cmd);
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
        pid_t pid;
        pid = fork();
        if (pid > 0)
        {
            waitpid(pid, &status, 0);
            /*Parent*/
        }
        else if (pid == 0)
        {
            // printf("CMddup1:/%s/\n", cmddup1);
            if (strchr(cmd, '>') != NULL)
            {
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

        /* Regular command */
        // retval = system(cmd);
        // fprintf(stdout, "Return status value for '%s': %d\n",
        //         cmd, retval);
    }

    return EXIT_SUCCESS;
}
