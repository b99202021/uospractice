#include"/home/twchang/microos/uos/kernel/print/print.h"

int main(void){
    int i;
    char c[5];
    put_char('\n');
    for(i=0;i<10;i++){
        put_str("TW OS is coming!!\n");
    }
    while(1);
    return 0;
}

