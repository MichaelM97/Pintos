#include <stdio.h>
#include <syscall.h>

int
main (void) 
{
 	const char *file_name = "SYSCREATE";
	filesize(open(file_name));

}
