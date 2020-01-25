/*
 Tested on
     RHEL 5.8 (Tikanga) 2.6.18-308.1.1.el5 x86_64
     FreeBSD

ref:
tcpdump -i lo -nnXSs 0 port 2004
https://answers.launchpad.net/graphite/+question/192753

- csh

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h> /* in.h needed for freebsd */

#define MAX_RECORDS 500
#define MSG_SIZE 1048000 /* aprox 1mb */
#define HOST "127.0.0.1"
#define PORT 2004
#define STR_LEN 512

int strn_cpy(char *buff, char *string, int size);

typedef struct metrics {
   char name[STR_LEN];
   long ts;
   float value;
} metrics_t;

/* pickle message, protocol version = 0 */
int pickle(struct metrics *data, char *p_msg) {
    int i;
    int index = 0;
    int s_cnt = 0;
    char tmp_str[STR_LEN];

    s_cnt = strn_cpy(p_msg,"(",sizeof("("));

    for(i = 0; i < MAX_RECORDS; i++) {
      if(strlen(data[i].name) && (data[i].ts > 0) && (data[i].value > 0)) {
            if(strlen(data[i].name) > (STR_LEN - 5)) { continue; }

            if(index > 0) {
               strcat(p_msg,"t");
               s_cnt++;
            }
            index++;

            s_cnt += snprintf(tmp_str, sizeof(tmp_str), "(S'%s'\np%d\n",data[i].name,index);
            strcat(p_msg,tmp_str);
            index++;

            /* dealing with floats */
            int int1 = (int)data[i].value;
            float float2 = (data[i].value - int1);
            int int2 = (int)(float2 * 10000000);

            /* timestamp, value */
            s_cnt += snprintf(tmp_str, sizeof(tmp_str), "(I%li\nF%d.%07d\ntp%d\n",data[i].ts,int1,int2,index);
            strcat(p_msg,tmp_str);
       }
    }

    s_cnt += snprintf(tmp_str, sizeof(tmp_str), "ttp%d\n.",++index);
    strcat(p_msg,tmp_str);

 return s_cnt;
}

int main() {
    char buffer[MSG_SIZE];
    int sock, s_len;
    unsigned long msg_size;
    struct hostent *host;
    struct sockaddr_in addr;

    /* about 500 metrics per message max */
    metrics_t metrics[MAX_RECORDS] = {
                                      {"testing.mytest.one", 1334251200, 110011.11011},
                                      {"testing.mytest.two", 1334255400, 254524.376},
                                      {"testing.mytest.three", 1334244900, 3675.0222},
                                      {"testing.mytest.four", 1334255700, 210789.265},
                                      {""}
                                   };

    /* pickle message */
    s_len = pickle(metrics,buffer);

    host = gethostbyname(HOST);
    sock = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr = *(struct in_addr*)host->h_addr;
    addr.sin_addr.s_addr = inet_addr(HOST);

    connect(sock, (struct sockaddr *)&addr, sizeof(addr));

    msg_size = (s_len + sizeof(uint32_t));
    uint32_t bufflen = htonl(msg_size);

    send(sock,&bufflen,sizeof(bufflen),0);
    send(sock,buffer,msg_size,0);

    close(sock);

 return(0);
}

/* FreeBSD strncpy() wrapper. make sure we NULL terminate */
int strn_cpy(char *buff, char *string, int size) {
    if(!string || !size) {
        printf("strn_cpy() err: empty string or missing size, cannot copy to buffer!\n");
       return(0);
    }

    int len = strlen(string);

    if(!len) {
        printf("strn_cpy() err: string(%d) zero, cannot copy to buffer!\n",len);
       return(0);
    } else if(len >= size) {
        printf("strn_cpy() err: string(%d) >= buffsize(%d), cannot copy to buffer!\n",len,size);
       return(0);
    } else {
       /* clear buffer and copy */
       bzero(buff,size);
       strncpy(buff,string,size);
       buff[len] = '\0';
    }

  return len;
}
