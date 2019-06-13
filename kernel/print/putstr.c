#include"./print.h"

void put_str(char* s){
    char * temp=s;
    while((*temp)) {
        put_char(*temp);
        temp++;
    }
    return ;
}

