#include <stdio.h>
#include <syscall.h>

int
main (void) 
{
 	const char *file_name = "SYSCREATE";
	int fd = open(file_name);
	open(file_name);
	close(fd);
	open(file_name);
}
