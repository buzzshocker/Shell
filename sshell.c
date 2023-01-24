#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define CMDLINE_MAX 512

// Struct for handling command line arguments
struct input {
    // Stores the arguments as a single string to be passed as the argument
    char argv[512];
    // Stores the command passed
    char command[512];
    // Stores the command and arguments to be passed to execvp
    char* args[512];
    // To store the output file name in case of output redirection
    char out_file[512];
};

// cd using chdir, with error handling
void cd(struct input* input) {
    // If directory can't be entered, display error message and print to stderr
    if (chdir(input -> argv) != 0) {
        fprintf(stderr, "Error: cannot cd into directory\n");
        printf("+ completed '%s' [1]\n", input -> command);
    }
    else {
        printf("+ completed '%s' [0]\n", input -> command);
    }
}

// pwd using getcwd, with error handling
void pwd() {
    // Buffer to store the position in the directory
    char dir_name[512];
    if (getcwd(dir_name, sizeof(dir_name))) {
        printf("%s\n", dir_name);
        printf("+ completed 'pwd' [0]\n");
    } else {
        perror("getcwd()");
        printf("+ completed 'pwd' [1]\n");
    }
}

int main(void) {
    char cmd[CMDLINE_MAX];

    while (1) {
        char *nl;
        // int retval;
        // For the file descriptor variable
        int fd = 0;


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
        struct input* input = malloc(sizeof(input));
        char *token = strtok(cmd, " ");
        int count = 0;
        while (token != 0) {
            count++;
            // If only one token, there is only a command without any arguments
            if (count == 1) {
                strcpy(input -> command, token);
            }  // More than 1 indicates presence of arguments
            if (count == 2) {
                strcpy(input -> argv, token);
            }
            if (count > 2) {
                strcat(input -> argv, " ");
                strcat(input -> argv, token);
            }
            token = strtok(0, " ");
        }

        // If more than 16 arguments passed, display error message
        if (count > 16) {
            fprintf(stderr, "Error: too many process arguments\n");
            continue;
        }

        /* Builtin command */
        if (!strcmp(cmd, "exit")) {
            fprintf(stderr, "Bye...\n");
            printf("+ completed '%s' [0]\n", input -> command);
            break;
        } else if (!strcmp(input -> command, "cd")) {
            cd(input);
            continue;
        } else if (!strcmp(input -> command, "pwd")) {
            pwd();
            continue;
        }

        pid_t pid;

        // Format the input as it should be for the exec command
        input -> args[0] = NULL;  // Holds the command
        input -> args[1] = NULL;  // Holds the arguments, if any
        input -> args[2] = NULL;  // Holds the NULL delimiter to signify
                                    // end of input
        if (count == 1) {  // If only one token, then there is just a command
            input -> args[0] = input -> command;
            input -> args[1] = NULL;
        }
        else if (count > 1) {  // If more than one, arguments also present
            input -> args[0] = input -> command;
            input -> args[1] = input -> argv;
            input -> args[2] = NULL;
        }

        for (size_t i = 0; i < strlen(input -> argv); i++) {
            printf("%c", input -> argv[i]);
        }
        pid = fork();
        if (pid == 0) {
            /* Child */
            int x = 0;
            while (input -> argv[x]) {
                if (input -> argv[x] == '>') {

                }
            }
            fd = open("input -> out_file", O_WRONLY | O_CREAT | O_TRUNC, 0664);
            dup2(fd, STDOUT_FILENO);
            close(fd);
            }
            execvp(input -> args[0], input -> args);
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
                fprintf(stderr, "Error: command not found or invalid argument\n");
                status = 1;
            }
            printf("+ completed '%s' [%d]\n", cmd, status);
        }
        else {
            perror("fork");
            exit(1);
        }
        strcpy(input -> command, "");
        input -> argv[0] = '\0';
    }

    return EXIT_SUCCESS;
}

