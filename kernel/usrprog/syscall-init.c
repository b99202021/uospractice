
#include "thread.h"
#include "syscall-init.h"
#include "syscall.h"
#include "string.h"
#include "console.h"

/*
enum SYSCALL_NR{
    SYS_GETPID
};
*/

// how many syscalls 
#define syscall_nr 32

typedef void * syscall;

syscall syscall_table[syscall_nr];

// get current task pid
uint32_t sys_getpid(){
    return running_thread()->pid;
}

// write to the console
uint32_t sys_write(char * str){
    console_put_str(str);
    return strlen(str);
}

void syscall_init(){
    syscall_table[0] = sys_getpid;
    syscall_table[1] = sys_write;
}


