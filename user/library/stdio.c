#include <stdio.h>
#include <syscall.h>
#include <kblookup.h>
#include <stdint.h>
#include <ctype.h>
extern int syscall_putch(char);
extern int syscall_get_key();

void putch(char c) {
    syscall_putch(c);
}

int puts(const char *s) {
    while (*s) 
        putch(*s++);
}

static void __putnum(uint32_t a, int base, char * alpha) {
    uint32_t x = a, acc = 1;
    while (x>=base) 
        acc *= base, x/=base; 
    while (acc)
        putch(alpha[a/acc]), a%=acc, acc/=base;
}

void putdec(uint32_t a) {
    __putnum(a, 10, "0123456789");
}

void puthex(uint32_t a) {
    puts("0x");
    __putnum(a, 16, "0123456789ABCDEF");
}

void putbin(uint32_t a) {
    puts("0b");
    __putnum(a, 16, "01");
}


int printf(const char *fmt, ...) {
    static uint32_t parameter;
    void * ap = (void*) &fmt;
    ap += sizeof(fmt);

    while (*fmt)
    {
        if (*fmt != '%')
            putch(*fmt);
        else
        {
            fmt++;
            //parameter = *(uint32_t*)ap;
            asm volatile("movl %1, %%eax;"
                         "movl %%ss:(%%eax), %%eax;"
                         "movl %%eax, %0;"
                         :"=m"(parameter)
                         :"m"(ap));
            ap += 4;
            if (*fmt == 'c')
                putch(parameter);
            else if (*fmt == 'x')
                puthex(parameter);
            else if (*fmt == 'b')
                putbin(parameter);
            else if (*fmt == 'd') {
                if ((int)(parameter) < 0) {
                    putch('-');
                    putdec(-(int)parameter);
                }
                else 
                    putdec(parameter);
            }
            else if (*fmt == 's')
                puts((char*)parameter);
        }
        fmt++;
    }
    ap = 0;
}

char getch() {
    uint8_t t = 0xff;
    while (t == 0xff)
        t = syscall_get_key();
    return kblookup[t];
}

void gets(char * s) {
    char c=' '; 
    while (isspace(c)) c=getch();
    while (1)
    {
        *s = c;
        s++;
        c = getch();
        if (isspace(c)) { *s = '\0'; return; }
    }
}

int scanf(const char *fmt, ...)
{
    static uint32_t parameter;  // for the function is looking for ds, not ss
    void * ap = (void*) &fmt;
    ap += sizeof(fmt);

    while (*fmt)
    {
        if (*fmt != '%')
            putch(*fmt);
        else
        {
            fmt++;
            asm volatile("movl %0, %%eax;"
                         "mov %%ss:(%%eax), %%eax;"
                         "movl %%eax, %1;"
                         :"=m"(ap)
                         :"m"(parameter));
            ap += 4;
            if (*fmt == 'c')
                *(char *)parameter = getch();
            else if (*fmt == 'd')
            {
                //type_int(parameter);
                static char s[128];
                gets(s);
                atob((int*)parameter, s, 10);
            }
            else if (*fmt == 's')
                gets((char *)parameter); 
        }
        fmt++;
    }
    ap = 0;
}

//  void __puts(char * s) {
//      syscall_puts(s);
//  }
