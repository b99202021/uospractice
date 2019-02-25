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

struct vaddr_bitmap kernel_vaddr_bitmap;


//--------------------------------------------------------------------------------------------


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
// fuck i just unintentionally used && as & and get protection error!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// and got stucked for about three days !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        memset( (void *)( (uint32_t)pte & 0xfffff000) , 0 , PAGE_SIZE );
    }

    ASSERT(!(*pte & 0x00000001));
// now we have to build the pte of course is not exist
    *pte = (paddr & 0xfffff000) | PG_P | PG_WR_W | PG_US_U ;
}

// this function return vaddr for cnt continuous memory
void * malloc(enum pool flag,uint32_t cnt){
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
    void * addr = malloc( kernel , cnt );
    if(addr){
        memset( addr , 0 , cnt*PAGE_SIZE );
    }
    release_lock(&(kernel_pool.lock));
    return addr;
}

// get cnt page memory for user memory and reset the memory to zero and return the virtual address
void * get_user_pages(uint32_t cnt){
    acquire_lock(&(user_pool.lock));
    void * addr = malloc( user , cnt );
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


