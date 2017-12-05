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
#include "devices/input.h"

//Define argument location codes
#define ARG_1 4
#define ARG_2 8
#define ARG_3 12

//Function prototypes
void syscall_init (void);
static struct file_info* get_file (int fd);
static int open_file(char*file_name);
static void system_exit (int exit_code);
#endif /* userprog/syscall.h */
