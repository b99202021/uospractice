#include"bitmap.h"
#include"stdint.h"
#include"print.h"
#include"debug.h"
#include"int.h"
#include"string.h"
#include"global.h"
#include"SYNC.h"
#include"thread.h"

#define PAGE_SIZE 4096

#define TOTAL_MEMORY 0x02000000

#define PHYSIC_BITMAP_ADDRESS 0xc009a000

#define used_page 256

#define K_HEAP_ADDR 0xc0100000

#define PDE_INX(addr) ((addr & 0xffc00000) >> 22)

#define PTE_INX(addr) ((addr & 0x003ff000) >> 12)


// three bitmap for physical, kernel virtual pool and user virtual pool
struct memory_pool kernel_pool, user_pool;

// the virtual bitmap of the kernel memory
struct vaddr_bitmap kernel_vaddr_bitmap;

// the memory description of the mem_block
struct mem_block_desc k_block_descs[DESC_CNT];

//--------------------------------------------------------------------------------------------

// this is for mem_block init
void block_desc_init(struct mem_block_desc * desc_array){
    uint16_t desc_idx, block_size=16;

    // initialize all the mem_block_desc
    for(desc_idx = 0 ; desc_idx < DESC_CNT ; desc_idx++ ){
        desc_array[desc_idx].block_size = block_size;
        // initalize the number of the mem_block in the arena
        desc_array[desc_idx].blocks_per_arena = (PG_SIZE - sizeof(struct arena))/block_size;
        // initalize of the free_list int the desc_array
        list_init(&desc_array[desc_idx].free_list);
        block_size *= 2;
    }
}

//---------------------------------------------------------------------------------------------


//this function set the bitmap bit for 1 or 0
void bitmap_set(struct bitmap * btmp,uint32_t location,uint8_t value){
    int numbyte=location/8,numbit=location%8;
    if(value){
        value = (value << numbit);
        *(btmp->location_ptr+numbyte) |= value;
    }
    else{
        value= (1<<numbit) ^ 0xff;
        *(btmp->location_ptr+numbyte) &= value;
    }
}

// clear the bitmap to zero
void bitmap_init(struct bitmap * btmp){
    int i;
    uint8_t * itr = btmp->location_ptr;
    for( i=0 ; i<btmp->byte_len ; i++ ){
        *itr++ = 0 ;
    }
}

// this function test the bit is 0 or 1 in the bitmap at location
int bitmap_state(struct bitmap * btmp,uint32_t location){
    int numbyte=location/8,numbit=location%8;
    return ((*(btmp->location_ptr+numbyte)>>numbit) & 1);
}

// this function initalize the memory pool and clear the bitmap to 0
void memory_pool_init(struct memory_pool * m_pool_ptr,uint32_t phy_addr,int pg_cnt,uint32_t location_addr){
    m_pool_ptr->pool_bitmap.byte_len = pg_cnt/8;
    m_pool_ptr->pool_bitmap.location_ptr = (uint8_t *)location_addr;
    
    m_pool_ptr->pool_size = PAGE_SIZE * pg_cnt;

    m_pool_ptr->physic_addr = phy_addr;

    bitmap_init( &m_pool_ptr->pool_bitmap );
}

// this function init all the memory pool and the vaddr_bitmap for kernel and set 0
void bitmap_init_all(){
    uint32_t all_memory = TOTAL_MEMORY;
    uint32_t used_memory = used_page * PAGE_SIZE + 0x00100000;
    uint32_t free_memory = all_memory - used_memory;

    uint32_t all_free_pages = free_memory/PAGE_SIZE;

// the kernel pages
    uint32_t kernel_pg_cnt = all_free_pages/2 ;

// the user pages
    uint32_t user_pg_cnt = all_free_pages - kernel_pg_cnt ;

// the kernel bitmap byte length
    uint32_t kbm_len = kernel_pg_cnt/8;

// the user bitmap byte kength
    uint32_t ubm_len = user_pg_cnt/8;

    uint32_t kernel_phy_start_addr = 0x100000 + PAGE_SIZE * used_page;

    uint32_t user_phy_start_addr = kernel_phy_start_addr + kernel_pg_cnt * PAGE_SIZE;

    uint32_t k_bitmap_addr = PHYSIC_BITMAP_ADDRESS;

    uint32_t u_bitmap_addr = k_bitmap_addr + kbm_len;

    memory_pool_init( &kernel_pool , kernel_phy_start_addr , kernel_pg_cnt , k_bitmap_addr );
    memory_pool_init( &user_pool , user_phy_start_addr , user_pg_cnt , u_bitmap_addr );

    kernel_vaddr_bitmap.vaddr = K_HEAP_ADDR;
    kernel_vaddr_bitmap.vbitmap.byte_len = kbm_len;
    kernel_vaddr_bitmap.vbitmap.location_ptr = (uint8_t *)(PHYSIC_BITMAP_ADDRESS + kbm_len + ubm_len);
    bitmap_init(&(kernel_vaddr_bitmap.vbitmap));

// initialize the memory pool lock
    init_lock( &(kernel_pool.lock) );
    init_lock( &(user_pool.lock) );

// initalization of the mem_block_desc array for the malloc system call
    block_desc_init(k_block_descs);
}

