#include "syscall.h"

// this is called statements expression in c where the return value is the last term in the curl

// ths last thing in the compound statement shpuld be an expression followed by a semicolon

#define _syscall0(NUMBER) ({    \
    int retval;                 \
    asm volatile (              \
    "int $0x80"                 \
    :"=a"(retval)               \
    :"a" (NUMBER)               \
    :"memory"                   \
    );                          \
    retval;                     \
})

#define _syscall1(NUMBER,ARG1) ({    \
    int retval;                 \
    asm volatile (              \
    "int $0x80"                 \
    :"=a"(retval)               \
    :"a" (NUMBER), "b"(ARG1)               \
    :"memory"                   \
    );                          \
    retval;                     \
})

#define _syscall2(NUMBER,ARG1,ARG2) ({    \
    int retval;                 \
    asm volatile (              \
    "int $0x80"                 \
    :"=a"(retval)               \
    :"a" (NUMBER), "b"(ARG1), "c"(ARG2)   \
    :"memory"                   \
    );                          \
    retval;                     \
})

#define _syscall3(NUMBER,ARG1,ARG2,ARG3)    ({  \
    int retval;                                 \
    asm volatile (                              \
    "int $0x80"                                 \
    : "=a" (retval)                             \
    : "a" (NUMBER), "b"(ARG1), "c"(ARG2), "d"(ARG3) \
    : "memory"                                  \
    );                                          \
    retval;                                     \
})

// getpid has no parameter
uint32_t getpid(){
    return _syscall0(SYS_GETPID);
}

uint32_t write(char * str){
    return _syscall1(SYS_WRITE,str);
}




