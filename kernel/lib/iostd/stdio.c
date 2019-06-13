#include "string.h"
#include "stdint.h"
#include "syscall.h"
#include "stdio.h"

// va_start, va_arg, va_end is for variable length parameters
// va_list is just typedef char * va_list

#define va_start(ap,v) ap=(va_list)&v

#define va_arg(ap,t) *((t*)(ap += 4))

#define va_end(ap) ap = 0

// change interger into ascii
// notice that we use pointer to pointer because we are changing the pointer's address
// recursive call for the interger transfer (function call overhead)
void itoa(uint32_t value,char ** buf_ptr_addr,uint8_t base){
    uint32_t m = value % base , i = value / base;
    if(i) itoa(i,buf_ptr_addr,base);
    if(m<10) *((*buf_ptr_addr)++) = m + '0';
    else *((*buf_ptr_addr)++) = m -10 + 'A';
}

// print the ap according to format to string
uint32_t vsprintf(char * str, const char * format, va_list ap){
    const char * index_ptr = format;
    char index_char = *index_ptr;
    char * buf = str;
    int32_t arg_int;
    char * arg_str;
    while(index_char){
        if(index_char != '%'){
            *(buf++) = index_char;
            index_char = *(++index_ptr);
            continue;
        }
    // notice that the switch can only be lvalue without ++ or --
        switch (index_char) {
            case 's':
                arg_str = va_arg(ap,char *);
                strcpy(buf,arg_str);
                buf += strlen(arg_str);
                index_char = *(++index_ptr);
                break;
            case 'c':
                *(buf++) = va_arg(ap,char);
                index_char = *(++index_ptr);
                break;
            case 'd':
                arg_int = va_arg(ap,int);
                if(arg_int < 0){
                    *(buf++) = '-';
                    arg_int *= -1;
                }
                itoa(arg_int,&buf,10);
                index_char = *(++index_ptr);
                break;
            case 'x':
                arg_int = va_arg(ap,int);
                itoa(arg_int,&buf,16);
                index_char = *(++index_ptr);
                break;
        }
    }
    return strlen(str);
}

// sprint the format to the buffer
uint32_t sprintf(char * buf,const char * format, ...){
    va_list args;
    uint32_t ret;
    va_start(args,format);
    ret = vsprintf(buf,format,args);
    va_end(args);
    return ret; 
}

uint32_t printf(const char * format, ...){
    va_list args;
// let args point to format
    va_start(args,format);
// the input buffer
    char buf[1024];
    vsprintf(buf,format,args);
// let args point to NULL
    va_end(args);
    return write(buf);
}







