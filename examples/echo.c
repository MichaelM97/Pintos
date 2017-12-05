#include <stdio.h>
#include <syscall.h>

int
main (void) 
{
 	const char *file_name = "SYSCREATE";
	int fd = open(file_name);
	write(fd, "Y", 100);	
write(fd, "e", 100);
write(fd, "l", 100);
write(fd, "l", 100);
write(fd, "o", 100);

}
