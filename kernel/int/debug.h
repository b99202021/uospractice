#ifndef __DEBUG_H
#define __DEBUG_H
#include"int.h"

#ifdef NDEBUG
    #define ASSERT(condition) ((void) 0)
#else
    #define ASSERT(condition) if (condition) {} else { intclose(); put_char('\n') ; put_str(#condition); while(1); }
#endif

#endif

