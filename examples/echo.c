#include <stdio.h>
#include <syscall.h>

int
main (void) 
{
	char c;
 	const char *file_name = "SYSCREATE";
	int fd = open(file_name);
	read(fd, &c, filesize(fd));

}
