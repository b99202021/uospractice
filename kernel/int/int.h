#ifndef __INTERRUPT_H
#define __INTERRUPT_H

#include"print.h"

void idt_init();

void intclose();

void intopen();

// if (return num) interrupt is opened else closed
int intr_status();

void register_handler(uint32_t , void *);

#endif
