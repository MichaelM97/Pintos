#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"

/* Function prototypes */
static void syscall_handler (struct intr_frame *);
static struct file_info* get_file (int fd);
static int open_file(char *file_name);
static void system_exit (int exit_code);

/* Finds file in thread list and return its information */
static struct file_info* get_file (int fd){
  struct thread *cur = thread_current ();
  struct list_elem *e;
  struct file_info *fi;

  //Loop through file list of current thread
  for(e = list_begin(&cur->files);
      e != list_end(&cur->files);
      e = list_next(e))
      {
        fi = list_entry(e, struct file_info, fpelem);
        if(fi->fd == fd) { //If file descriptors match
          return fi; //Return file_info struct for the matched file
        }
      }
  return NULL;
}

/*
* Retrieves arguments from the stack
* Position of argument defined by offset
* Offset sizes defined in syscall.h
*/
static uint32_t fetch_args(struct intr_frame *f, int offset) {
    return *((uint32_t*)(f->esp + offset));
}

/*
* Opens file with the same passed name
* Returns file descriptor
*/
static int open_file(char *file_name)
{
  struct file* file = filesys_open(file_name);
  struct thread *cur = thread_current();
  int fd;

  //If file does not exist then error
  if (file == NULL){
    fd = -1;
    return fd;
  }

  //Set file discriptor based on file location in list
  struct file_info *fi = malloc(sizeof(struct file_info));
  fd = 2;
  while(get_file(fd) != NULL)
  {
    fd++;
  }

  //Return file information to struct
  fi->fd = fd;
  fi->fp = file;
  list_push_back(&cur->files, &fi->fpelem);

  return fd;
}


/* Exit thread and return exit code to child */
static void system_exit (int exit_code) {
  struct thread *child = thread_current();
  struct thread *parent = child->parent_thread;

  //Return exit code
  child->exit_code = exit_code;

  //Retrieve the current child
  struct list_elem *e;
  struct child_process *cp = NULL;

  //Set the child process if parent exists
  if(parent != NULL) {
    for (e = list_begin (&parent->children);
         e != list_end (&parent->children);
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

/* Initalises the syscall_handler */
void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

/* Handles system calls */
static void
syscall_handler (struct intr_frame *f UNUSED)
{
  /* Get the system call number,
  which is define in 'Pintos/lib/syscall-nr.h' */
  uint32_t *p = f->esp;

  switch(*p) {

    case SYS_HALT:{
      shutdown_power_off();
      break;
    }

    case SYS_EXIT:{
      system_exit((int)*((uint32_t*)(f->esp + ARG_1)));
      break;
    }

    case SYS_EXEC:{
      //Processes file and chlid ID is returned to file
      f->eax = process_execute ((char *)*((uint32_t*)(f->esp + ARG_1)));
      break;
    }

    case SYS_WAIT:{
      //Process wait and return if successful
      f->eax = process_wait(*((uint32_t*)(f->esp + ARG_1)));
      break;
    }

    case SYS_CREATE:{
      //Creates file, and returns true if successful
      f->eax = filesys_create(
        (char *)*((uint32_t*)(f->esp + ARG_1)), //File Name
        (unsigned)*((uint32_t*)(f->esp + ARG_2)) //File Size
      );
      break;
    }

    case SYS_REMOVE:{
      //Removes file, and returns true if successful
      f->eax  = filesys_remove(
        (char *)*((uint32_t*)(f->esp + ARG_1)) //File name
      );
      break;
    }

    case SYS_OPEN:{
      //Opens file, returns file descriptor (or -1 if unsuccessful)
      char *files_name = ((char *)*((uint32_t*)(f->esp + ARG_1)));
      f->eax = open_file(files_name);
      break;
    }

    case SYS_FILESIZE:{
      struct file_info *fi = get_file ((int)*((uint32_t*)(f->esp + ARG_1)));
      //Exits if file doesnt exist
      if (fi == NULL) {
        system_exit(-1);
      }
      //Returns file length
      f->eax = file_length (fi->fp);
      break;
    }

    case SYS_READ:{
      //Set file descriptor, buffer, and size from arguments
      int fd = *((uint32_t*)(f->esp + ARG_1));
      void *buffer = ((uint32_t*)(f->esp + ARG_2));
      unsigned file_size = *((uint32_t*)(f->esp + ARG_3));
      struct file_info *fi;
      //Read from stdin (if that is buffer location)
      if(fd == STDIN_FILENO) {
        for(int i = 0; i < (int)file_size; i++) {
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
      //Read bytes from file and return byte count
      f->eax = file_read(fi->fp, buffer, file_size);
      break;
    }

    case SYS_WRITE:{
      //Set file descriptor, buffer, and size from arguments
      int fd = *((uint32_t*)(f->esp + ARG_1));
      const void *buffer = ((uint32_t*)(f->esp + ARG_2));
      unsigned int file_length = *((uint32_t*)(f->esp + ARG_3));

      if(fd == 1) { //Write to the console and return file length
      putbuf((const char *)buffer, (size_t) file_length);
        f->eax = file_length;
      }
      else { //Get file to write to
        struct file_info *fi = get_file(fd);
        if(fi != NULL) { //Returns number of bytes written
          f->eax = file_write (fi->fp, buffer, file_length);
        }
        else { //If file doesnt exist then cannot write
          f->eax = 0;
        }
      }
      break;
    }

    case SYS_TELL:{
      //Return position of next byte to be read
      struct file_info *fi = get_file((int)*((uint32_t*)(f->esp + ARG_1)));
      if(fi != NULL) {
         f->eax = file_tell (fi->fp);
      }
      else {
        f->eax = 0;
      }
      break;
    }

    case SYS_SEEK:{
      //Get file descriptor and position
      int fd = (int) fetch_args(f,ARG_1);
      unsigned position = (unsigned) fetch_args(f,ARG_2);

      //Seek through file if it exists
      struct file_info *fi = get_file(fd);
      if (fd != (int)NULL) {
        file_seek(fi->fp,position);
      }
      break;
    }

    case SYS_CLOSE:{
      int fd = ((int)fetch_args(f,ARG_1));
      struct file_info *fi;

      //Closes file and removes it from list in struct
      fi = get_file(fd);
      if (fi != NULL) {
        file_close(fi->fp);
        list_remove(&fi->fpelem);
        free(fi);
      }
      break;
    }
  }
}
