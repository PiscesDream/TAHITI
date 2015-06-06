#include <ldt.h>

void set_ldt_gate(ldt_entry_t* ldt, uint32_t base, uint32_t limit, uint32_t flag) {
    *ldt = 0;
    
    *ldt |= limit        & 0x000F0000;
    *ldt |= (flag << 8)  & 0x00F0FF00;
    *ldt |= (base >> 16) & 0x000000FF;
    *ldt |= base         & 0xFF000000;
    *ldt <<= 32;
    *ldt |= base << 16;        
    *ldt |= limit & 0x0000FFFF;
}
