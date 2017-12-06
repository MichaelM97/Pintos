#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

//Library definitions
#include <stdio.h>
#include <syscall-nr.h>
#include "devices/shutdown.h" //For shutdown_power_off
#include "userprog/process.h" //For process_execute and process_wait
#include "threads/thread.h" //Retrieve thread and other relevent structs
#include "filesys/filesys.h" //Allows access to file system functions
#include "filesys/file.h" //Allows access to file functions
#include "threads/malloc.h" //Used for memory allocation for structs
#include "devices/input.h" //Used for getting input from terminal

//Define argument location codes
#define ARG_1 4
#define ARG_2 8
#define ARG_3 12

/* Function prototypes */

/*
* Name: syscall_init
* Arguments: N/A (void)
* Returns: N/A (void)
* Description:
Initialses the syscall_handler
*/
void syscall_init (void);

/*
* Name: get_file
* Arguments: int fd
* Returns: struct file_info
* Description:
Finds file in thread list and return its information
*/
static struct file_info* get_file (int fd);

/*
* Name: open_file
* Arguments: char *file_name
* Returns: int
* Description:
Opens file with a given name, returns the file descriptor
*/
static int open_file(char *file_name);

/*
* Name: system_exit
* Arguments: int exit_code
* Returns: N/A (void)
* Description:
Processes exiting the system, returns exit code to the child thread
*/
static void system_exit (int exit_code);
#endif /* userprog/syscall.h */
