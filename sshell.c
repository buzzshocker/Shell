#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define ARGS 16
#define CMDLINE_MAX 512

struct cmdline_args{
    char *exec_args[ARGS];
    char *arguments;
};

void pwd()
{
    // Buffer to store the position in the directory
    char dir_name[CMDLINE_MAX];
    fprintf(stdout, "%s\n", getcwd(dir_name, sizeof(dir_name)));
    fprintf(stderr, "+ completed 'pwd' [0]\n");
}

void cd(struct cmdline_args cargs, char *cmd_cd)
{
    // If directory can't be entered, display error message and print to stder
    // printf("aa%s", cargs.exec_args[1]);
    if (chdir(cargs.exec_args[1]) != 0)
    {
        fprintf(stderr, "Error: cannot cd into directory\n");
        fprintf(stderr, "+ completed '%s' [1]\n", cmd_cd);
    }
    else
    {
        fprintf(stderr, "+ completed '%s' [0]\n", cmd_cd);
    }
}

int main(void)
{
    char cmd[CMDLINE_MAX];
    char cmd_dup[CMDLINE_MAX];
    char cmd_dup2[CMDLINE_MAX];
    char cmd_dup3[CMDLINE_MAX];

    while (1) {
        for(size_t i = 0; i < CMDLINE_MAX; i++){
            cmd[i] = '\0';
        }
        char *nl;
        // int retval = 0;
        struct cmdline_args cargs;

        for (size_t i = 0; i < ARGS; i++){
            cargs.exec_args[i] = NULL;
        }
        int status = 0;

        /* Print prompt */
        printf("sshell$ ");
        fflush(stdout);

        /* Get command line */
        fgets(cmd, CMDLINE_MAX, stdin);
        // printf("CMD: %lu", strlen(cmd));
        cmd[strlen(cmd) - 1] = '\0';

        strcpy(cmd_dup, cmd);
        strcpy(cmd_dup2, cmd);
        strcpy(cmd_dup3, cmd);

        /* Print command line if stdin is not provided by terminal */
        if (!isatty(STDIN_FILENO)) {
            printf("%s", cmd);
            fflush(stdout);
        }

        /* Remove trailing newline from command line */
        nl = strchr(cmd, '\n');
        if (nl)
            *nl = '\0';

        /* Builtin command */
        if (!strcmp(cmd, "exit")) {
            fprintf(stderr, "Bye...\n");
            fprintf(stderr, "+ completed 'exit' [0]\n");
            break;
        }

        if (!strcmp(cmd_dup, "pwd")){
            pwd();
            continue;
        }

        int counter = 0;
        // strcpy(cmd_dup , "echo hi");
        char *token = strtok(cmd_dup," ");

        // loop until strtok() returns NULL
        while (token)  {

            // print token
            // printf("Token: /%s/\n", token);
            cargs.exec_args[counter] = token;

            // take subsequent tokens
            token = strtok(NULL," ");
            ++counter;
        }
        cargs.exec_args[counter] = NULL;
        ++counter;

        if (strstr(cmd_dup2, "cd") != NULL){
            cd(cargs, cmd);
            continue;
        }

        // printf("Counter: %d\n", counter);
        // for (int i = 0; i < counter; i++){
        //     printf("1: %s\n", cargs.exec_args[i]);
        // }

        /* Regular command */
        pid_t pid = fork();
        if (pid > 0){
            waitpid(pid, &status, 0);
            // parent

        }
        else if (pid == 0){
            // child
            // cargs.exec_args[0] = "echo";
            // cargs.exec_args[1] = "hi";
            // cargs.exec_args[2] = NULL;
            execvp(cargs.exec_args[0], cargs.exec_args);
            exit(1);
        }
        else {
            perror("fork");
            exit(1);
        }


        fprintf(stderr, "+ completed '%s' [%d]\n",
                cmd, status);
    }

    return EXIT_SUCCESS;
}
