#ifndef __STDIO_H
#define __STDIO_H

typedef char * va_list;

// print the format to str
uint32_t vsprintf(char *, const char *,va_list ap);

// the realization of printf (variable length function)
uint32_t printf(char *, ...);

#endif 
