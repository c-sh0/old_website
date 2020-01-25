/* 
 * icmp.c
 *
 * Example Code to show how to build a ICMP_ECHO packet 
 * send it off to a remote host and wait for a reply.
 *
 * Thu Jun 26 13:45:58 CDT 2003
 * -CH
 *
 *
 */


#include <stdio.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define ETH_P_IP 0x800


u_short in_cksum(unsigned short *addr, int len)
{
u_short *word = (u_short*) addr;
u_long acc = 0;

 while((len -= 2) >= 0)
          acc += *(word++);

return ~(*(u_short*)&acc + *((u_short*)&acc + 1));
}


int main (int argc, char **argv)
{
    
   struct iphdr *ip;
   struct icmphdr *icmp;
   struct sockaddr_in addr;
   int sock, optval, result; 
   char *packet, *buffer;
/*   int s; */

      if(argc != 2) {
           fprintf(stderr,"usage: %s <ip_addr>\n",argv[0]);
           return(1);
      }

ip   = (struct iphdr *) malloc(sizeof(struct iphdr));
icmp = (struct icmphdr *) malloc(sizeof(struct icmphdr));
packet  = (char *) malloc(sizeof(struct iphdr) + sizeof(struct icmphdr));
buffer  = (char *) malloc(sizeof(struct iphdr) + sizeof(struct icmphdr));


addr.sin_family = AF_INET;
addr.sin_addr.s_addr = inet_addr(argv[1]);



ip = (struct iphdr *) packet;

/* ip->frag_off |= htons(0x2000); */
 ip->ihl     = 5;
 ip->version = 4;
 ip->tos     = 0;
 ip->tot_len = sizeof(struct iphdr) + sizeof(struct icmphdr);
 ip->id      = htons(getuid());
 ip->ttl      = 255;
 ip->protocol = IPPROTO_ICMP;
 ip->daddr    = addr.sin_addr.s_addr;
 ip->check    = in_cksum((unsigned short *)ip, sizeof(struct iphdr));



icmp = (struct icmphdr *) (packet + sizeof(struct iphdr));


  icmp->type = ICMP_ECHO;
  icmp->code = 0;
  icmp->un.echo.id = 0;
  icmp->un.echo.sequence = 0;
  icmp->checksum = 0;
  icmp->checksum = in_cksum((unsigned short *)icmp,sizeof(struct icmphdr));



if((sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
      fprintf(stderr,"error opening raw socket.\n");
      return(-1);
   }

    setsockopt(sock, SOL_IP, IP_HDRINCL, &optval, sizeof(int));
/* s = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP)); */




   sendto(sock,packet,ip->tot_len,0,(struct sockaddr *)&addr,sizeof(struct sockaddr));
   result = recv(sock,buffer,sizeof(struct iphdr)+sizeof(struct icmphdr),0);

        if(result>-1) {
             printf("%d: ICMP ECHO Reply...\n",result);
            /* printf("%s\n",buffer);  */
        } 


        close(sock);
        return(0);
}
