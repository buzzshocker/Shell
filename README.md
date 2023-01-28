# ecs150_proj1

We downloaded the starter files and then created the Makefile in Phase 0. My partner and I worked on every single phase side by side together. To begin with, we created a struct called 'struct cmdline_args' that holds all the command and the arguments passed. We then decided to have 2 structs, one for the piping arguments(pipeline) and another one(cmdline_args) for all the other command line arguments. The cmdline_args struct has char *arguments, char command[], char *exec_args[], char *out_file, bool output_redirect and bool output_append, and the struct pipeline has char *array and char *array2 in it.

Phase 1
In phase 1, the commands are sent to the cmd_with_no_pipes function in the beginning. It checks whether the type of the command on whether it is output redirection, output append or a normal one. Since phase 1 only has normal commands, we send it to another function called common_cmd that separates the commannds and the arguments and stores them into the exec_args array. Then we create the pipe and then call fork. After calling fork(), in the child we have the check condition to check for output redirection and ouput append.
