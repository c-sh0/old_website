#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int i;

	printf("Content-type: text/plain\n\n");

	printf("%d command line arguments.\n", argc);

	for (i = 0; i < argc; i ++)
			printf("arg %d = \"%s\"\n", i, argv[i]);
		
	fflush(stdout);

 return(0);
}
