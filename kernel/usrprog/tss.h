#ifndef __TSS_H
#define __TSS_H

#include "thread.h"

// init the task state segment
void tss_init();

// update tss esp0
void update_tss_esp(struct task_struct *);

#endif
