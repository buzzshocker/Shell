# ecs150_proj1

We downloaded the starter files and then created the Makefile in Phase 0. My partner and I worked on every single phase side by side together. To begin with, we created a struct called 'struct cmdline_args' that holds all the command and the arguments passed. We then decided to have 2 structs, one for the piping arguments(pipeline) and another one(cmdline_args) for all the other command line arguments. The cmdline_args struct has char *arguments, char command[], char *exec_args[], char *out_file, bool output_redirect and bool output_append  
