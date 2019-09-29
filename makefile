all: shell.c minishell.h
	gcc -o shell  shell.c

clean:
	rm -f shell

