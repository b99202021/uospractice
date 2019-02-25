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

void threada(void *);

void threadb(void *);

void initialize_all();

//struct LOCK lock;

int main(){

    initialize_all();

//    init_lock( &lock );

    put_str("now is in the main thread \n");

    set_prio("main_thread", 10);

// NOTICE : the thread start always open the interrupt
    thread_start("threada",threada, 0 , 10 );

    intopen();

    int i;

    while(1){
//        acquire_lock( &lock );
        console_put_char('m');
//        release_lock( &lock );
    }
    while(1);
}

void threada(void * arg){

    int i;

    while(1){
//        acquire_lock( &lock );
        console_put_char('a');
//        release_lock( &lock );
    }
    while(1);
}

/*void threadb(void * arg){
    while(1){
        acquire_lock( &lock );
        put_char('b');
        release_lock( &lock );
    }
}*/

void initialize_all(){
    idt_init();
    bitmap_init_all();
    timer_init();
    thread_init();
    console_init();
}
