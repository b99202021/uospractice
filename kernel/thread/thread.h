#ifndef __THREAD_H
#define __THREAD_H

#include"stdint.h"
#include"list.h"
#include"bitmap.h"

typedef void thread_func(void *);

// task_status is used in PCB
enum task_status{
    TASK_RUNNING,
    TASK_READY,
    TASK_BLOCKED,
    TASK_WAITING,
    TASK_HANGING,
    TASK_DIED
};

// intr_stack is the content push into the stack when interrupted
// this struct is used to protect the thread source when interrupt comes
struct intr_stack{

// the interrupt number
    uint32_t vec_no;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp_dummy;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

// while get into the kernel when privildge changes 
    uint32_t err_code;
    void (*eip) (void);
    uint32_t cs;
    uint32_t eflags;
    void * esp;
    uint32_t ss;
};

// for storage of the thread stack
// this struct is used to protect the thread source when we call switch_to
struct thread_stack {
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edi;
    uint32_t esi;

// at the first time the eip point to the kernel thread, while other times point to the return address of switch_to function
    void (*eip) (thread_func * func,void * fun_arg);
// the unused return address is just for parameter's reference and it would be replaced by the kernel thread address
    void (*unused_retaddr);
// the function pointer that the kernel thread is calling
    thread_func * function;
// the function's argument that the kernel thread is calling
    void * func_arg;
};

struct task_struct{
// the kernel thread's stack 
    uint32_t * self_kstack;
    enum task_status status;
// the thread priority
    uint8_t priority;
    char name[16];
// how many time the program left
    uint32_t ticks;
// how many time have the thread excutes
    uint32_t elapsed_ticks;
// the tag for thread in gerneral waiting ready queue
    struct _node general_tag;
// the node in list_all_tag
    struct _node all_list_tag;
// per process page table
    uint32_t * pg_dir;
// the virtual bitmap for user prog
    struct vaddr_bitmap userprog_vaddr;
// the magic number that we used to check if the task_struct is destroyed
    uint32_t stack_protector;
};

// get the current running thread program control block pointer
struct task_struct * running_thread();

static void kernel_thread(thread_func *,void *);

// put the function and the function argument to the location at thread stack (this function is called after init_thread)
void thread_create(struct task_struct *,thread_func, void *);

// init the basic informations for the thread
void init_thread(struct task_struct *,char *,int);

// establish a thread according to the name, priority, function and function args and also allocate a page for the task_struct (PCB) in the kernel
struct task_struct * thread_start(char*,thread_func,void *,int);

// the most important {the scheduler}
void sched();

// init all about thread
void thread_init();

// set the priority of the thread
void set_prio(char * name,uint8_t prio);

// get the current running thread
struct task_struct * running_thread();

// block itself
void thread_block(enum task_status);

// unblock the thread in the waiting list
void thread_unblock(struct task_struct *);

#endif 


