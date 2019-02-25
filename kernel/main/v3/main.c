#include "print.h"
#include "int.h"
#include "debug.h"
#include "bitmap.h"

int main(){
    put_char('\n');
    put_str("This is the TW micro OS ! \n");

// first init the interrupt table
    idt_init();

// second init the bitmap for memory management
    bitmap_init_all();
/*    
// now we should open the interrupt by setting interrupt flag IF on
    asm volatile("sti");

    ASSERT(1==2);

    while(1);
*/

    void * addr = get_kernel_pages(2);
    put_str("now we get kernel pages with address start at : \n");
    put_int( (uint32_t)addr );
    put_char('\n');

    while(1);

    return 0;
}
