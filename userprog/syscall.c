#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"
#include "filesys/filesys.h"
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
      shutdown_power_off();
      break;
    }

    // Case for System Exit called
    case SYS_EXIT:{
      printf("System EXIT has been called!\n");
      struct thread *child = thread_current();
      struct thread *parent = child->parent_thread;

      //Set exit code
      int exit_code = *((uint32_t*)(f->esp + ARG_1));
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

    //Case for System Execute Called
    case SYS_EXEC:{
      printf("System EXECUTE has been called!\n");
      //Processes file and chlid ID is returned to fil
      f->eax = process_execute ((char *)*((uint32_t*)(f->esp + ARG_1)));
      break;
    }

    //Case for System Wait called
    case SYS_WAIT:{
      printf("System WAIT has been called!\n");
      f->eax = process_wait(*((uint32_t*)(f->esp + ARG_1)));
      break;
    }
    case SYS_OPEN: {
      char *files_name = ((char *)*((uint32_t*)(f->esp + ARG_1))); //Filename off of stack
      f->eaz = open_file(files_name);
    }

    static int open_file(char*file_name)
    {
      struct file* file = filesys_open(file_name);
      struct thread *cur = thread_current();
      int file_descriptor;

      if (file == NULL){
        file_descriptor = -1;
        return file_descriptor;
      }

      struct file_size *fi = malloc(sizeof(struct file_info));

      file_descriptor = 2;
      //0 + 1r STDIN_FILENO and STDOUT_FILENO
        while(get_file(file_descriptor) != NULL) {
          file_descriptor++;
          }
        //store onto the threads list of files
        fi->file_descriptor = fd;
        fi->fp = file;
        list_push_back(&cur->files, &fi->fpelem);

        return file_descriptor;
          }
  }
}
