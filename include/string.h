#ifndef _STRING_H_
#define _STRING_H_

#ifndef NULL
#define NULL ((void *) 0)
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

static inline unsigned char *memcpy(unsigned char *dest, const unsigned char *src, size_t count) {
    while (count--) *dest++ = *src++;
    return dest;
}

static inline unsigned char *memset(unsigned char *dest, unsigned char val, size_t count) {
    while (count--) *dest++ = val;
    return dest;
}

static inline unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count) {
    while (count--) *dest++ = val;
    return dest;
}

static inline size_t strlen(const char *str)
{
    size_t _result = 0;
    while (*str++) ++_result;
    return _result;
}


#include <stdbool.h>

static inline bool strncmp(char *p, char *q, int n)
{
    while (n--) {
        if ((!*p && *q) || (!*q  && *p)) return false;
        if (!*p && !*q) return true;
        if (*p >= 'a' && *p <= 'z') *p += 'A' - 'a';
        if (*q >= 'a' && *q <= 'z') *q += 'A' - 'a';
        if (*p != *q) return false;
        p++, q++;
    }
    return true;
}

static inline bool strcmp_space(char *p, char *q)
{
    while (*p && *q && *p!=' ' && *q!=' ') {
        if (*p >= 'a' && *p <= 'z') *p += 'A' - 'a';
        if (*q >= 'a' && *q <= 'z') *q += 'A' - 'a';
        if (*p != *q) return false;
        p++, q++;
    }
    if ((*p && *p != ' ')||(*q && *q != ' ')) return false;
    return true;
}



#endif

