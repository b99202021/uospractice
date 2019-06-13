#ifndef __IOQUEUE_H
#define __IOQUEUE_H

#include "stdint.h"
#include "SYNC.h"

#define bufsize 64

struct ioqueue{
    struct LOCK lock;
    struct task_struct * producer;
    struct task_struct * consumer;
    char buf[bufsize];
    uint32_t head;
    uint32_t tail;
};

void init_ioqueue(struct ioqueue *);

uint8_t full_ioqueue(struct ioqueue *);

uint8_t empty_ioqueue(struct ioqueue *);

void ioq_put_char(struct ioqueue *,char);

char ioq_get_char(struct ioqueue *);

#endif 
















