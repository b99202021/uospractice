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

void initialize_all();

//struct LOCK lock;

int main(){

    initialize_all();

    intopen();

    while(1) ;

    return 0;

}

void initialize_all(){
    idt_init();
    bitmap_init_all();
    timer_init();
    thread_init();
    console_init();
    keyboard_init();
}