//this function find the sequential cnt pages in btmp
int bitmap_scan(struct bitmap * btmp,uint32_t cnt){
    int count=0,i=0;
    while(count < cnt  &&  i < 8*btmp->byte_len ){
        if(bitmap_state(btmp,i)==0){
            count++;
        }
        else{
            count=0;
        }
        i++;
    }
    if(count==cnt) return i-cnt;
    else return -1;
}

// the function return the virtual address for cnt sequential memory blocks
static void * vaddr_get(enum pool pf,uint32_t cnt){
    int bit_index_start = -1,i,vaddr_start = 0;
    if(pf==kernel){
        bit_index_start=bitmap_scan( &(kernel_vaddr_bitmap.vbitmap), cnt );
        if(bit_index_start == -1) return 0;
        for(i=0;i<cnt;i++){
            bitmap_set ( &(kernel_vaddr_bitmap.vbitmap) , bit_index_start+i ,1);
        }
        return (void * )( kernel_vaddr_bitmap.vaddr + PAGE_SIZE * bit_index_start );
    }
    else{
        struct task_struct * cur = running_thread();
        bit_index_start = bitmap_scan( &(cur->userprog_vaddr.vbitmap) , cnt );
        if(bit_index_start == -1) return 0;
        for(i=0;i<cnt;i++){
            bitmap_set( &(cur->userprog_vaddr.vbitmap) , bit_index_start , 1 );
        }
        vaddr_start = cur->userprog_vaddr.vaddr + bit_index_start * PAGE_SIZE;
        return (void *)vaddr_start;
    }
}

// now we have to get the pdt address
uint32_t * pde_ptr(uint32_t vaddr){
    uint32_t * pde = (uint32_t *) (0xfffff000 + PDE_INX(vaddr) * 4 );
}

// so as the pet address
uint32_t * pte_ptr(uint32_t vaddr){
    uint32_t * pte = (uint32_t *) (0xffc00000 + ((vaddr & 0xffc00000) >> 10 ) + PTE_INX(vaddr) * 4 );
}

//this function return back the physical address of one page from the physical bitmap
void * palloc(struct memory_pool * m_pool_ptr){
    int index = bitmap_scan( &(m_pool_ptr->pool_bitmap) , 1);
    bitmap_set( &(m_pool_ptr->pool_bitmap),index,1);
    return (void *) (m_pool_ptr->physic_addr + PAGE_SIZE * index);
}

// this function take a vaddr and paddr and map then on the page table
void page_table_add(void * _vaddr,void * _paddr){

// convert the address to uint32_t and get the pde and pte address for modification
    uint32_t vaddr = (uint32_t) _vaddr;
    uint32_t paddr = (uint32_t) _paddr;
//debug
/*
    put_str("vaddr:");
    put_int(vaddr);
    put_char(' ');
    put_str("paddr:");
    put_int(paddr);
    put_char('\n');
*/

    uint32_t * pde = pde_ptr(vaddr);
    uint32_t * pte = pte_ptr(vaddr);
        
// check if the pde is established
    if(!(*pde & 0x00000001)){
        *pde = ( (uint32_t)palloc( &kernel_pool ) & 0xfffff000 ) | PG_P | PG_WR_W | PG_US_U ;
// since it is the new allocated pde, we have to clean the page for pte
// fuck unintentionally used && as & and get protection error!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// and got stucked for about three days !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        memset( (void *)( (uint32_t)pte & 0xfffff000) , 0 , PAGE_SIZE );
    }

    ASSERT(!(*pte & 0x00000001));
// now we have to build the pte of course is not exist
    *pte = (paddr & 0xfffff000) | PG_P | PG_WR_W | PG_US_U ;
}

// this function return vaddr for cnt continuous memory
void * malloc_page(enum pool flag,uint32_t cnt){
    if( flag == kernel ){
        void * addr = vaddr_get( kernel, cnt );
        int i;
        for(i=0;i<cnt;i++){
            page_table_add(addr + i * PAGE_SIZE , palloc( &kernel_pool ) );
        }
        return addr;
    }
    else{
        return 0;
    }
}

//we should implent free function for returning back the memory
void free(enum pool,void * vaddr,int32_t pg_cnt);

