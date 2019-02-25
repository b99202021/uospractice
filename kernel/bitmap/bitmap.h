#ifndef __BITMAP_H
#define __BITMAP_H

#include"stdint.h"
#include"print.h"
#include"SYNC.h"

struct bitmap {
// the size of the bitmap in bytes
    uint32_t byte_len;
// where is the location pointer
    uint8_t * location_ptr;
};

struct memory_pool {
    struct bitmap pool_bitmap;
// how many memory do the pool manages
    uint32_t pool_size;
// the start physical addr the pool manages
    uint32_t physic_addr;
// protect the sturture
    struct LOCK lock;
};

struct vaddr_bitmap{
    struct bitmap vbitmap;
// the start of the vaddr which is 0x00100000
    uint32_t vaddr;
};

enum pool{
    kernel,user
};


//-----------------------------------------------------------function declaration


// this function set the bitmap bit for 1 or 0
void bitmap_set( struct bitmap *,uint32_t,uint8_t ) ;

// clear the bitmap to zero
void bitmap_init(struct bitmap *) ;

// this function test the bit is 0 or 1 in the bitmap at location
int bitmap_state(struct bitmap * btmp,uint32_t location) ;

// this function initalize the memory pool and clear the bitmap to 0
void memory_pool_init(struct memory_pool *,uint32_t,int,uint32_t );

// this function init all the memory pool and the vaddr_bitmap for kernel and set 0
void bitmap_init_all();

//this function find the sequential cnt pages in btmp
int bitmap_scan(struct bitmap *,uint32_t);

// the function return the virtual address for cnt sequential memory blocks
static void * vaddr_get(enum pool,uint32_t);

// now we have to get the pdt address
uint32_t * pde_ptr(uint32_t);

// so as the pet address
uint32_t * pte_ptr(uint32_t);

//this function return back the physical address of one page from the physical bitmap
void * palloc(struct memory_pool *);

// this function take a vaddr and paddr and map then on the page table
void page_table_add(void * ,void * );

// this function return vaddr for cnt continuous memory
void * malloc(enum pool,uint32_t);

// get cnt page memory for kernel memory and reset the memory to zero and return the address
void * get_kernel_pages(uint32_t);

void * get_user_pages(uint32_t);

// get a page for selected page
void * get_a_page(enum pool,uint32_t);

// get the physical address for virtual address
uint32_t addr_v2p(uint32_t);
#endif
