#include <screen.h>

void panic(const char * s) {
	scr_puts("Kernel panic: ");
    scr_puts(s);
	for(;;);
}

