#ifndef _PROGRAM_HEADER_H_
#define _PROGRAM_HEADER_H_

// start
__asm__("call _main\n");
int main();
#include <system.h>
int _main() {
    main();

    exit(0);
    return 0;
}

#endif

