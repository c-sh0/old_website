/* gcc -Wall -O2 -o setsid setsid.c */ 
/* cc -Wall -O2 -o setsid setsid.c */ 
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define _(Text) (Text)

int main(int argc, char *argv[]) {
        int ret; 	
	if (argc < 2) {
		fprintf(stderr, _("usage: %s program [arg ...]\n"),
			argv[0]);
		exit(1);
	}
	if (getpgrp() == getpid()) {
		switch(fork()){
		case -1:
			perror("fork");
			exit(1);
		case 0:		/* child */
			break;
		default:	/* parent */
			exit(0);
		}
	}
	if ((ret = setsid()) < 0) {
		perror("setsid"); /* cannot happen */
		exit(1);
	}
	execvp(argv[1], argv + 1);
	perror("execvp");
	exit(1);
}
