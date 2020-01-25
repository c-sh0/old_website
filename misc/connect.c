/*****************************************************************/
/*   This is a simple example of making a TCP connection         */
/*  in C.                                                        */    
/* 								 */    
/*     In this example we are issueing a HEAD request to port 80 */
/*  of a specified host and printing to the screen what was      */
/*  received.                                                    */
/*                                                               */
/* 08.12.01                                                      */
/* -NoCoNFLiC                                                    */
/*                                                               */
/* compile:                                                      */
/*   gcc -Wall -o connect connect.c                              */
/*                                                               */
/*****************************************************************/

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>



#define PORT 80 
#define DATA "HEAD / /HTTP/1.0\n\r\n\r"

int main(int argc, char *argv[])
{
        char buffer[1024];
        char receive[1024];
	struct sockaddr_in addr;
	struct hostent *host;
	int s;

	if(argc != 2) {
		fprintf(stderr,"usage: %s <host>\n", argv[0]);
		return 1;
	}


/* copy what we want to send to the buffer */
     strcpy(buffer,DATA);


/* resolve hostname and print    */
     host = gethostbyname(argv[1]);
	if(host == NULL) {
		perror("Connect failed");
		return 1;
	}

printf("----------------------------------------------------------\n");
  printf("Hostname : %s\n",host->h_name);                                    /* prints the hostname */
  printf("IP Address: %s\n",inet_ntoa(*(struct in_addr *)host->h_addr));     /* prints IP address */
printf("----------------------------------------------------------\n\n");


/* socket inet connect() setup */
     s = socket(AF_INET, SOCK_STREAM, 0);
     addr.sin_family = AF_INET;
     addr.sin_port = htons(PORT);
     addr.sin_addr = *(struct in_addr*)host->h_addr; 
  
/* This is just for the IP address  
   addr.sin_addr.s_addr = inet_addr(argv[1]);  */ 

/* make the connection   */
	if(connect(s, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		perror("Connect failed");
		return 1;
	}

/* send and receive our data */
/* print the received data to the screen and the close the connection */
       send(s, buffer, sizeof(buffer), 0); 

       while(recv(s, receive, sizeof(receive), 0)) 
         printf("%s\n",receive);

       close(s);

return 0;

}
