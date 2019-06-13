# 1 "syscall-init.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "syscall-init.c"
# 14 "syscall-init.c"
typedef void * syscall;

syscall syscall_table[32];


uint32_t sys_getpid(){
    return running_thread()->pid;
}

void syscall_init(){
    syscall_table[0] = sys_getpid;
}
