#include "sync.h"
#include "thread.h"
#include "print.h"
#include "list.h"
#include "int.h"
#include "stdint.h"

// init the semaphore
void init_sema(struct semaphore * sema_ptr,uint8_t value){
    sema_ptr->value = value;
    _list_init(&(sema_ptr->waiters));
}

// init the lock
void init_lock(struct lock * lock_ptr){
    lock_ptr->holder = 0;
    lock_ptr->holder_repeat_num = 0;
    init_sema(&(lock_ptr->semaphore),1);
}

// require a lock
void acquire_lock(struct lock * lock_ptr){
    struct task_struct * thread_ptr = running_thread();
// if thread_ptr equals to lock holder --->>> increase the repeat number
    if(thread_ptr == lock_ptr->holder){
        lock_ptr->holder_repeat_num++;
    }
// else try to get the lock or blocked
    else{
        sema_down(&(lock_ptr->semaphore));
// if get the lock
        lock_ptr->holder = thread_ptr ;
        lock_ptr->holder_repeat_num = 1;
    }
}

// relealse lock
void release_lock(struct lock * lock_ptr){
// if lock_ptr have request many times just decrese
    ASSERT(lock_ptr->holder == running_thread());
    if( lock_ptr->holder_repeat_num > 1 ){
        (lock_ptr->holder_repeat_num)--;    
    }

// else unblock others
    else{
        lock_ptr->holder_repeat_num = 0;
        lock_ptr->holder = 0;
        sema_up(&(lock_ptr->semaphore));
    }
}

// semaphore up value (atmic)
void sema_up(struct semaphore * sema){
    int intr = intr_status();
    intclose();
    if( !_list_empty(&(sema->waiters)) ){
// get the thread in waiting list and unblock the thread
        struct task_struct * blocked_thread = elem2entry(struct task_struct , general_tag , _list_pop_front(&(sema->waiters))) ;
        thread_unblock(blocked_thread);
    }
    sema->value ++ ;
    if(intr) intopen();
}

// semaphore down and block the thread if the resource is unavailable (atomic)
void sema_down(struct semaphore * sema){
    int intr = intr_status();
    intclose();
    while(sema->value == 0){
// append the running thread to waiting list
        _list_add_back( &(sema->waiters) , &(running_thread()->general_tag) );
        thread_block(TASK_BLOCKED);
    }
    sema->value --;
    if(intr) intopen();
}


