#include "print.h"
#include "int.h"
#include "debug.h"
#include "bitmap.h"
#include "string.h"
#include "thread.h"
#include "list.h"
#include "timer.h"

void threada(void *);

void threadb(void *);

void initialize_all();

int main(){
    initialize_all();

    put_str("now is in the main thread \n");

    set_prio("main_thread", 10);
    
    thread_start("threada",threada, 0 , 10 );

    thread_start("threadb",threadb, 0 , 10 );

    intopen();

    while(1) {
        intclose();
        put_str("main ");
        intopen();
    }
}

void threada(void * arg){
    while(1) {
        intclose();
        put_str("arga ");
        intopen();
    }
}

void threadb(void * arg){
    while(1){
        intclose();
        put_str("argb ");
        intopen();
    }
}

void initialize_all(){
    idt_init();
    bitmap_init_all();
    timer_init();
    thread_init();
}
