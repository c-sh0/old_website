/*
         IP 2 Decimal and Back agian
        ------------------------------
  
 Ever see a URL in the form of

    http://3232235521/

  Basicly all that is the actual decimal value of 

     192.168.0.1

  This Program converts an IP ( dotted notation ) to 
 the Decimal ( dotless notation) and Back agian. 

    Compile:
       gcc -lm -o ip2d ip2d.c 

 For more Conversion info:
   http://www.pc-help.org/obscure.htm

    -NoCoNFLiC
     nocon@darkflame.net
*/



#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

int a,b,c,d;
char ip_address[20];
double num,num2,num3,num4;
int x,x2,x3,x4;
unsigned int numa;

int main(int argc, char *argv[])
{
       if(argc!=2) {
        printf("\nUsage: %s [-c] [-d]\n",argv[0]);
        printf("\t\t-c --- IP to Decimal\n");
        printf("\t\t-d --- Decimal to IP\n\n");
         exit(1); 
      }
	if(argv[1][0]=='-' && argv[1][1] =='c') {
           printf("Enter IP to Convert: ");
              fgets(ip_address, sizeof(ip_address), stdin);
               sscanf(ip_address,"%d.%d.%d.%d",&a,&b,&c,&d);


  /* Convert Dotted notation to dotless Notation  */
         num=(pow(256,3))*a;
         num2=(pow(256,2))*b;
         num3=256*c;
         num4=num+num2+num3+d;
           
            printf("%0.f\n",num4);
          }
            
          if(argv[1][0]=='-' && argv[1][1] =='d') {
             printf("Enter Number to Convert: ");
              fgets(ip_address, sizeof(ip_address), stdin);
               sscanf(ip_address,"%u",&numa); 


  /* Covert Dotless notation to dotted Notation */            
         x=numa/pow(256,3); 
         x2=(numa-(x*pow(256,3)))/pow(256,2);
         x3=((numa-(x*pow(256,3)))-(x2*pow(256,2)))/256;
         x4=(((numa-(x*(pow(256,3))))-(x2*(pow(256,2))))-(x3*256));

         printf("%d.%d.%d.%d\n",x,x2,x3,x4);
        }
 
 return(0);
}

