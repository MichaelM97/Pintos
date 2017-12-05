#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

//Library definitions
#include <stdio.h>
#include <syscall-nr.h>
#include "devices/shutdown.h" //For shutdown_power_off
#include "userprog/process.h" //For process_execute and process_wait
#include "threads/thread.h" //Retrieve thread and other relevent structs

//Define argument location codes
#define ARG_1 4
#define ARG_2 8
#define ARG_3 12

//Function prototypes
void syscall_init (void);
static struct file_info* get_file (int fd);
#endif /* userprog/syscall.h */
