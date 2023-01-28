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

    while (1)
    {
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

        /* Builtin command */
        if (!strcmp(cmd, "exit"))
        {
            fprintf(stderr, "Bye...\n");
            break;
        }

        pid_t pid;
        int status;
        pid = fork();
        if (pid > 0)
        {
            waitpid(pid, &status, 0);
            /*Parent*/
        }
        else if (pid == 0)
        {
            // printf("CMddup1:/%s/\n", cmddup1);
            ParsingFunc(cmddup1);
            /*Child*/
            execvp(argument.exec_args[0], argument.exec_args);
            perror("execvp");
            exit(1);
        }

        /* Regular command */
        // retval = system(cmd);
        // fprintf(stdout, "Return status value for '%s': %d\n",
        //         cmd, retval);
    }

    return EXIT_SUCCESS;
}
