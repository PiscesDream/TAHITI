#ifndef _STDIO_H_
#define _STDIO_H_

#include <stdint.h>

void putch(char);
int puts(const char *s);
void putdec(uint32_t) ;
void puthex(uint32_t) ;
void putbin(uint32_t) ;
int printf(const char *fmt, ...) ;
char getch();
void gets(char * s);
int scanf(const char *fmt, ...);

#endif
