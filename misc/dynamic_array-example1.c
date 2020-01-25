/* 
  example of creating a dynamic array of strings 

    ** proububly want to modify this to only allocate string sizes 
       this example just guessti-mates sizes allocated

       Also, you can NOT get the size of a dynamic array 
       keep track of it yourself.
    **

   This is a rough draft. I lost the clean version. :(
  - csh 09/16/2015
  ref:
   http://www.happybearsoftware.com/implementing-a-dynamic-array.html

*/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main() {
     // some random strings of data to fill dynamic array
     char *names[] = { "00000","11111","222222","3333","44444","5555","66666666666666","7777","888","9999","101101" };
     char **array = NULL; 
     int i,total_elements; 
     int num_allocated = 3; // re-alocate some memory start

     printf(".:Static array contents:.\n"); 
     for(i = 0; i < (sizeof(names)/sizeof(names[0])); i++) { 

          //init array mem 
          if(i == 0) { 
            array = malloc((strlen(names[i]) + 1) * sizeof(*names)); //hope this is enough to allocate
            printf("Init array, size: %lu \n",((strlen(names[i]) + 1) * sizeof(*names)));
          } 

          if(i == num_allocated) { 
             num_allocated *= 2; 
             void *tmp = realloc(array,(num_allocated * sizeof(array))); //hope this is enough to re-allocate

             if(!tmp) { 
                fprintf(stderr, "ERROR: Couldn't realloc memory!\n");
                free(array); 
                return(-1);
             } 
             
             array = tmp; 
             printf("num_allocated: %d, size: %lu \n",num_allocated,(num_allocated * sizeof(array)));
          }             
          array[i] = names[i];  
          printf("Element: %d = %s , addr = %p\n",i, names[i],&names[i]);
     }     

     // print dynamic array contents
     printf("\n\n.:Dynamic array contents:.\n"); 
     total_elements = i; // can't get total elements from a dynamic array 
     for(i = 0; i < total_elements; i++) {
          printf("Element: %d = %s , addr = %p\n",i, array[i],*&array[i]);
     } 

     free(array); 

  return 1; 
} 
