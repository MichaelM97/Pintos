#include <stdio.h>
#include <syscall.h>

int
main (void) 
{
 	const char *file_name = "SYSCREATE";
	int fd = open(file_name);
	write(fd, "HelloWRITE", 100);	

}
