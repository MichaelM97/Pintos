#include <stdio.h>
#include <syscall.h>

int
main (void) 
{
 	const char *file_name = "SYSCREATE";
	unsigned file_size = 1000;
 	create(file_name, file_size);

}
