#include <stdlib.h>
#include <stdint.h>

static uint32_t seed;

void srand(int s) {
    seed = s;
}

uint32_t rand() {
    seed = (seed * 19843 + 29959);
    return seed;
}
