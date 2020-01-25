
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(void) {
char c, offset;

   while ((c = getchar()) != EOF) {

            if (isalpha(c)) {
		  offset = isupper(c) ? 'A' : 'a';
	  	  putchar(offset + ((c - offset + 13) % 26));
  	    } else {
	           putchar(c);
	    }
   }

   return EXIT_SUCCESS;
}

