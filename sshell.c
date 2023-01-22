#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define CMDLINE_MAX 512

int main(void)
{
    char cmd[CMDLINE_MAX];

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

        /* Builtin command */
        if (!strcmp(cmd, "exit"))
        {
            fprintf(stderr, "Bye...\n");
            break;
        }
        char str[512];
        char argv[512];
        char *token = strtok(cmd, " ");
        int count = 0;
        while (token != 0)
        {
            count++;
            if (count == 1)
            {
                strcpy(str, token);
            }
            if (count == 2)
            {
                strcpy(argv, token);
            }
            if (count > 2)
            {
                strcat(argv, " ");
                strcat(argv, token);
            }
            token = strtok(0, " ");
        }

        pid_t pid;

        char *args[512] = {NULL, NULL, NULL};
        if (count == 1)
        {
            args[0] = str;
            args[1] = NULL;
        }
        else if (count > 1)
        {
            args[0] = str;
            args[1] = argv;
            args[2] = NULL;
        }

        pid = fork();
        if (pid == 0)
        {
            /* Child */
            execvp(args[0], args);
            perror("execvp");
            exit(1);
        }
        else if (pid > 0)
        {
            /* Parent */
            int status;
            waitpid(pid, &status, 0);
            // printf("Child returned %d\n",
            //        WEXITSTATUS(status));
        }
        else
        {
            perror("fork");
            exit(1);
        }
        strcpy(str, "");
        argv[0] = '\0';

        // /* Regular command */
        // retval = system(cmd);
        // fprintf(stdout, "Return status value for '%s': %d\n",
        //         cmd, retval);
    }

    return EXIT_SUCCESS;
}