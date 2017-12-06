#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"

static void syscall_handler (struct intr_frame *);
static struct file_info* get_file (int fd);
static void system_exit (int exit_code);

static struct file_info* get_file (int fd){
  struct thread *cur = thread_current ();
  struct list_elem *e;
  struct file_info *fi;

  for(e = list_begin(&cur->files); e != list_end(&cur->files);
    e = list_next(e)){

    fi = list_entry(e, struct file_info, fpelem);
    if(fi->fd == fd) {
      return fi;
    }
  }
  return NULL;
}

static uint32_t fetch_args(struct intr_frame *f, int offset) {

    return *((uint32_t*)(f->esp + offset));

}
static int open_file(char*file_name)
{
  struct file* file = filesys_open(file_name);
  struct thread *cur = thread_current();
  int fd;

  if (file == NULL){
    fd = -1;
    return fd;
  }

  struct file_info *fi = malloc(sizeof(struct file_info));

  fd = 2;

  while(get_file(fd) != NULL)
  {
    fd++;
  }

  fi->fd = fd;
  fi->fp = file;
  list_push_back(&cur->files, &fi->fpelem);

  return fd;
}

static void system_exit (int exit_code) {
  struct thread *child = thread_current();
  struct thread *parent = child->parent_thread;

  //Set exit code
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
}

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
      system_exit((int)*((uint32_t*)(f->esp + ARG_1)));
      break;
    }

    //Case for System Execute Called
    case SYS_EXEC:{
      printf("System EXECUTE has been called!\n");
      //Processes file and chlid ID is returned to file
      f->eax = process_execute ((char *)*((uint32_t*)(f->esp + ARG_1)));
      break;
    }

    //Case for System Wait called
    case SYS_WAIT:{
      printf("System WAIT has been called!\n");
      f->eax = process_wait(*((uint32_t*)(f->esp + ARG_1)));
      break;
    }

    //Case for System Create called
    case SYS_CREATE:{
      printf("System CREATE has been called!\n");
      //Creates file, and returns true if successful
      bool successful = filesys_create(
        (char *)*((uint32_t*)(f->esp + ARG_1)) //File Name
        , (unsigned)*((uint32_t*)(f->esp + ARG_2))); //File Size
      f->eax = successful;
      printf("\nCREATE WAS %d\n", successful); //Temp line for testing
      break;
    }

    //Case for System Remove called
    case SYS_REMOVE:{
      printf("System REMOVE has been called!\n");
      //Removes file, and returns true if successful
      bool successful = filesys_remove(
        (char *)*((uint32_t*)(f->esp + ARG_1)) //File name
      );
      f->eax = successful;
      printf("\nREMOVE WAS %d\n", successful); //Temp line for testing
      break;
  }

    //Case for System Open called
    case SYS_OPEN: {
      printf("System OPEN has been called!\n");
      char *files_name = ((char *)*((uint32_t*)(f->esp + ARG_1)));
      int successful = open_file(files_name);
      f->eax = successful;
      printf("\nOPEN WAS %d\n", successful); //Temp line for testing
      break;
   }

   //Case for System Filesize called
   case SYS_FILESIZE:{
      printf("System FILESIZE has been called!\n");
     //Gets file info
     struct file_info *fi = get_file ((int)*((uint32_t*)(f->esp + ARG_1)));
     //Exits if file doesnt exist
     if (fi == NULL) {
       system_exit(-1);
     }
     //Returns file length
     f->eax = file_length (fi->fp);
     break;
   }

   //Case for System Read called
   case SYS_READ:{
      printf("System READ has been called!\n");
      //Set fd, buffer, and size from arguments
      int fd = *((uint32_t*)(f->esp + ARG_1));
      void *buffer = ((uint32_t*)(f->esp + ARG_2));
      unsigned file_size = *((uint32_t*)(f->esp + ARG_3));
      struct file_info *fi;
      //Read file from stdin
      if(fd == STDIN_FILENO) {
        int i;
        for(i = 0; i < (int)file_size; i++) {
        *(uint8_t *)(buffer+i) = input_getc();
      }
        f->eax = file_size;
      }
      //If fd stdout then exit
      if(fd == STDOUT_FILENO) {
        system_exit(-1);
      }
      //If file name empty then exit
      fi = get_file(fd);
      if (fi == NULL){
        system_exit(-1);
      }
      //Read bytes from file and return them
      int bytes_read_fr = file_read(fi->fp, buffer, file_size);
      f->eax = bytes_read_fr;
      printf("BYTES READ = %d", bytes_read_fr);
      break;
   }

   //Case for System Write called
   case SYS_WRITE:{
      printf("System WRITE has been called!\n");
     //Set fd, buffer, and size from arguments
     int fd = *((uint32_t*)(f->esp + ARG_1));
     const void *buffer = ((uint32_t*)(f->esp + ARG_2));
     unsigned int file_length = *((uint32_t*)(f->esp + ARG_3));

     //If fd is 1, write to the console and return file length
     if(fd == 1) {
      putbuf((const char *)buffer, (size_t) file_length);
      f->eax = file_length;
      }
     else {
      struct file_info *fi = get_file(fd);
      if(fi != NULL) {
        //Returns number of bytes written
        f->eax = file_write (fi->fp, buffer, file_length);
      }
      else {
        f->eax = 0;
      }
    }
    break;
   }

   //Case for System Tell called
   case SYS_TELL:{
      printf("System TELL has been called!\n");
      struct file_info *fi = get_file((int)*((uint32_t*)(f->esp + ARG_1)));
      //Return position of next byte to be read
      if(fi != NULL) {
         f->eax = file_tell (fi->fp);
      }
      else {
        f->eax = 0;
      }
     break;
   }

   //Case for System Seek
     case SYS_SEEK:
     {
     printf("System SEEK has been called!\n");

       int arg1 = (int) fetch_args(f,ARG_1);
       unsigned arg2 = (unsigned) fetch_args(f,ARG_2);

       struct file_info *fi = get_file(arg1);
       if (arg1 != NULL)
       {
         file_seek(fi->fp,arg2);
       }
      break;
   }

     //Case for System Close
     case SYS_CLOSE:
     {
     printf("System CLOSE has been called!\n");
     int argu = ((int)fetch_args(f,ARG_1));

     struct file_info *fi;
     fi = get_file(argu);
     if (fi != NULL) {
       file_close(fi->fp);
       list_remove(&fi->fpelem);
       free(fi);

     }
     break;
   }
   }
}
