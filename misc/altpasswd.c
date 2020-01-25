/********************************************************************
altpasswd.c

README
-------------------------

  Simple passwd program. Get a new password using uid info from 
the standard /etc/passwd ( getpwnam() ) file but use an alternate 
encrypted "/path/to/alt/shadow" file. If user running the program 
does not have a UID entry in the standard "/etc/passwd" and equiv 
username in "/path/to/alt/shadow" then user will not be able to 
change password in "/path/to/alt/shadow". User must enter their 
old password which is checked against the password hash in 
"/path/to/alt/shadow" before changing to a new password. super-user 
can only change the password for another user. Be careful about 
setting any kind of  SUID/GUID bits. I have done some input bounds 
checking on password and username input, but it might still be 
exploitable in some way. use this program at your discretion. If you 
are worried users will try to exploit this program to gain higher 
privileges, then i think you have bigger problems ;-)

    The alternate password file follows the same format as any password 
( crypt(3) variant of DES ) shadow file.
             ex:
                   sam:oiuJMbiOUyI./:11604::::::
                   john:UvOiFjP.iOUyI:11382::::::
                   bill:Pi.oPNMbiOUyI:11743::::::

                   [.....]

      The only thing that is changed/modified in said file is just the 
encrypted passwd hash. I may write added functionality for password 
change/expire times in the future.

  I wrote this as part of a password dist/propagation tool. I wanted 
to send an alternate password file to several client machines that had 
the limited user accounts different from our primary shell server. Yeah 
we could have used NIS, but wanted to avoid the extra overhead. This 
gives a logged in user the ability to change their password at one 
primary location rather than having to hop on several machines to simply 
change their password. also is a good interface tool for users to change any 
.htpasswd type files for simple web based authentication. This program
works much the same way as the standard "passwd" program.

  This has been tested and compiled on Linux and Solaris 2.x. I would expect
this to compile fine on *BSD as well but have not tried. If not porting it 
should be fairly straight forward. 


Compile:

     gcc -o altpasswd altpasswd.c -lcrypt


   Any Suggestions, Bugs, etc.. welcome. yeah, The code needs work.


noconflic
http://nocon.darkflame.net
nocon@darkflame.net

*********************************************************************/ 

#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <time.h>
#include <crypt.h>
#include <ctype.h>
#include <fcntl.h>

/****************************************** 
 change this to your alternate shadow file 
******************************************/ 
#define PASSWDFILE "/etc/alt.shadow"

char chng_pass(char *argvuname, int id);
int checkpass(char *s);
int to64(char *s, long v, int n);
int getlock(int fd, int type);
int rand();


int main (int argc, char *argv[])
{

struct passwd *pw;
char *uname,*argusr;
uid_t	uid;


if (argc != 2) { 
           printf("Usage: %s <username>\n",argv[0]); 
           printf("Only superuser can change the password for another user.\n");
           exit(1);  

 } else if((strlen(argv[1])) > 8 ) {

            printf("supplied username is larger than 8 characters.\n");
            exit(1);
 } else {
  
   argusr = argv[1]; 
   pw = getpwnam(argusr);
   uname = getlogin();
   uid = getuid();

       if(uid == 0 && pw) {
            printf("Changing password for %s...\n",argusr);
            
                  chng_pass(argusr,uid);
 
      } else if(!pw || uid != pw->pw_uid) {
           printf("non-existent user %s, only superuser can change the password for another user.\n",argusr);
           exit(1);

       } else {
           printf("Changing password for %s...\n",uname);
         
                  chng_pass(argusr,uid);
    
        }
}

  return(0);
} 

/*************************************** 
   function to get old password, 
change new pasword, then write to file 
***************************************/

