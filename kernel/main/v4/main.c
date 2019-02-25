#include "print.h"
#include "int.h"
#include "debug.h"
#include "bitmap.h"
#include "string.h"
#include "thread.h"

void threadtry(void *);

void initialize_all();

int main(){
    initialize_all();

    put_str("now is in the main thread \n");

    char msg[15]="hi there\n";
    
    thread_start("k_thread",threadtry,(void *)msg,200);

    while(1);
}

void threadtry(void * arg){
    int i;
    for(i=0;i<10;i++){
        put_str((char *)arg);
    }
    while(1);
}

void initialize_all(){
    idt_init();
    bitmap_init_all();
}
