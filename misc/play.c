/* play.c 
 *
 *  This program reads an ASCII file and display's the 
 * contents line by line, pausing between lines for "n"
 * seconds if specified. This works much in the same way 
 * as the "/play" command, present in some IRC clients.
 *
 *    Here ya go pr0ject, enjoy! ;-)
 *
 * Compile: 
 *
 *      gcc -o play play.c
 *
 * - noconflic    
 *
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc,char *argv[])
{

FILE *filehandle;
char buffer[80];
int sec;

  if(argc>=4) {
    printf("Usage: %s <filename> <seconds>\n",argv[0]);
    exit(1);
  }

  if(argc == 3) { sec = atoi(argv[2]); }
  else { sec = 0; }

filehandle = fopen(argv[1],"r");

   if(!filehandle) {
     printf("Error opening file: %s\n",argv[1]);
          exit(1);
   }

while(fgets(buffer,sizeof(buffer),filehandle) != NULL) { 
   printf("%s",buffer);
   sleep(sec);
 } 

   fclose(filehandle);
 return(0);
} 