char chng_pass(char *argvuname, int id)
{
FILE *shadow_file;

char line_buf[100],uname_buf[10],salt[3];
char pass_buf[10],newpass_buf[10];
char *username,*passwd,*login;
char *result,*cpw,*newpass;

long len,num,pos,userlen;

int fd;
int count = 3;
int n = 0;

    /*********************************************** 
       Lock the password file, not sure if this 
       is the best/right way, but it seems to work 
    ***********************************************/
       fd = open(PASSWDFILE,O_RDWR); 
              if (fd < 0) {
                      perror(PASSWDFILE);
                    exit(1);;
                  }

               getlock(fd,F_WRLCK);          
     /***********************************************/

    login = uname_buf; 
    shadow_file = fopen(PASSWDFILE,"r+");

                 if(shadow_file == NULL) {
                         perror(PASSWDFILE); 
                       exit(1);
                 } 

	strncpy(login,argvuname,strlen(argvuname));
	login[strlen(argvuname)] = ':';
	login[strlen(argvuname)+1] = '\0';

         if(fgets(line_buf,sizeof(line_buf),shadow_file) == NULL) {
                printf("Read error\n");
                      perror(PASSWDFILE);
              exit(1);
         }

    rewind(shadow_file);

while(fgets(line_buf,sizeof(line_buf),shadow_file) != NULL) {
   
              if(strstr(line_buf,login) != NULL) {
                 
                    num = ftell(shadow_file);
                    len = strlen(line_buf);

                    username =  strtok(line_buf,":");
                    passwd = strtok(NULL,":");
                    userlen = strlen(username);
 
                    pos = (userlen+1) + (num - len);
                 
                     if(id != 0) { 
                          result = crypt(getpass("OLD Password:"),passwd);
                      }

                          if(id != 0 && strcmp(result,passwd)) {
                                 printf("Permission denied.\n");
                                   n = 0;
                               break;
                        }  else {

                         while(count) {
                             strncpy(pass_buf,getpass("New password:"),8);
                             pass_buf[8] = '\0';
                             cpw = pass_buf;

                                    if(!*cpw) {
                                         n = 0;
                                         count = 0;
                                         break;
                                     }

				    if(strcmp(passwd,crypt(cpw,passwd)) == 0) {
                                       printf("Ummm, you are triing to \"change\" that password right ?\n");
                                       n = 0;
                                       --count;
                                       continue;
                                     }


                                      if(strlen(cpw) < 8 ) {
                                             printf("Password must be 8 characters long\n");     
                                             n = 0;
                                             --count;
                                             continue;
                                     } 
                                       if(checkpass(cpw) < 2) {
                                            printf("Password too weak. Try mixed case and/or numbers\n");
                                            --count;
                                             n = 0;
                                            continue; 
                                     } else {
 			                    strncpy(newpass_buf,getpass("Re-enter new password:"),8);
                                            newpass_buf[8] = '\0';
                                            newpass =  newpass_buf;
                                              
                                              if(strcmp(cpw,newpass)) {
                                                     printf("Password mismatch.\n");
                                                    --count;
                                                    n = 0;
                                                    continue;
                                              } else {
                                                   n = 0;
                                                  break;
                                             }
                                     }
                           }          

        
         if(count == 0) {
                   printf("Password unchanged.\n"); 
                   n = 0;
                break; 
         } else {
                  
                  /***************************************************** 
                       Set the salt taken from FreeBSD password src. 
                  ******************************************************/
			srand((int)time((time_t *)NULL));
			to64(&salt[0],rand(),9);
			to64(&salt[3], rand(), 3);
			to64(&salt[6], rand(), 2);
			to64(&salt[8], rand(), 5);
			to64(&salt[13], rand(), 5);
			to64(&salt[17], rand(), 5);
			to64(&salt[22], rand(), 5);
			salt[27] = '\0';        
                    /************************************************/

                              fseek(shadow_file,pos,SEEK_SET); 
                              fprintf(shadow_file,crypt(newpass,salt));

                           printf("Password successfully changed.\n");
                       break;
            }

         } 
              
     } else { n = 1; } 
       
    fflush(shadow_file);
  }
  
               if(n == 1) {
                    printf("Sorry user %s not found in %s.\n",argvuname,PASSWDFILE); 
               }

      fclose(shadow_file);
    getlock(fd, F_UNLCK);  /***** Release file lock *****/
  return(0);
}


/************************************************
 check and make sure we have mixed case password 
 may add stricker policy's later.
*************************************************/
int checkpass(char *s)
{
int i = 0;
     for (;*s != '\0';s++) {
         if(isupper(*s)) i++;
         if(isdigit(*s)) i++;
      }
   return(i);
}        
 
             
/********************************************************
 Set the salt function taken from FreeBSD password src.
********************************************************/
int to64(char *s, long v, int n)
{
static unsigned char itoa64[] =         /* 0 ... 63 => ascii - 64 */
        "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    while (--n >= 0) {
        *s++ = itoa64[v&0x3f];
        v >>= 6;
    }

  return(0);
}

/***********************************************
  Get file lock on the alt shadow file
***********************************************/
int getlock(int fd, int type)
{
struct flock lockinfo;

    lockinfo.l_whence = SEEK_SET;
    lockinfo.l_start = 0;
    lockinfo.l_len = 0;
    lockinfo.l_type = type;

      if (!fcntl(fd, F_SETLK, &lockinfo)) {
        return(0);

     } else {
            fcntl(fd, F_GETLK, &lockinfo);
                printf("password file is Locked pid: %d\n",lockinfo.l_pid);
              exit(0);
    }
 return(0);
}
