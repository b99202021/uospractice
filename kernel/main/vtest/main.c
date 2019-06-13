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

void initialize_all();

void k_thread();

void p_thread();

int num;

int main(){

    initialize_all();

    put_str("i am a kernel !\n");

    num = 0;

    thread_start("k_thread",k_thread,0,20);

    process_execute(p_thread,"p_thread");

    intopen();

    while(1){
    }

    return 0;
}

void k_thread(){
    while(1){
        console_put_int(num);
    }
}

void p_thread(){
    while(1){
        num ++;
    }
}


void initialize_all(){
    idt_init();
    bitmap_init_all();
    timer_init();
    thread_init();
    console_init();
    keyboard_init();
    tss_init();
}
