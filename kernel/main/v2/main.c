#include"print.h"
#include"int.h"
#include"debug.h"

int main(){
    put_str("here comes the interrupt description table ! \n");
    idt_init();

    
    // now we should open the interrupt by setting interrupt flag IF on
    asm volatile("sti");

    ASSERT(1==2);

    while(1);
    
    return 0;
}
