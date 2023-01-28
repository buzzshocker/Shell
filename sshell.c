/// Sources cited -
// CITATIONS
// https://codeforwin.org/c-programming/c-program-to-trim-trailing-white-space-characters-in-string
// https://javatutoring.com/c-program-trim-leading-and-trailing-white-space-characters-from-string/

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
}

int main(void)
{
    char cmd[CMDLINE_MAX];
    char cmddup1[CMDLINE_MAX];
    char cmddup2[CMDLINE_MAX];
    char cmddup3[CMDLINE_MAX];
    char Cmdcd[CMDLINE_MAX];
    char Cmd_count[CMDLINE_MAX];
    char Cmd_pipe[CMDLINE_MAX];

    while (1)
    {
        bool output_redirect = false;
        bool output_append = false;
        char *nl;
        // int retval;

        /* Print prompt */
        printf("sshell@ucd$ ");
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
        strcpy(Cmd_count, cmd);
        strcpy(Cmd_pipe, cmd);
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
            split = strtok(NULL, " ");
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

        int iterator = 0;
        char* count_parse = strtok(Cmd_count, " ");
        while (count_parse != NULL) {
            count_parse = strtok(NULL, " ");
            iterator++;
        }

        if (iterator > ARGS) {
            fprintf(stderr, "Error: too many process arguments\n");
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

        char *filename;

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
                if (filename == NULL) {
                    fprintf(stderr, "Error: no output file\n");
                    continue;
                }
                ParsingFunc(cga);
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
                char *Comm = strtok(cmddup2, ">");
                if (filename == NULL) {
                    fprintf(stderr, "Error: no output file\n");
                    continue;
                }
                ParsingFunc(Comm);
            }
            if (output_redirect == true)
            {
                fd_output = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0664);
                if (fd_output == -1) {
                    fprintf(stderr, "Error: cannot open output file\n");
                    continue;
                }
                dup2(fd_output, STDOUT_FILENO);
                close(fd_output);
            }
            else if (output_append == true)
            {
                fd_output = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0664);
                if (fd_output == -1) {
                    fprintf(stderr, "Error: cannot open output file\n");
                    continue;
                }
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

    return EXIT_SUCCESS;
}
