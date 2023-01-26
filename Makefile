
sshell: sshell.c
	gcc -Wall -Werror -g -Wextra sshell.c -o sshell

clean:
	rm sshell
