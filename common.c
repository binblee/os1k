#include "common.h"
void putchar(char ch);

void* memset(void *buf, char c, size_t n){
    uint8_t *p = (uint8_t *) buf;
    while (n--)
        *p ++ = c;
    return buf;
}

void* memcpy(void *dst, const void *src, size_t n){
    uint8_t *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;
    while(n--){
        *d++ = *s++;
    }
    return dst;
}

//TODO implement an alternative function (strcpy_s).
void* strcpy(char *dst, const char *src){
    char *d = dst;
    while( *src ){
        *d++ = *src++;
    }
    *d = '\0';
    return dst;
}
int strcmp(const char *s1, const char *s2){
    while( *s1 && *s2 ){
        if(*s1 != *s2)
            break;
        s1++;
        s2++;
    }
    // The casting to unsigned char * when comparing is done to conform to the POSIX specification.
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

void printf(const char* format, ...){
    va_list args;
    va_start(args, format);
    while(*format){
        if (*format == '%'){
            format ++;
            switch (*format){
                case 's':{
                    const char* str = va_arg(args, char*);
                    while(*str){
                        putchar(*str);
                        str++;
                    }
                    break;
                }
                case 'd':{
                    int value = va_arg(args, int);
                    unsigned magnitude = value;
                    if(value < 0){
                        magnitude = - magnitude;
                        putchar('-');
                    }
                    unsigned divisor = 1;
                    while(magnitude / divisor > 9){
                        divisor *= 10;
                    }
                    while(divisor >0){
                        while(divisor > 0){
                            putchar('0' + magnitude / divisor);
                            magnitude %= divisor;
                            divisor /= 10;
                        }
                    }
                    break;
                }
                case 'x':{
                    // size 32, print MSB-first
                    unsigned value = va_arg(args,unsigned);
                    for(int i=7; i>=0; i--){
                        unsigned nibble = (value >> (i*4)) & 0xf;
                        putchar("0123456789abcdef"[nibble]);
                    }
                    break;
                }
                case '%':
                case '\0':
                    putchar('%');
                    break;
            }
        }else{
            putchar(*format);
        }
        format ++;
    }
}
