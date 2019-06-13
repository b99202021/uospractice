// this if for 8253 timer init program 

// and also the registation of interrupt

#include"io.h"
#include"print.h"
#include"thread.h"
#include"debug.h"
#include"timer.h"
#include"int.h"

#define IRQ0_FREQ 200
#define INPUT_FRE 1193180
#define COUNTER0_VAL INPUT_FRE/IRQ0_FREQ
#define COUNTER0_PORT 0x40
#define COUNTER0_NO 0
#define COUNTER_MODE 2
#define READ_WRITE_LATCH 3
#define PIT_CONTROL_PORT 0x43

static void intr_timer_handler();

static void frequency_set(uint8_t counter_port,uint8_t counter_no,uint8_t rwl,uint8_t counter_mode,uint16_t counter_value){
    outb(PIT_CONTROL_PORT,(uint8_t)(counter_no << 6 | rwl << 4 | counter_mode <<1 ));
    outb(counter_port,(uint8_t)counter_value);
    outb(counter_port,(uint8_t)counter_value>>8);
}

void timer_init(){
    frequency_set(COUNTER0_PORT,COUNTER0_NO,READ_WRITE_LATCH,COUNTER_MODE,COUNTER0_VAL);
    register_handler(0x20,(void *)intr_timer_handler);
}

// how many time interrupt come since the computer boot 
uint32_t ticks;

static void intr_timer_handler(){
    struct task_struct * cur_thread = running_thread();

// ensure that the stack have not overflow to the stack protector

    ASSERT(cur_thread->stack_protector = 0x19920706);

    cur_thread->elapsed_ticks++;

// we also need to increase the ticks for the ticks

    ticks++;

    if(cur_thread->ticks == 0)
        sched();
    else{
        cur_thread->ticks--;
    }
}

