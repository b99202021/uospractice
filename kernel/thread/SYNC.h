#ifndef __LOCK_H
#define __LOCK_H

#include "list.h"

struct LOCK{
    struct task_struct * holder;
    struct _list waiters;
};

void init_lock(struct LOCK *);

void acquire_lock(struct LOCK *);

void release_lock(struct LOCK *);

#endif