// get cnt page memory for kernel memory and reset the memory to zero and return the address
void * get_kernel_pages(uint32_t cnt){
    acquire_lock(&(kernel_pool.lock));
    void * addr = malloc_page( kernel , cnt );
    if(addr){
        memset( addr , 0 , cnt*PAGE_SIZE );
    }
    release_lock(&(kernel_pool.lock));
    return addr;
}

// get cnt page memory for user memory and reset the memory to zero and return the virtual address
void * get_user_pages(uint32_t cnt){
    acquire_lock(&(user_pool.lock));
    void * addr = malloc_page( user , cnt );
    if(addr){
        memset( addr , 0 , cnt*PAGE_SIZE );
    }
    release_lock(&(user_pool.lock));
    return addr;
}

// get a page for selected virtual address
void * get_a_page(enum pool flag,uint32_t vaddr){
    struct memory_pool * pool_ptr = (flag == kernel) ? &kernel_pool : &user_pool;
    acquire_lock( &(pool_ptr->lock) );
    
    // first set the corresponding bitmap 
    struct task_struct * cur = running_thread();
    int32_t bit_idx = -1;

    if(cur->pg_dir != 0 && flag == user){
        bit_idx = (vaddr - cur->userprog_vaddr.vaddr)/PAGE_SIZE;
        //ASSERT(bit_idx > 0);
        bitmap_set(&(cur->userprog_vaddr.vbitmap),bit_idx,1);
    }
    else if(cur->pg_dir == 0 && flag == kernel){
        bit_idx = (vaddr - kernel_vaddr_bitmap.vaddr)/PAGE_SIZE;
    }
    else{
        //ASSERT("not allow kenrel alloc user space"=="");
    }

// get a physical page from the memory pool
    void * page_phyaddr = palloc(pool_ptr);
    if(page_phyaddr == 0) return 0;

// link the physical addr to virtual addr
    page_table_add( (void *) vaddr,page_phyaddr);
    release_lock( &(pool_ptr->lock));
    return (void *) vaddr;
}

// get the virtual address to physical address
uint32_t addr_v2p(uint32_t vaddr){
    uint32_t * pte = pte_ptr(vaddr);
    return ((*pte & 0xfffff000) + (vaddr & 0x00000fff));
}

// ---------------------------------------------------------------------
// this part is for the arena and malloc

// return the idx memory block address in the arena
static struct mem_block * arena2block(struct arena * a,uint32_t idx){
    return (struct mem_block *)((uint32_t)a + sizeof(struct arena) + idx * a->desc->block_size);
}

// return the arena address given the memory block address
static struct arena * block2arena(struct mem_block * b){
    return (struct mem_block *)((uint32_t)b & 0xfffff000);
}

// get size memory in the heap
void * sys_malloc(uint32_t size){
    enum pool PF;
    struct memory_pool * mem_pool;
    uint32_t pool_size;
    struct mem_block_desc * descs;
    struct task_struct * cur_thread = running_thread() ;
    
// which memory pool to get the memory and initalization of the variables
    if(cur_thread->pgidr == 0){
        PF = kernel;
        pool_size = kernel_pool.pool_size;
        mem_pool = &kernel_pool;
        descs = k_block_descs;
    }
    else{
        PF = user;
        pool_size = user_pool.pool_size;
        mem_pool =  &user_pool;
        descs = cur->thread->u_block_desc;
    }

    if(size < 0 || size > pool_size) return 0;

    struct arena * a;
    struct mem_block * b;

// get memory lock first to avoid multithread apply the same memory pool
    acquire_lock(&mem_pool->lock);

// if the size > 1024 then give the whole page
    if(size > 1024){
// how many pages do we need to allocate
        uint32_t page_cnt = DIV_ROUND_UP(size + sizeof(struct arena),PG_SIZE);
// get the memeory from malloc_page
        a = malloc_page(PF,page_cnt);
// if we can get memory
        if(a != 0){
// clear the page for memory
            memset((void *)a,0,page_cnt*PG_SIZE);
            a->desc = 0;
            a->cnt = page_cnt;
            a->large = 1;
            release_lock(&mem_pool->block);
            return (void *)(a+1);
        }
        else{
            release_lock(&mem_pool->block);
            return 0;
        }
    }
    else{
        uint8_t desc_idx;
// find the proper size of memory block for the allocation
        for(desc_idx = 0;desc_idx<DESC_CNT;desc_idx++){
            if(size<descs[desc_idx]) break;
        }

// if there is no free memory block in the arena
        if(_list_empty(&descs[desc_idx].free_list)){
// get one page for the arena for the page
            a = malloc_page(PF,1);
            if(a == 0){
                release_lock(&mem_block->lock);
                return 0;
            }
// clear the page for the arena
            memset((void *)a,0,PG_SIZE);
            
            a->desc = &descs[desc_idx];
            a->large = 0;
        // increase the available blocks in the arena
            a->cnt = descs[desc_idx].blocks_per_arena;
            uint32_t block_idx;

// close the interrupt while list append
            int intrstatus = intr_status();
            intclose();
            
            for(block_idx = 0 ; block_idx < descs[desc_idx].blocks_per_arena ; block_idx++){
                b = arena2block(a,block_idx);
            // add the free_node to the free_list
                _list_add_back(&a->desc->free_list,&b->free_node);
            }
            
            if(intrstatus) intopen();
        }
        
        // start to allocate the memory block
        b = elem2entry(struct mem_block,free_elem,_list_pop_front(&(descs[desc_idx].free_list)));
        // set the memory in the memory block to zero
        memset((void *)b,0,descs[desc_idx].block_size);

        // get the struct arena where b is in
        a = block2arena(b);
        a->cnt--;
        release_lock(&mem_pool->lock);
        return (void *)b;
    }
}

