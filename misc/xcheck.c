/* 
    Simple check to see if a given host has an open X display.
 This can be easily used in a shell script to scan a range
of ip's. ;-)

Linux Compile:

  gcc -L/usr/X11R6/lib -lX11 -o xcheck xcheck.c 

- noconflic
http://nocon.darkflame.net
nocon@darkflame.net

*/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <X11/Xlib.h>

void trap(int sig)
{
   printf("XOpenDisplay() timeout.\n");
   exit(1); 
}

int main(int argc, char* argv[])
{
Display *display;
char *ip;

     if (argc != 2) {
         printf("Usage: %s <ip:display.num>\n", argv[0]);
         printf("   ex: %s 192.168.1.1:0.0\n", argv[0]);
         exit(-1);
     }

ip = argv[1];

printf("----------------------------------\n");
printf("Simple check for open X display.  \n");
printf("http://nocon.darkflame.net        \n");
printf("----------------------------------\n");
printf("Trying: %s\n",ip);
 
              signal(SIGALRM,trap);
              alarm(10);  /* Timeout */

                display=XOpenDisplay(ip); 

            if (display != NULL) {
               printf("%s: X Display on %s is open.\n",argv[0],ip);
               XCloseDisplay(display); 
              }
 return 0;
}
