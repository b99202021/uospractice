#ifndef __STRING_H
#define __STRING_H

#include"print.h"

void memset(void *,uint8_t,uint32_t);

void memcpy(void *,const void * ,uint32_t);

int memcmp(const void *,const void *,uint32_t);

void strcpy(char *,char *);

uint32_t strlen(char *);

int string_equal(const char *,const char *);

#endif