//-----------------------------------------------------
// this part is for memory free

// this is for free the pg_phys_addr to the memory pool
void pfree(uint32_t pg_phy_addr){
    struct memory_pool * mem_pool;
    uint32_t bit_idx = 0;
    if(pg_phy_addr >= user_pool.physic_addr){
        mem_pool = &user_pool;
        bit_idx = (pg_phy_addr - user_pool.physic_addr)/PG_SIZE;
    }
    else{
        mem_pool = &kernel_pool;
        bit_idx = (pg_phy_addr - kernel_pool.physic_addr)/PG_SIZE;
    }
    // clear the bitmap for the removed memory
    bitmap_set(&mem_pool->pool_bitmap,bit_idx,0);
}

// remove the page table entry corresponding to virtual address
static void page_table_pte_remove(uint32_t vaddr){
    uint32_t * pte = pte_ptr(vaddr);
    *pte &= ~PG_P_1;
    asm volatile ("invlpg %0"::"m"(vaddr):"memory");
}

// free the memory consecutive pg_cnt in the pte
static void vaddr_remove(enum pool pf,void * _vaddr, uint32_t pg_cnt){
    uint32_t bit_idx_start = 0, vaddr = (uint32_t) _vaddr , cnt = 0;

    // where is the memory in the pool
    if(pf == kernel){
        bit_idx_start = (vaddr - kernel_vaddr_bitmap.vaddr)/PG_SIZE;
        while(cnt < pg_cnt){
            bitmap_set(&kernel_vaddr_bitmap.vbitmap,bit_idx_start+cnt++,0);
        }
    }
    else{
        struct task_struct * cur_thread = running_thread();
        bit_idx_start = (vaddr - cur_thread -> user_prog_vaddr.vaddr)/PG_SIZE;
        while(cnt < pg_cnt){
            bitmap_set(&kernel_vaddr_bitmap.vbitmap,bit_idx_start+cnt++,0);
        }
    }
}

// free the vaddr conti cnt physical memory
void mfree_page(enum pool pf, void * _vaddr , uint32_t pg_cnt){
    uint32_t pg_phy_addr;
    uint32_t vaddr = (int32_t) _vaddr , page_cnt = 0 ;
    // find the physical memory of the virtual memeory
    pg_phy_addr = addr_v2p(vaddr);

    // find out where is the memory in
    if(pg_phy_addr >= user_pool.physic_addr){
        vaddr -= PG_SIZE;
        // clear the cnt page in the memory pool
        while(page_cnt < pg_cnt){
            vaddr += PG_SIZE;
            pg_phy_addr = addr_v2p(vaddr);
            // return the physical page to the mempry pool
            pfree(pg_phy_addr);
            // clear the pte of the virtual address
            page_table_remove(vaddr);
            page_cnt++;
        }
        vaddr_remove(pf,_vaddr,pg_cnt);
    }
    // the memory is in the kernel memory pool
    else{
        vaddr -= PG_SIZE;
        while(page_cnt < pg_cnt){
            vaddr += PG_SIZE;
            // find the physical page corresponding to the virtual memory
            pg_phy_addr = addr_v2p(vaddr);
            // return the physical to the memory pool
            pfree(pg_phy_addr);
            // clear the pte of the virtual address
            page_table_remove(vaddr);
            page_cnt++;
        }
        vaddr_remove(pf,_vaddr,pg_cnt);
    }
}

// this is for the sys free
void sys_free(void * ptr){
    // if ptr == 0 return
    if(ptr == 0) return;
    enum pool PF;
    struct memory_pool * mem_pool;

    // setup the pool and memory_pool
    if(running_thread()->pgdir){
        PF = user;
        mem_pool = &user_pool;
    }
    else{
        PF = kernel;
        mem_pool = &kernel_pool;
    }
    
    // acuire the lock of the memory pool
    acquire_lock(&mem_pool->lock);

    struct mem_block * b = ptr;
    struct arena * a = block2arena(b);








    



















