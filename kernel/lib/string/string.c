#include"print.h"
#include"string.h"
#include"debug.h"

void memset(void * dst,uint8_t value,uint32_t size){
    uint8_t * _dst=(uint8_t *)dst;
    ASSERT(dst!=0);
    while(size-- > 0) *_dst++=value;
}

void memcpy(void * dst, const void * src, uint32_t size){
    uint8_t * _dst=(uint8_t *)dst;
    uint8_t * _src=(uint8_t *)src;
    ASSERT(dst!=0 && src!=0);
    while(size--) *_dst++=*_src++;
}

int memcmp( const void * a, const void * b, uint32_t size){
    ASSERT(a!=0 && b!=0);
    const char * _a=(const char*) a;
    const char * _b=(const char*) b;
    while(size--){
        if(*_a!=*_b) return (*_a>*_b) ? 1 : -1 ;
        _a++;
        _b++;
    }
    return 0;
}

void strcpy(char * dst,char * src){
    while(*src) *dst++=*src++;
}

uint32_t strlen(char * src){
    char * temp = src;
    uint32_t len=0;
    while(*temp++) len++;
    return len;
}

int string_equal(const char * a,const char *b){
    while(*a == *b){
        if(*a == 0) return 1;
        a++;
        b++;
    }
    return 0;
}
