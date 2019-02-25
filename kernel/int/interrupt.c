#include "stdint.h"
#include "global.h"
#include "io.h"
#include "print.h"

// there are 33 interrupt service routine here
#define IDT_DESC_CNT 48

//here is the control for 8259A (interrupt controller)
#define PIC_M_CRTL 0x20
#define PIC_M_DATA 0x21
#define PIC_S_CRTL 0xa0
#define PIC_S_DATA 0xa1

//eflags register if ==1
#define EFLAGS_IF 0x200

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

intr_handler idt_table[IDT_DESC_CNT];

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
    outb (PIC_M_DATA, 0xfc);
    outb (PIC_S_DATA, 0xff);

    put_str("8259A initalized !! \n");
    return ;
}

// this part is for initializing interrupt name for purpose of easy debugging

char * intr_name[IDT_DESC_CNT];

//general handling function;
static void general(uint32_t vec_nr){
    if( vec_nr==0x27 || vec_nr==0x2f ) return ;
    
    //put_str("\n the interrupt number is :");
    //put_int(vec_nr);
    //put_char('\n');
    
// clear the top fo the tty for excetption message
    int i;
    set_cursor(0);
    for(i=0;i<320;i++){
        put_char(' ');
    }

    set_cursor(0);

    put_str("!!!!   exception happens   !!!!\n");

    set_cursor(88);
    put_str(intr_name[vec_nr]);

// interrupt number 14 is the page fault interrupt
    if(vec_nr == 14){
        uint32_t vaddr=0;
        asm volatile(
        "mov %%cr2, %0"
        :"=r"(vaddr)
        :
        :
        );
        put_str("the page fault address is: ");
        put_int(vaddr);
    }

    set_cursor(168);

    put_str("\n !!!!   excetption ends  !!!!");
    while(1);
}

// this is for initalize exception handling function
static void exception_init(){
    int i;
    intr_name[0]="divide error";
    intr_name[1]="debug exception";
    intr_name[2]="NMI interrupt";
    intr_name[3]="breakpoint exception";
    intr_name[4]="overflow exception";
    intr_name[5]="bound range exceed exception";
    intr_name[6]="invalid opcode";
    intr_name[7]="device not available";
    intr_name[8]="double fault exception";
    intr_name[9]="coprocessor segment overrun";
    intr_name[10]="invalid tss exception";
    intr_name[11]="segment not present";
    intr_name[12]="stack fault exception";
    intr_name[13]="gerneral protection exception";
    intr_name[14]="page fault";
    intr_name[15]="reserved";
    intr_name[16]="floating point error";
    intr_name[17]="aligment check exc";
    intr_name[18]="machine check exc";
    intr_name[19]="SIMD floating point exc";

    for(i=20;i<IDT_DESC_CNT;i++){
        intr_name[i]="unset yet";
    }
    for(i=0;i<IDT_DESC_CNT;i++){
        idt_table[i]=(intr_handler) general;
    }
    return ;
}

// the init all function 
void idt_init(){
    idt_desc_init();
    pic_init();
    exception_init();
    
    // this is the LDTR should be load to LDTR
    uint64_t idt_address=((sizeof(idt)-1) | ((uint64_t)((uint32_t)idt))<<16 );
    asm volatile ("lidt %0"::"m"(idt_address));
    put_str("idt inited!\n");
    return;
}

// open the interrupt if it's closed
void intopen(){
    int eflags=0;
    asm volatile(
    "pushfl \n\t"
    "popl %0"
    :"=g"(eflags)
    :
    :
    );
    if(!(eflags & EFLAGS_IF)){
        asm volatile(
        "sti"
        :
        :
        :
        );
    }
}

// close the interrupt if it's open
void intclose(){
    int eflags=0;
    asm volatile(
    "pushfl \n\t"
    "popl %0"
    :"=g"(eflags)
    :
    :
    );
    if(eflags & EFLAGS_IF){
        asm volatile(
        "cli"
        :
        :
        :
        );
    }
}

// get the interrupt status (return 1 if open) 
int intr_status(){
    int eflags=0;
    asm volatile(
    "pushfl \n\t"
    "popl %0"
    :"=g"(eflags)
    :
    :
    );
    if(eflags & EFLAGS_IF) return 1;
    else return 0;
}

// register the interrupt handler function 
void register_handler(uint32_t vec_no,void * function){
    idt_table[vec_no]=function;
}


























