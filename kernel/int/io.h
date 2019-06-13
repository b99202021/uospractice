//notice that for the purpose of efficiency we should make all the function call inline
//as i did not owing to the simplicity

#ifndef __LIB_IO_H
#define __LIB_IO_H

#include"stdint.h"

static void outb(uint16_t port, uint8_t data){
// tje rep means repeat until ecx equals to zero while "+" means first written
    asm volatile(
    "outb %b0, %w1"
    :
    :"a"(data), "d"(port)
    :
    );
}

static void outsw(uint16_t port, const void * addr, uint32_t count){
    asm volatile(
    "cld \n\t"
    "rep outsw"
    :"+S"(addr), "+c"(count)
    :"d"(port)
    :
    );
}

static uint8_t inb(uint16_t port){
    uint8_t data;
    asm volatile(
    "inb %w1, %b0"
    :"=a"(data)
    :"Nd"(port)
    :
    );
    return data;
}

static void insw(uint16_t port, const void * addr, uint32_t count){
    asm volatile(
    "cld \n\t"
    "rep insw"
    :"+D"(addr),"+c"(count)
    :"d"(port)
    :"memory"
    );
}

#endif
