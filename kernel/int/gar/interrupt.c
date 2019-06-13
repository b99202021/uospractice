#include "stdint.h"
#include "global.h"
#include "io.h"
#include "print.h" 

// there are 33 interrupt service routine here
#define IDT_DESC_CNT 33

//here is the control for 8259A (interrupt controller)
#define PIC_M_CRTL 0x20
#define PIC_M_DATA 0x21
#define PIC_S_CRTL 0xa0
#define PIC_S_DATA 0xa1

typedef void * intr_handler;

// this is the interrupt descriptor
struct gate_desc{
    uint16_t func_offset_low;
    uint16_t selector;
    uint8_t unused;
    uint8_t attribute;
    uint16_t func_offset_high;
};

// import the interrupt handler
extern intr_handler intr_entry_table[IDT_DESC_CNT];

static struct gate_desc idt[IDT_DESC_CNT];

//make struct gate_desc using attr and handler
static void make_idt_desc(struct gate_desc* gate_ptr, uint8_t attr,intr_handler function){
    gate_ptr->func_offset_low=(uint32_t)function & 0x0000ffff;
    gate_ptr->selector=SELECTOR_K_CODE;
    gate_ptr->unused=0;
    gate_ptr->attribute=attr;
    gate_ptr->func_offset_high=((uint32_t)function & 0xffff0000)>>16;
    return ;
}

static void idt_desc_init(void){
    int i;

    // make all the interrupt desc table for the attr and the service function
    for(i=0;i<IDT_DESC_CNT;i++){
        make_idt_desc(&idt[i],IDT_ATTR_DPL0,intr_entry_table[i]);
    }
    put_str("idt is established!!! \n");
}

static void pic_init(void){
    // first we should initialize the ICW1~ICW4 for master 8259A
    outb (PIC_M_CRTL, 0x11);
    outb (PIC_M_DATA, 0x20);
    outb (PIC_M_DATA, 0x04);
    outb (PIC_M_DATA, 0x01);
    
    // second initialize the slave 8259A
    outb (PIC_S_CRTL, 0x11);
    outb (PIC_S_DATA, 0x28);
    outb (PIC_S_DATA, 0x02);
    outb (PIC_S_DATA, 0x01);
    
    // only open the time-interrupt
    outb (PIC_M_DATA, 0xfe);
    outb (PIC_S_DATA, 0xff);

    put_str("8259A initalized !! \n");
    return ;
}

void idt_init(){
    idt_desc_init();
    pic_init();
    
    // this is the LDTR should be load to LDTR
    uint64_t idt_address=((sizeof(idt)-1) | ((uint64_t)((uint32_t)idt))<<16 );
    asm volatile ("lidt %0"::"m"(idt_address));
    put_str("idt inited!\n");
    return;
}



