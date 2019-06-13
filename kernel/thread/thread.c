// why we have to use thread -> the main purpose is to protect the resource the process flow is now using and make switching possible

#include"thread.h"
#include"print.h"
#include"string.h"
#include"int.h"
#include"global.h"
#include"bitmap.h"
#include"debug.h"
#include"bitmap.h"
#include"process.h"

#define PAGE_SIZE 4096

// the context switch function by assembly
extern void switch_to(struct task_struct * cur,struct task_struct * next);

// global variable including main thread, waitng list, pending list
struct _list thread_ready_list;

struct _list thread_all_list;

struct task_struct * main_thread_ptr;

// storage of the _node in the ready list ?????? (i don't understand why here)
struct _node * thread_node_ptr;

// the lock of the pid
struct LOCK pid_lock;


//---------------------------------------------------------------------------------------------------------------------------------------------------


// allocate pid for the thread
static pid_t allocate_pid(){
    static pid_t next_pid = 0;
    acquire_lock(&pid_lock);
    next_pid ++;
    release_lock(&pid_lock);
    return next_pid;
}

// return the task_struct pointer of the running thread
struct task_struct * running_thread(){
    uint32_t esp;
    asm volatile(
    "mov %%esp, %0"
    :"=g"(esp)
    :
    :
    );
    return (struct task_struct *)(esp & 0xfffff000);
}  

// the kernel thread called the passed function
static void kernel_thread(thread_func * function,void * func_arg){
// it is essential to open the interrupt before executing function to avoid scheduling problem
    intopen();
    function(func_arg);
}

// put the function and the function argument to the location at thread stack (this function is called after init_thread)
void thread_create(struct task_struct * pthread_ptr, thread_func function, void * func_arg){

// at the start of stack we reserved space for the intr_stack struct
    pthread_ptr->self_kstack -= sizeof(struct intr_stack);

// also we should reserve for the thread_stack
    pthread_ptr->self_kstack -= sizeof(struct thread_stack);

// and now we want to initialize the struct thread_stack
    struct thread_stack * k_stack_ptr = (struct thread_stack *) (pthread_ptr->self_kstack);
    k_stack_ptr->eip = kernel_thread;
    k_stack_ptr->function = function;
    k_stack_ptr->func_arg = func_arg;
    k_stack_ptr->ebp = k_stack_ptr-> ebx = k_stack_ptr-> edi = k_stack_ptr-> esi = 0;
}

// init the basic informations for the thread
void init_thread(struct task_struct * pthread_ptr,char * name,int prio){
// clear the page
    memset(pthread_ptr,0,PAGE_SIZE);
// get the pid for the thread
    pthread_ptr->pid = allocate_pid();
    strcpy(pthread_ptr->name,name);

// main thread and other thread
    if(pthread_ptr == main_thread_ptr){
        pthread_ptr->status = TASK_RUNNING;
    }
    else{
        pthread_ptr->status = TASK_READY;
    }


// the stack should be at the top of the PCB page
    pthread_ptr->self_kstack = (uint32_t *)((uint32_t) pthread_ptr + PAGE_SIZE);
    pthread_ptr->priority = (uint8_t) prio;
    pthread_ptr->ticks = (uint32_t) prio;
    pthread_ptr->elapsed_ticks = 0;
    pthread_ptr->pg_dir = 0;

    pthread_ptr->stack_protector = 0x19920706;
}

// establish a thread according to the name, priority, function and function args and also allocate a page for the task_struct (PCB) in the kernel
struct task_struct * thread_start(char * name,thread_func function,void * func_arg,int prio){
    struct task_struct * thread = (struct task_struct *) get_kernel_pages(1);
    init_thread(thread,name,prio);
    thread_create(thread,function,func_arg);

/* now we have to put the stack pointer to esp and pop the status of registers 
    asm volatile(
    "mov %0, %%esp \n\t"
    "pop %%ebp \n\t"
    "pop %%ebx \n\t"
    "pop %%edi \n\t"
    "pop %%esi \n\t"
    "ret"
    :
    :"g"(thread->self_kstack)
    :"memory"
    );
*/
    
// to run the thread we should put the thread tag into ready_list and all_list
    _list_add_back( &thread_ready_list , &(thread->general_tag) );
    _list_add_back( &thread_all_list , &(thread->all_list_tag) );

    return thread;
}

static void make_main_thread(){
    main_thread_ptr = running_thread();
    init_thread( main_thread_ptr , "main_thread" , 60 );
// we don't have to put the main thread into the thread_ready_list since it's already running    
    _list_add_back( &thread_all_list , &(main_thread_ptr->all_list_tag) );
}

// the scheduler function whcih calls switch_to
/*
    the things that scheduler should do
    1. recognize why the thread is sceduled out
    2. set the TASK status for the pcb
    3. move the current thread to thread_list
    4. get the next thread pcb
    5. called the switch_to
*/

// change the priority using the thread name
void set_prio(char * name,uint8_t prio){
    struct task_struct * tptr = elem2entry( struct task_struct, all_list_tag , _list_front(&(thread_all_list)) );
    if(string_equal(name,tptr->name)){
        tptr->priority = prio;
    }
}

// BIG question ( Why scheduler don't have to diable interrupt?????????????)
void sched(){

// when scheduling, the interrupt should be disabled
// ASSERT( !intr_status() );

    struct task_struct * cur = running_thread();

// if the thread is scheduling out since the time strucstruct t skice is up then we append the thread ready list
    if(cur->status == TASK_RUNNING){
        _list_add_back( &(thread_ready_list) , &(cur->general_tag) );
// reset the time slice
        cur->ticks = cur->priority;
// reset the task status
        cur->status = TASK_READY;
    }
// if the thread is not ready, do nothing
    
// thread pointer reset
    thread_node_ptr = 0;

// ASSERT( !_list_empty( &(thread_ready_list) ) );

// get the next thread pointer
    thread_node_ptr = _list_pop_front( &thread_ready_list );

// get the pcb for thread pointer
    struct task_struct * next = elem2entry( struct task_struct , general_tag , thread_node_ptr );

    next->status = TASK_RUNNING;

// load the page_dir into the cr3 register if it is user program
    process_activate(next);

    switch_to( cur , next );
}

// the thread initaialization of all
void thread_init(){
    _list_init(&thread_ready_list);
    _list_init(&thread_all_list);
// init the pid lock
    init_lock(&pid_lock);
    make_main_thread();
}

// block the running thread and set the status to input
void thread_block(enum task_status status){
// for debugging
//    put_str("start block  ");

// get the interrupt status
    int intr = intr_status();
// close the interrupt for the purpose of atomic process
    intclose();
    
    struct task_struct * task_ptr = running_thread();
    
    task_ptr->status = status;

// for debugging
//    put_str("end block  ");

// call the scheduler
    sched();

// NOTICE!!!! the restore of interrupt status after the thread is unblocked !!!!
    if(intr) intopen();
}

// unblock the thread and put it into the thread ready list
void thread_unblock(struct task_struct * task_ptr){
// ASSERT(!_list_find(&(thread_ready_list),&(task_ptr->general_tag)));

// for debugging
//    put_str("start unblock  ");

    int intr = intr_status();

    intclose();

// change the thread status
    task_ptr->status = TASK_READY;
    _list_add_front( &thread_ready_list , &(task_ptr->general_tag) );

// for debugging
//    put_str("finish unblock  ");
    
    if(intr) intopen();
}





