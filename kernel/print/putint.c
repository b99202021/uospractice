#include"./print.h"

void put_int(uint32_t n){
    if(n==0) put_char('0');
    int i;
    char c[8];
    for(i=0;i<8;i++){
        char l;
        if(n%16 > 9){
            c[7-i]=(char)(n%16 - 10 + 'a');
        }
        else{
            c[7-i]=(char)(n%16 + '0');
        }
        n/=16;
    }
    put_char('0');
    put_char('x');
    for(i=0;i<8;i++){
        put_char(c[i]);
    }
    return ;
}
