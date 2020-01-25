/* 
  This is a rough draft. I lost the clean version. :(
  - csh 09/16/2015 
  ref: 
   http://www.happybearsoftware.com/implementing-a-dynamic-array.html

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	char *name;
	//int number;
} DATA;

DATA 	*the_array = NULL;
int 	num_elements = 0; // To keep track of the number of elements used
int     num_allocated = 0; // This is essentially how large the array is

int AddToArray (DATA item)
{
printf("Elements: %d\n",num_elements); 
printf("E: %lu\n",(sizeof(the_array)/sizeof(the_array[0]))); 

	if(num_elements == num_allocated) { // Are more refs required?
		

		// Feel free to change the initial number of refs and the rate at which refs are allocated.
		if (num_allocated == 0)
			num_allocated = 3; // Start off with 3 refs
		else
			num_allocated *= 2; // Double the number of refs allocated
		
		// Make the reallocation transactional by using a temporary variable first
		void *_tmp = realloc(the_array, (num_allocated * sizeof(DATA)));

printf("num_allocated: %d\n",num_allocated); 
		
		// If the reallocation didn't go so well, inform the user and bail out
		if (!_tmp)
		{ 
			fprintf(stderr, "ERROR: Couldn't realloc memory!\n");
			return(-1); 
		}
		
		// Things are looking good so far, so let's set the 
		the_array = (DATA*)_tmp;	
	}
	
	the_array[num_elements] = item; 
	num_elements++;
	
	return num_elements;
}

int main()
{
	// Some data that we can play with
	char *names[] = { "fydo567999","22222","3333","44444","5555","6666","7777","8888" };
//	int numbers[] = { 42, 33, 15, 74 };
	int i;
	

	// Populate!
	for (i = 0; i < (sizeof(names)/sizeof(names[0])); i++)
	{
		DATA temp;
		
		temp.name = malloc((strlen(names[i]) + 1) * sizeof(char));
		strncpy(temp.name, names[i], strlen(names[i]) + 1);
//		temp.number = numbers[i];
		
		if (AddToArray(temp) == -1) // If there was a problem adding to the array,
			return 1;				// we'll want to bail out of the program. You
									// can handle it however you wish.
	}
	
	// Regurgitate!
	for (i = 0; i < (sizeof(names)/sizeof(names[0])); i++)
	{
		//printf("%s's number is %d!\n", the_array[i].name, the_array[i].number);
		printf("%s \n", the_array[i].name);
	}
	
	printf("\n%d allocated, %d used\n", num_allocated, num_elements);
	
	
	// Deallocate!
	for (i = 0; i < (sizeof(names)/sizeof(names[0])); i++)
	{
		free(the_array[i].name);
	}

	free(the_array);	
	
	// All done.
	return 0;
}

