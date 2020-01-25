/*

    Just a Quick and dirty ssh Version scanner.
Scans a subnet and reports sshd versions.

Compile:
    gcc -Wall -o sshver sshver.c

noconflic
http://nocon.darkflame.net
Mon Jul 1 2002

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define PORT 22
#define TIMEOUT 5

int main (int argc, char *argv[])
{
char ip[1024];
char net[1024];
char verbuff[1024];
char *hostname;           	       /* pointer to ip of host */
struct sockaddr_in saddr; 		/* socket address */
struct hostent *host;			/* getnamebyhost info returned */
struct timeval timeout;
int s,i,flags,x;
fd_set fd_r, fd_w;

if (argc != 2) {
         printf("Scan for SSHd Versions.\n");
         printf("   Usage: %s <subnet>\n", argv[0]);
         printf("   ex: %s 192.168.0\n", argv[0]);
     exit(1);
    }

strncpy(net,argv[1],999);

for(i=1;i<255;i++) {
    sprintf(ip,"%s.%d",net,i);

         timeout.tv_sec         = TIMEOUT;
         timeout.tv_usec        = 0;

hostname = ip;
host = gethostbyname(hostname);

memcpy((char *)&saddr.sin_addr,(char *)host->h_addr, host->h_length);

	   saddr.sin_family = host->h_addrtype;
           saddr.sin_port = htons(PORT);

       s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
       fcntl(s, F_SETFL, (flags = fcntl(s,F_GETFL)) | O_NONBLOCK );

                connect(s, (struct sockaddr *)&saddr, sizeof(saddr));

                     FD_ZERO(&fd_r);
                     FD_SET(s, &fd_r);
                     FD_ZERO(&fd_w);
                     FD_SET(s, &fd_w);

                        select(s+1, &fd_r, &fd_w, NULL, &timeout);

   if(FD_ISSET(s, &fd_w))
       {
             if(!connect(s, (struct sockaddr *)&saddr, sizeof(saddr))) {
                  sleep(1);
                  x = recv(s,verbuff,(sizeof(verbuff)-1),0);
                  verbuff[x] = '\0';
                  printf("%s: Version: %s",ip,verbuff);
              } else {
                   perror(ip);
              }
    } else {
       printf("%s: Connection timeout\n",ip);
    }

     memset(&(verbuff),0,sizeof(verbuff));
  close(s);
}
  return (0);
}

