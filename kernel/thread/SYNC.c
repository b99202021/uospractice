#include "SYNC.h"
#include "thread.h"
#include "print.h"
#include "int.h"
#include "debug.h"
#include "list.h"

void init_lock(struct LOCK * lock_ptr){
    lock_ptr->holder = 0;
    _list_init(&(lock_ptr->waiters));
}

void acquire_lock(struct LOCK * lock_ptr){
    int intr = intr_status();
    intclose();
    struct task_struct * thread_ptr = running_thread();
// here we should use while cause that we dont know when thread awakes the lock is available    
    while(lock_ptr->holder){
        _list_add_back( &(lock_ptr->waiters) , &(thread_ptr->general_tag) );
        thread_block(TASK_BLOCKED);
    }
    
    lock_ptr->holder = thread_ptr;

    if(intr) intopen();
}

void release_lock(struct LOCK * lock_ptr){
    ASSERT( lock_ptr->holder == running_thread() );
    int intr = intr_status();
    intclose();
    lock_ptr->holder = 0;
    if( !_list_empty( &(lock_ptr->waiters) ) ){
        thread_unblock( elem2entry( struct task_struct , general_tag , _list_pop_front( &(lock_ptr->waiters) ) ) );
    }
    if(intr) intopen();
}
