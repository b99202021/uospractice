#ifndef __CONSOLE_H
#define __CONSOLE_H

#include "print.h"
#include "SYNC.h"

// init the console resource
void console_init();

// encapsulate all the print function
void console_put_char(uint8_t);

void console_put_int(uint32_t);

void console_put_str(char *);

#endif

