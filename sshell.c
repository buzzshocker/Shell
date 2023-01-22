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

        // splitting
        int r = 0;
        char str[512] = "";
        int index = 0;
        while (cmd[r] != ' ')
        {
            str[index] = cmd[r];
            r++;
            index++;
        }
        printf("str:%s\n", str);

        char argv[512];

        argv[0] = '\0';
        r++;
        int i = 0;
        while (cmd[r] != '\0')
        {
            argv[i] = cmd[r];
            argv[i + 1] = '\0';
            r++;
            i++;
        }
        // return 1;
        printf("argv[0]: %c\n", argv[0]);

        printf("cmd: %s\n", cmd);

        pid_t pid;

        // char p[512];
        // p[0] = '\0';

        char *args[] = {str, argv, NULL};

        printf("args[0]: %s\n", args[0]);
        printf("args[1]: %s\n", args[1]);
        // printf("args[2]: %s\n", args[2]);

        // for (size_t i = 0; argv[i] != '\0'; i++)
        // {
        //     printf("%c\n", argv[i]);
        // }

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
        // strcpy(argv, "");
        argv[0] = '\0';
        // int args_sz = sizeof args / sizeof args[0];
        // for (int i = 0; i < args_sz; i++)
        // {
        //     args[i] = '\0';
        // }
        // args = {};
        // memset(argv, 0, sizeof(argv));

        printf("\nPRINTING\n");
        // printf("argv[0]: %c\n", argv[0]);
        // printf("argv[1]: %c\n", argv[1]);
        // for (size_t i = 0; argv[i] != '\0'; i++)
        // {
        //     printf("%c\n", argv[i]);
        // }

        // /* Regular command */
        // retval = system(cmd);
        // fprintf(stdout, "Return status value for '%s': %d\n",
        //         cmd, retval);
    }

    return EXIT_SUCCESS;
}