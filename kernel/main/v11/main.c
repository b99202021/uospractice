#include "print.h"
#include "int.h"
#include "debug.h"
#include "bitmap.h"
#include "string.h"
#include "thread.h"
#include "list.h"
#include "timer.h"
#include "console.h"
#include "SYNC.h"
#include "keyboard.h"
#include "ioqueue.h"
#include "tss.h"
#include "process.h"
#include "syscall.h"
#include "syscall-init.h"


void initialize_all();

void k_thread();

void p_thread();

int num;

int main(){

    initialize_all();

    put_str("i am a kernel !\n");

    num = 0;

    process_execute(p_thread,"p_thread");

    thread_start("k_thread",k_thread,0,20);
    
    intopen();

    while(1) ;

    return 0;
}

void k_thread(){
    while(1){
        console_put_int(num);
    }
}

void p_thread(){
    num = getpid();
    while(1) ;
}


void initialize_all(){
    idt_init();
    bitmap_init_all();
    timer_init();
    thread_init();
    console_init();
    keyboard_init();
    tss_init();
    syscall_init();
}
