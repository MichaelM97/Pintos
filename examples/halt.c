#include <stdio.h>
#include <syscall.h>

int
main (void) 
{
 	const char *file_name = "SYSCREATE";
	remove(file_name);
}
