#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED)
{
  /* Get the system call number,
  which is define in 'Pintos/lib/syscall-nr.h' */
  uint32_t *p = f->esp;

  //Switch statement for handling system calls
  switch(*p) {

    // Case for System Halt called
    case SYS_HALT:{
      printf("System HALT has been called!\n");
      shutdown();
      break;
    }

  }
}
