#ifndef _STRING_H_
#define _STRING_H_

#include <stddef.h>
char * strchr(const char *str, int c);
char *strncpy(char *dest, const char *src, size_t n);
size_t strcspn(const char *str1, const char *str2);
size_t strlen(const char *str);

#endif

