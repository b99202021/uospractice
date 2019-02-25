#include "console.h"

static struct LOCK console_lock;

void console_init(){
    init_lock(&(console_lock));
}

void console_put_char(uint8_t a){
    acquire_lock(&(console_lock));
    put_char(a);
    release_lock(&(console_lock));
}

void console_put_str(char * s){
    acquire_lock(&(console_lock));
    put_str(s);
    release_lock(&(console_lock));
}

void console_put_int(uint32_t n){
    acquire_lock(&(console_lock));
    put_int(n);
    release_lock(&(console_lock));
}

