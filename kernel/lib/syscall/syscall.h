#ifndef __LIB_SYSCALL_H
#define __LIB_SYSCALL_H

#include "stdint.h"

// the number of syscalls
enum SYSCALL_NR{
    SYS_GETPID,
    SYS_WRITE
};

// the syscall of getpid
uint32_t getpid();

// the syscall of write
uint32_t write(char *);

#endif

