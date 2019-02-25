#ifndef __PROCESS_H
#define __PROCESS_H

#define USER_STACK3_VADDR   (0xc0000000-0x1000)

#define USER_VADDR_START    0x08048000

// run the process
void start_process(void *);

// load the page directory to the cr3 register
void page_dir_activate(struct task_struct *);

// if it is necessary, update the esp0 in the program control block
void process_activate(struct task_struct *);

// get space from kernel and create the page directory
uint32_t * create_page_dir();

// create the bitmap for user prog
void create_user_vaddr_bitmap(struct task_struct *);

// start the user prog
void process_execute(void *,char *);

#endif
