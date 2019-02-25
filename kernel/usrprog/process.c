#include "tss.h"
#include "debug.h"
#include "bitmap.h" 
#include "thread.h"
#include "list.h"
#include "int.h"
#include "global.h"
#include "process.h"
#include "string.h"

#define default_prio 20

extern void intr_exit(void);

void start_process(void * filename_){
    void * function = filename_ ;
    struct task_struct * cur = running_thread();
// we want to initail the intr_stack and use iretd to change the provilege
    cur->self_kstack += sizeof(struct thread_stack);
    struct intr_stack * proc_stack = (struct intr_stack *) cur->self_kstack;
    proc_stack->edi = proc_stack->esi = proc_stack->ebp = proc_stack-> esp_dummy = proc_stack-> ebx = proc_stack-> ecx = proc_stack-> eax = 0;
    proc_stack->gs = 0;
    proc_stack->ds = proc_stack->es = proc_stack-> fs = SELECTOR_U_DATA;
    proc_stack->eip = function;
    proc_stack->cs = SELECTOR_U_CODE;
    proc_stack->eflags = (EFLAGS_IOPL_0|EFLAGS_MBS|EFLAGS_IF_1);
    proc_stack->esp = (void *)((uint32_t)get_a_page(user,USER_STACK3_VADDR)+PAGE_SIZE);
    proc_stack->ss = SELECTOR_U_DATA;
    asm volatile(
    "movl %0, %%esp \n\t"
    "jmp intr_exit"
    :
    :"g"(proc_stack)
    :"memory"
    );
}

// load the page directory to the cr3 register 
void page_dir_activate(struct task_struct * p_thread){
    uint32_t pagedir_phy_addr = 0x100000;
// get the physical address of the page directory
    if(p_thread->pg_dir !=0){
        pagedir_phy_addr = addr_v2p((uint32_t)p_thread->pg_dir);
    }
    asm volatile(
    "movl %0, %%cr3"
    :
    :"r"(pagedir_phy_addr)
    :"memory"
    );
}

void process_activate(struct task_struct * pthread){
// first load the page directory to the cr3 register
    page_dir_activate(pthread);

// if is user thread we have to update the esp0
    if(pthread->pg_dir){
        update_tss_esp(pthread);
    }
}

// get the memory space from kernel space since we cannnot let user access the page diretory
// the function return the virtual address of the page directory
uint32_t * create_page_dir(){
    uint32_t * page_dir_vaddr = get_kernel_pages(1);
// map the kernel memeory to the same pages
    memcpy( (void *) ( (uint32_t) page_dir_vaddr + 0x300 * 4) , (void *)(0xfffff000 + 0x300 * 4) , 0x100 * 4 );
// and we have to map the directory address to the last page
    uint32_t new_dir_phy_dir = addr_v2p((uint32_t)page_dir_vaddr);
    page_dir_vaddr[1023] = new_dir_phy_dir | PG_US_U | PG_WR_W | PG_P ;
    return page_dir_vaddr;
}

// create the bitmap for the user program (just map all of them)
void create_user_vaddr_bitmap(struct task_struct * user_prog){
// follow the linux, the user program start at 0x08048000
    user_prog->userprog_vaddr.vaddr = USER_VADDR_START;
// how many page do we need to hold the virtual bitmap
    uint32_t pg_cnt = DIV_ROUND_UP( (0xc0000000-USER_VADDR_START)/PAGE_SIZE/8 , PAGE_SIZE );
// get the pages from the kernel
    user_prog->userprog_vaddr.vbitmap.location_ptr = get_kernel_pages(pg_cnt);
// init zero and the bytelen
    user_prog->userprog_vaddr.vbitmap.byte_len = (0xc0000000-USER_VADDR_START)/PAGE_SIZE/8 ;
    bitmap_init( &(user_prog->userprog_vaddr.vbitmap) );
}

extern struct _list thread_ready_list, thread_all_list;

// establish the user process
void process_execute(void * filename,char * name){
// get a pcb from the kernel
    struct task_struct * thread = get_kernel_pages(1);
// init the thread
    init_thread(thread,name,default_prio);
// create the virtual bitmap
    create_user_vaddr_bitmap(thread);
// create the thread
    thread_create(thread,start_process,filename);
// create the page directory
    thread->pg_dir = create_page_dir();

// put the thread into the ready list
    int intr = intr_status();
    intclose();
    _list_add_back( &thread_ready_list , &thread->general_tag );
    _list_add_back( &thread_all_list , &thread->all_list_tag );
    if(intr) intopen();
}

