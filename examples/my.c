/* Wait for a subprocess to finish. */

#include <stdio.h>
#include <syscall.h>

int
main (void) 
{
 	const char *file_name = "SYSCREATEFileName";
	int file_size = 2;
 	create(file_name, file_size);
}
