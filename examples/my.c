#include <stdio.h>
#include <syscall.h>

int
main (void)
{
    //printf("Running my.c to test SYS_HALT\n");

	halt();
    
    return EXIT_SUCCESS;
}
