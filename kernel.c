#include "kernel.h"
#include "common.h"
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

extern char __bss[], __bss_end[], __stack_top[];

struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3, long arg4, long arg5, long fid, long eid) {
    register long a0 __asm__("a0") = arg0;
    register long a1 __asm__("a1") = arg1;
    register long a2 __asm__("a2") = arg2;
    register long a3 __asm__("a3") = arg3;
    register long a4 __asm__("a4") = arg4;
    register long a5 __asm__("a5") = arg5;
    register long a6 __asm__("a6") = fid;
    register long a7 __asm__("a7") = eid;
    __asm__ __volatile__("ecall"
                        : "=r"(a0), "=r"(a1)
                        : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5),
                        "r"(a6), "r"(a7)
                        : "memory");

    return (struct sbiret) {.error = a0, .value = a1};
}

void putchar(char ch) {
    sbi_call(ch, 0, 0, 0, 0, 0, 0, 1 /* Console Putchar */);
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
                    while(magnitude / divisor > 0){
                        divisor *= 10;
                    }
                    if(divisor == 1)
                        putchar('0');
                    else
                        divisor /= 10;
                    const char digits[]="0123456789";
                    while(divisor > 0){
                        putchar(digits[magnitude / divisor]);
                        magnitude %= divisor;
                        divisor /= 10;
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

void *memset(void *buf, char c, size_t n) {
    uint8_t *p = (uint8_t *) buf;
    while (n--)
        *p ++ = c;
    return buf;
}

void kernel_main(void) {
    printf("\n\nHello %s\n", "World!");
    printf("%d\n",10);
    printf("1 + 2 = %d, %x\n", 1 + 2, 0x1234abcd);
    for (;;) {
        __asm__ __volatile__("wfi");
    }
}

__attribute__((section(".text.boot")))
__attribute__((naked))
void boot(void) {
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n"
        "j kernel_main\n"
        :
        : [stack_top] "r" (__stack_top)
    );
}
