#include <stdio.h>
#include <syscall.h>

int
main (int childID)
{
	wait(childID);
    
	return EXIT_SUCCESS;
}
