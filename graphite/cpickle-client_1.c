/*
pickle client example. Needs work, this is just an example, nothing more.
client will send a full 1mb message buff reguardless if it's full.

Tested on RHEL 5.8 (Tikanga) 2.6.18-308.1.1.el5 x86_64

Ref:
tcpdump -i lo -nnXSs 0 port 2004
http://docs.python.org/library/pickle.html
http://peadrop.com/blog/2007/06/18/pickle-an-interesting-stack-language
http://grokbase.com/t/python/tutor/1198jnwcbw/how-obsolete-is-2-2-and-a-pickle-question
http://grokbase.com/t/python/python-list/038fd2ceq5/cpickle-alternative
http://www.sensepost.com/cms/resources/labs/papers/sour_pickles/sour_pickles.pdf

- csh

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define HOST "127.0.0.1"
#define PORT 2004
#define MAX_RECORDS 500
#define MSG_SIZE 1048000 /* 1mb  */

typedef struct metrics {
   char name[256];
   int ts;
   float value;
} metrics_t;


/* pickle message, yeah it's ugly */
int pickle_dumps(struct metrics *data, char *msg) {
    int i = 0;
    int p = 0;
    int m = 0;
    int str_len = 512;
    char string[str_len];
    bzero(string,str_len);

    strcat(msg,"(lp0\n");
    for(i = 0; i != MAX_RECORDS; i++) {
      if(strlen(data[i].name)) {
         p++;
           if(!m) {
              sprintf(string,"(lp%d\n",p);
              strcat(msg,string);
              bzero(string,str_len);
           }
         p++;
           sprintf(string,"S'%s'\n",data[i].name);
           strcat(msg,string);
           sprintf(string,"p%d\n",p);
           strcat(msg,string);
           bzero(string,str_len);
         p++;
           sprintf(string,"a(lp%d\n",p);
           strcat(msg,string);
           bzero(string,str_len);

         p++;
           sprintf(string,"I%d\n",data[i].ts);
           strcat(msg,string);
           bzero(string,str_len);

            /* 'F' = float, 'I' = int */
            /* dealing with floats */
            int int1 = (int)data[i].value;
            float float2 = (data[i].value - int1);
            int int2 = (int)(float2 * 10000000);

            sprintf(string,"aF%d.%d\n",int1,int2);
            strcat(msg,string);
            bzero(string,str_len);

           if(strlen(data[(i+1)].name)) {
             sprintf(string,"aaa(lp%d\n",p);
             strcat(msg,string);
             bzero(string,str_len);
           }

         if(!m) { p--; }
         m = 1;
      }

    }
    strcat(msg,"aaa.");
    msg[strlen(msg)] = '\0';

 return(1);
}

int main() {
    int sock;
    struct hostent *host;
    struct sockaddr_in addr;
    char send_message[MSG_SIZE];
    bzero(send_message,MSG_SIZE);

    metrics_t metrics[MAX_RECORDS] = {
                                      {"testing.mytest.one", 1333998900, 666.260827},
                                      {"testing.mytest.four", 1333998900, 666.260827},
                                      {"testing.mytest.five", 1333998900, 666.260827},
                                      {"testing.mytest.six", 1333998900, 666.260827},
                                      {"testing.mytest.seven", 1333998900, 666.260827},
                                   };


    /* hostent struct */
    if((host = gethostbyname(HOST)) == NULL) {
        fprintf(stderr, "gethostbyname() failed");
     exit(1);
    }

    /* set up connection */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr = *(struct in_addr*)host->h_addr;
    addr.sin_addr.s_addr = inet_addr(HOST);

    /* open connection   */
    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
         perror("Connect failed");
	return 1;
    }

   /* pickle message */
   pickle_dumps(metrics,send_message);

   /* message length */
   unsigned long buflen = sizeof(send_message);
   uint32_t bufsend = htonl(buflen);

   /* send message */
   if(send(sock, &bufsend, sizeof(bufsend), 0) == -1) {
      perror("send");
     exit(1);
   }
   if(send(sock,send_message,bufsend*sizeof(char), 0) == -1) {
      perror("send");
     exit(1);
   }

   /* close connection */
   close(sock);

 return(0);
}


