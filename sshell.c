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
struct cmdline_args {
    // Stores the arguments as a single string to be passed as the argument
    char* arguments;
    // Stores the command passed
    char command[CMDLINE_MAX];
    // Stores the command and arguments to be passed to execvp
    char* exec_args[ARGS];
    // To store the output file name in case of output redirection
    char* out_file;
};

// Pipeline handling struct
struct pipeline {
    // Stores the number of pipes in the input
    int count;
    // Stores the inputs from each pipe
    char* array;
};

// cd using chdir, with error handling
void cd(struct cmdline_args* cmdline_args) {
    // If directory can't be entered, display error message and print to stderr
    if (chdir(cmdline_args -> exec_args[1]) != 0) {
        fprintf(stderr, "Error: cannot cd into directory\n");
        printf("+ completed '%s' [1]\n", cmdline_args -> exec_args[0]);
    }
    else {
        printf("+ completed '%s' [0]\n", cmdline_args -> exec_args[0]);
    }
}

// pwd using getcwd, with error handling
void pwd() {
    // Buffer to store the position in the directory
    char dir_name[CMDLINE_MAX];
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
    char cmd_dup[CMDLINE_MAX];

    while (1) {
        struct cmdline_args* cmdline_args = malloc(sizeof(*cmdline_args));
        struct pipeline* pipeline = malloc(sizeof(pipeline));
        bool output_redirect = false;
        bool output_append = false;

        for (size_t i = 0; i < ARGS; i++) {
            cmdline_args->exec_args[i]= NULL;
        }

        char *nl;
        // For the file descriptor variable and count for pipeline
        int fd_pipe[2];
        pipeline -> count = 1;
        // For the file descriptor variable for output redirection
        int fd_output = 0;

        /* Print prompt */
        printf("sshell@ucd$ ");
        fflush(stdout);

        /* Get command line */
        //bool gt_cmd = false;
        fgets(cmd, CMDLINE_MAX, stdin);
        //char ch = fgets(cmd, CMDLINE_MAX, stdin);
        while(cmd[0]==0x0A){
            printf("sshell@ucd$ ");
            fflush(stdout);
            fgets(cmd, CMDLINE_MAX, stdin);
        }
        strcpy(cmd_dup, cmd);

        /* Print command line if stdin is not provided by terminal */
        if (!isatty(STDIN_FILENO)) {
            printf("%s", cmd);
            fflush(stdout);
        }

        /* Remove trailing newline from command line */
        nl = strchr(cmd, '\n');
        if (nl)
            *nl = '\0';

        // Check the number of pipes present
        for (size_t i = 0; i < strlen(cmd); i++) {
            if (cmd[i] == '|') {
                pipeline -> count++;
            }
        }

        // Tokenize the input from the command line and classify them as
        // arguments or commands
        int counter = 0;
        if (strstr(cmd, ">>")) {
            output_append = true;
            char* pointer = strtok(cmd, ">>");
            cmdline_args -> out_file = strtok(NULL, ">>");
            cmdline_args -> arguments = strtok(pointer, " ");
            printf("%s",cmdline_args -> arguments);
            while (cmdline_args -> arguments != NULL) {
                cmdline_args -> exec_args[counter] = cmdline_args -> arguments;
                cmdline_args -> arguments = strtok(NULL, " ");
                counter++;
            }
            cmdline_args -> exec_args[counter] = NULL;
            counter++;
        } else if (strchr(cmd, '>') != NULL) {
            output_redirect = true;
            char* pointer = strtok(cmd, ">");
            cmdline_args -> out_file = strtok(NULL, ">");
            cmdline_args -> arguments = strtok(pointer, " ");
            printf("%s",cmdline_args -> arguments);
            while (cmdline_args -> arguments != NULL) {
                cmdline_args -> exec_args[counter] = cmdline_args -> arguments;
                cmdline_args -> arguments = strtok(NULL, " ");
                counter++;
            }
            cmdline_args -> exec_args[counter] = NULL;
            counter++;
        } else {
            cmdline_args -> arguments = strtok(cmd, " ");
            while (cmdline_args -> arguments != NULL) {
                cmdline_args -> exec_args[counter] = cmdline_args -> arguments;
                cmdline_args -> arguments = strtok(NULL, " ");
                counter++;
            }
            cmdline_args -> exec_args[counter] = NULL;
            counter++;
        }

        // If more than 16 arguments passed, display error message
        if (counter > ARGS) {
            fprintf(stderr, "Error: too many process arguments\n");
            continue;
        }

        /* Builtin command */
        if (!strcmp(cmd_dup, "exit")) {
            fprintf(stderr, "Bye...\n");
            printf("+ completed '%s' [0]\n", cmdline_args -> exec_args[0]);
            break;
        } else if (!strcmp(cmdline_args -> exec_args[0], "cd")) {
            cd(cmdline_args);
            continue;
        } else if (!strcmp(cmdline_args -> exec_args[0], "pwd")) {
            pwd();
            continue;
        } else if (!strcmp(cmdline_args -> exec_args[0], "")) {
            fprintf(stderr, "Error: command not found or invalid argument\n");
            continue;
        }

        if (output_redirect || output_append) {
            char* break_cmd = strtok(cmdline_args -> out_file, " ");
            //printf("%s\n", break_cmd);
            cmdline_args -> out_file = break_cmd;
        }

        for (int i = 0; i < pipeline -> count; i++) {

            pid_t pid;
            pid = fork();
            pipe(fd_pipe);
            if (pid == 0) {
                /* Child */
                if (output_redirect == true) {
                    fd_output = open(cmdline_args -> out_file, O_WRONLY | O_CREAT | O_TRUNC, 0664);
                    dup2(fd_output, STDOUT_FILENO);
                    close(fd_output);
                } else if (output_append == true){
                    fd_output = open(cmdline_args -> out_file, O_WRONLY | O_CREAT | O_APPEND, 0664);
                    dup2(fd_output, STDOUT_FILENO);
                    close(fd_output);
                }
//                close(fd_pipe[1]);
//                dup2(fd_pipe[0], STDIN_FILENO);
//                close(fd_pipe[0]);
                execvp(cmdline_args -> exec_args[0], cmdline_args -> exec_args);
                //perror("execvp");
                exit(1);
            } else if (pid > 0) {
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
        }
        strcpy(cmdline_args -> command, "");
        free(cmdline_args);
        free(pipeline);
    }

    return EXIT_SUCCESS;
}

