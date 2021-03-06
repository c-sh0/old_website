/*
UDP RAW SOCKET EXAMPLE CODE by Murat Balaban [murat@enderunix.org]

   Modified to add payload and better checksuming (checksum code taken from dhclient/packet.c)

  09/16/2015
  - csh

*/



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
//#include <netinet/tcp.h>

#include <arpa/inet.h>

u_int32_t checksum(unsigned char *buf, unsigned nbytes, u_int32_t sum) {
        int i;

        /* Checksum all the pairs of bytes first... */
        for (i = 0; i < (nbytes & ~1U); i += 2) {
                sum += (u_int16_t)ntohs(*((u_int16_t *)(buf + i)));
                if (sum > 0xFFFF)
                        sum -= 0xFFFF;
        }

        /*
         * If there's a single byte left over, checksum it, too.
         * Network byte order is big-endian, so the remaining byte is
         * the high byte.
         */
        if (i < nbytes) {
                sum += buf[i] << 8;
                if (sum > 0xFFFF)
                        sum -= 0xFFFF;
        }

        return (sum);
}

u_int32_t wrapsum(u_int32_t sum) {
        sum = ~sum & 0xFFFF;
        return (htons(sum));
}

struct psd_udp {
        struct in_addr src;
        struct in_addr dst;
        unsigned char pad;
        unsigned char proto;
        unsigned short udp_len;
        struct udphdr udp;
};

unsigned short in_cksum(unsigned short *addr, int len)
{
        int nleft = len;
        int sum = 0;
        unsigned short *w = addr;
        unsigned short answer = 0;

        while (nleft > 1) {
                sum += *w++;
                nleft -= 2;
        }

        if (nleft == 1) {
                *(unsigned char *) (&answer) = *(unsigned char *) w;
                sum += answer;
        }

        sum = (sum >> 16) + (sum & 0xFFFF);
        sum += (sum >> 16);
        answer = ~sum;
        return (answer);
}


unsigned short in_cksum_udp(int src, int dst, unsigned short *addr, int len)
{
        struct psd_udp buf;

        memset(&buf, 0, sizeof(buf));
        buf.src.s_addr = src;
        buf.dst.s_addr = dst;
        buf.pad = 0;
        buf.proto = IPPROTO_UDP;
        buf.udp_len = htons(len);
        memcpy(&(buf.udp), addr, len);
        return in_cksum((unsigned short *)&buf, 12 + len);
}


unsigned short csum(unsigned short *buf, int nwords)
{
    unsigned long sum;
    for(sum=0; nwords>0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum &0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}


void *run(void *arg)
{
        struct ip ip;
        struct udphdr udp;
        int sd;
        const int on = 1;
        struct sockaddr_in sin;

        char *str = "<158>BLAAAAAtes1234567DDDDDDDDDDDDDDtes1234567WORKS";

        unsigned long len = sizeof(struct ip) + sizeof(struct udphdr) + strlen(str);

        u_char *packet;
        packet = (u_char *)malloc(len);

        ip.ip_hl = 0x5;
        ip.ip_v = 0x4;
        ip.ip_tos = 0x0;
        ip.ip_len = len;
        //ip.ip_id = htons(12830);
        ip.ip_id = htons(random());
        ip.ip_off = 0x0;
        ip.ip_ttl = 64;
        ip.ip_p = IPPROTO_UDP;
        ip.ip_sum = 0x0;
        ip.ip_src.s_addr = inet_addr("192.168.13.50");
        //ip.ip_src.s_addr = inet_addr("151.236.24.58");
        ip.ip_dst.s_addr = inet_addr("172.18.96.60");
        //ip.ip_sum = in_cksum((unsigned short *)&ip, sizeof(ip));
        ip.ip_sum = wrapsum(checksum((unsigned char *)&ip, sizeof(ip), 0));
        memcpy(packet, (char *)&ip, sizeof(ip));


        //udp.uh_sport = htons(45512);
        udp.uh_sport = htons(0); // we don't care to recive anything back
        udp.uh_dport = htons(514);
        //udp.uh_ulen = htons(8 + strlen(str)); // #define UDP_HDRLEN  8
        udp.uh_ulen = htons(sizeof(udp) + strlen(str)); // #define UDP_HDRLEN  8
        udp.uh_sum = 0;
        //udp.uh_sum = in_cksum_udp(ip.ip_src.s_addr, ip.ip_dst.s_addr, (unsigned short *)&udp, sizeof(udp));
        udp.uh_sum = wrapsum(checksum((unsigned char *)&udp, sizeof(udp),
                          checksum((unsigned char *)str, strlen(str), checksum((unsigned char *)&ip.ip_src,
                          2 * sizeof(ip.ip_src), IPPROTO_UDP + (u_int32_t)ntohs(udp.uh_ulen)))));


        memcpy(packet + sizeof(ip), &udp, sizeof(udp));
        memcpy(packet + sizeof(ip) + sizeof(udp), str, strlen(str));


/* verify should return 0 */
printf("checksum = %d\n",udp.uh_sum);
printf("verify checksum = %d\n",wrapsum(checksum((unsigned char *)&udp, sizeof(udp), 
                          checksum((unsigned char *)str, strlen(str), checksum((unsigned char *)&ip.ip_src,
                          2 * sizeof(ip.ip_src), IPPROTO_UDP + (u_int32_t)ntohs(udp.uh_ulen)))))
);

        if ((sd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
                perror("raw socket");
                exit(1);
        }

        if (setsockopt(sd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0) {
                perror("setsockopt");
                exit(1);
        }


        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = ip.ip_dst.s_addr;
        sin.sin_port = htons(514);

        if (sendto(sd, packet, len, 0, (struct sockaddr *)&sin, sizeof(struct sockaddr)) < 0)  {
                perror("sendto");
                exit(1);
        }

}


int main(int argc, char **argv)
{
        run(NULL);
        return 0;
}

