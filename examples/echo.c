#include <stdio.h>
#include <syscall.h>

int
main (void) 
{
	char c;
 	const char *file_name = "SYSCREATE";
	int fd = open(file_name);
	write(fd, "HelloWRITE", 100);	
	read(fd, &c, 100);

}
