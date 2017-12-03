#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"

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

  //Declare variables
  //int status;

  //Switch statement for handling system calls
  switch(*p) {

    // Case for System Halt called
    case SYS_HALT:{
      printf("System HALT has been called!\n");
      shutdown_power_off();
      break;
    }

    // Case for System Exit called
    case SYS_EXIT:
      printf("System EXIT has been called!\n");
      struct thread *child = thread_current();
      struct thread *parent = child->parent_thread;

      //Set exit code
      int exit_code = *((uint32_t*)(f->esp + 4));
      child->exit_code = exit_code;

      //Retrieve the current child
      struct list_elem *e;
      struct child_process *cp = NULL;

      //Set the child process if parent exists
      if(parent != NULL) {
        for (e = list_begin (&parent->children); e != list_end (&parent->children);
           e = list_next (e))
           {
          cp = list_entry (e, struct child_process, c_elem);
          if(cp->pid == child->tid){
            break;
          }
        }
        //Get exit code from child process
        if(cp->pid == child->tid && cp != NULL) {
          cp->return_code = exit_code;
          sema_up(&cp->alive);
        }
      }
      thread_exit();
      break;
  }
}
