#include <ctype.h>
#include <string.h>

inline int isspace(const int c) {
    return strchr(ISSPACE, c) != NULL;
}

inline int isdigit(const int c) {
    return strchr(ISDIGIT, c) != NULL;
}

void atob(int * dest, const char * src, int base) {
    const char *p = src;
    size_t times = strlen(src)-1;
    int mul = 1;
    while (times--) mul *= base;

    *dest = 0;
    while (*p) {
        *dest += mul * ((*p)-'0');
        p++;
        mul /= 10;
    }
}

