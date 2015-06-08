#include <string.h>
#include <stdbool.h>

size_t strlen(const char *str) {
    size_t t = 0;
    while (*str++) t++;
    return t; 
}

char * strchr(const char *str, int c) {
    char * p = str;
    while (*p) {
        if (*p == c) break;
        p++;
    }
    if (*p) return p;
    else return NULL;
}


char *strncpy(char *dest, const char *src, size_t n) {
    int index = 0;
    char *p = dest;
    while (index < n && *src)
        *dest++ = *src++, index++;
    while (index < n)
        *dest++ = '\0';
    return p;
}

size_t strcspn(const char *str1, const char *str2) {
    size_t index = 0;
    while (*str1++) {
        if (strchr(str2, *str1) != NULL)
            break;
        index++;
    }
    return index;
}


