/* Wait for a subprocess to finish. */

#include <stdio.h>
#include <syscall.h>
//#include "threads/lib.h"
//#include "threads/main.h"

int
main (void) 
{
  printf("wait(exec()) = %d", wait (exec ("child-simple")));
}
