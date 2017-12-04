#include <stdio.h>
#include <syscall.h>

int
main (void)
{

	exec("halt");
    
    return EXIT_SUCCESS;
}
