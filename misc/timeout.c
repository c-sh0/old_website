/*  
Example of setting a connection timeout for connect()
Thanx to "loophole" for the select() example. ;-)

-noconflic
http://nocon.darkflame.net


Compile: 
         gcc -Wall timeout.c -o timeout

Example Output:

[nocon@]$ ./timeout 4.34.195.211
Connecting....
Alive
Conection to 80 refused
[nocon@]$ ./timeout 64.58.76.177
Connecting....
Alive
Port 80 Open
[nocon@]$ ./timeout 4.34.191.100
Connecting....
Conection timeout
Conection to 80 refused
  


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

#define PORT 80       /* TCP port number    */
#define TIMEOUT 5     /* timeout in seconds */

int main (int argc, char *argv[]) 
{
char *hostname = argv[1];         /* pointer to name of server */
struct sockaddr_in saddr; 		/* socket address */
struct hostent *host;			/* getnamebyhost info returned */
int s,i;


fd_set fd_r, fd_w;
struct timeval timeout;
int flags;

if (argc != 2) {
      printf("Usage: %s [host]\n",argv[0]); 
     exit(1);  
    }

timeout.tv_sec         = TIMEOUT;
timeout.tv_usec        = 0;

host = gethostbyname(hostname);

memcpy((char *)&saddr.sin_addr,(char *)host->h_addr, host->h_length); 
saddr.sin_family = host->h_addrtype;
saddr.sin_port = htons(PORT);

/* set up the socket for connect */
s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

/* set the socket fd to non-blocking mode */
fcntl(s, F_SETFL, (flags = fcntl(s,F_GETFL)) | O_NONBLOCK );

/* Try to connect */
printf("Connecting....\n");
  connect(s, (struct sockaddr *)&saddr, sizeof(saddr));

/* select() macros, see select(2) manpage */
FD_ZERO(&fd_r);
FD_ZERO(&fd_w);
FD_SET(s, &fd_r);
FD_SET(s, &fd_w);

/*  timeout durring connect() ??  */
select(s+1, &fd_r, &fd_w, NULL, &timeout);
   if(FD_ISSET(s, &fd_w))
       {
        printf("Alive\n");
    } else {
        printf("Conection timeout\n");
    }

/* get Port status */
i = connect(s, (struct sockaddr *)&saddr, sizeof(saddr));

if(i) {
    printf("Conection to 80 refused\n");
} else {
    printf("Port 80 Open\n");
}

/* close the fd and exit */
   close(s);
  return (0);
}
