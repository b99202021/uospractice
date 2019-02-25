#include "ioqueue.h"
#include "int.h"
#include "SYNC.h"
#include "global.h"
#include "debug.h"

// init the ioqueue
void init_ioqueue (struct ioqueue * io_ptr){
    init_lock(&(io_ptr->lock));
    io_ptr->producer = 0;
    io_ptr->consumer = 0;
    io_ptr->head = 0;
    io_ptr->tail = 0;
}

static uint32_t next_pos(uint32_t num){
    return (num+1) % bufsize;
}

uint8_t full_ioqueue (struct ioqueue * io_ptr){
    return next_pos(io_ptr->tail) == io_ptr->head;
}

uint8_t empty_ioqueue (struct ioqueue * io_ptr){
    return (io_ptr->head == io_ptr->tail);
}

// we should turn off the interrupt in the function
void ioq_put_char(struct ioqueue * io_ptr,char byte){
    ASSERT( !intr_status() );

    while( full_ioqueue(io_ptr) ){
        acquire_lock(&(io_ptr->lock));
        io_ptr->producer = running_thread();
        thread_block(TASK_BLOCKED);
        release_lock(&(io_ptr->lock));
    }

    if( io_ptr->consumer ){
        io_ptr->consumer = 0;
        thread_unblock(io_ptr->consumer);
    }

    io_ptr->buf[io_ptr->tail] = byte;
    io_ptr->tail = next_pos(io_ptr->tail);
}

// so as this funcition
char ioq_get_char(struct ioqueue * io_ptr){
    ASSERT( !intr_status() );

    while( empty_ioqueue(io_ptr) ){
// we have to protect the structure since there might be multiple comsumer
        acquire_lock(&(io_ptr->lock));
        io_ptr->consumer = running_thread();
        thread_block(TASK_BLOCKED);
// release the lock after unblocked
        release_lock(&(io_ptr->lock));
    }

    if(io_ptr->producer){
        io_ptr->producer = 0;
        thread_unblock(io_ptr->producer);
    }

    char ret = io_ptr->buf[io_ptr->head];
    io_ptr->head = next_pos(io_ptr->head);
    
    return ret;
}







