#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define CMDLINE_MAX 512

// cd system call using chdir, with error handling
void cd(char argv[512], char command[512]) {
    // If directory can't be entered, display error message
    if (chdir(argv) != 0) {
        printf("Error: cannot cd into directory\n");
        printf("+ completed '%s' [1]\n", command);
    }
    else {
        printf("+ completed '%s' [0]\n", command);
    }
}

int main(void) {
    char cmd[CMDLINE_MAX];

    while (1) {
        char *nl;
        // int retval;

        /* Print prompt */
        printf("sshell$ ");
        fflush(stdout);

        /* Get command line */
        fgets(cmd, CMDLINE_MAX, stdin);

        /* Print command line if stdin is not provided by terminal */
        if (!isatty(STDIN_FILENO)) {
            printf("%s", cmd);
            fflush(stdout);
        }

        /* Remove trailing newline from command line */
        nl = strchr(cmd, '\n');
        if (nl)
            *nl = '\0';

        // Tokenize the input from the command line and classify them as
        // arguments or commands
        char command[512];
        char argv[512];
        char *token = strtok(cmd, " ");
        int count = 0;
        while (token != 0) {
            count++;
            // If only one token, there is only a command without any arguments
            if (count == 1) {
                strcpy(command, token);
            }  // More than 1 indicates presence of arguments
            if (count == 2) {
                strcpy(argv, token);
            }
            if (count > 2) {
                strcat(argv, " ");
                strcat(argv, token);
            }
            token = strtok(0, " ");
        }

        // If more than 16 arguments passed, display error message
        if (count > 16) {
            printf("Error: too many process arguments\n");
            continue;
        }

        /* Builtin command */
        if (!strcmp(cmd, "exit")) {
            fprintf(stderr, "Bye...\n");
            printf("+ completed '%s' [0]\n", command);
            break;
        } else if (!strcmp(command, "cd")) {
            cd(argv, command);
            continue;
        }

        pid_t pid;

        // Format the input as it should be for the exec command
        char *args[512] = {NULL, NULL, NULL};
        if (count == 1) {
            args[0] = command;
            args[1] = NULL;
        }
        else if (count > 1) {
            args[0] = command;
            args[1] = argv;
            args[2] = NULL;
        }

        pid = fork();
        if (pid == 0) {
            /* Child */
            execvp(args[0], args);
            //perror("execvp");
            exit(1);
        }
        else if (pid > 0) {
            /* Parent */
            int status;
            waitpid(pid, &status, 0);
            // printf("Child returned %d\n",
            //        WEXITSTATUS(status));
            if (status != 0) {
                printf("Error: command not found\n");
                status = 1;
            }
            printf("+ completed '%s' [%d]\n", cmd, status);
        }
        else {
            perror("fork");
            exit(1);
        }
        strcpy(command, "");
        argv[0] = '\0';

        // /* Regular command */
        // retval = system(cmd);
        // fprintf(stdout, "Return status value for '%s': %d\n",
        //         cmd, retval);
    }

    return EXIT_SUCCESS;
}