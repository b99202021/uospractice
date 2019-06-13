// remeber that we are writing a single core os
// thus we don't use busy waiting
// when the thread request resource
// we always put them into waiting list
// for other thread holding resource to wake them up

#ifndef __SYNC_H
#define __SYNC_H

#include "stdint.h"
#include "list.h"
#include "thread.h"

// using binary semaphore to realize lock
struct semaphore{
// how many resources
    uint8_t value;
// the list for the waiting threads need to be waken up
    struct _list waiters;
};

struct lock{
// the thread's pcb that is holding the semaphore
    struct task_struct * holder;
// lock realized by semaphore
    struct semaphore semaphore;
// how many times the holder have request the same lock
    uint32_t holder_repeat_num;
};

// init the sema
void init_sema(struct semaphore *,uint8_t);

// init the lock
void init_lock(struct lock *);

// acquire the lock
void acquire_lock(struct lock *);

// release the lock
void release_lock(struct lock *);

void sema_up(struct semaphore *);

void sema_down(struct semaphore *);

#endif

