#include <stdint.h>
#include <gdt.h>
#include <tss.h>
#include <string.h>

gdt_ptr_t _sys_gdt_ptr;
gdt_entry_t * _sys_gdt;
#define _sys_gdt_count 0x200 

void _set_gdt_gate(uint32_t num, uint32_t base, uint32_t limit, uint16_t flag)
{
    _sys_gdt[num] = 0;
 
    _sys_gdt[num] |= limit        & 0x000F0000;    // set limit bits 19:16
    _sys_gdt[num] |= (flag << 8)  & 0x00F0FF00;    // set type, p, dpl, s, g, d/b, l and avl fields
    _sys_gdt[num] |= (base >> 16) & 0x000000FF;    // set base bits 23:16
    _sys_gdt[num] |= base         & 0xFF000000;    // set base bits 31:24
 
    // Shift by 32 to allow for low part of segment
    _sys_gdt[num] <<= 32;
 
    // Create the low 32 bit segment
    _sys_gdt[num] |= base << 16;                       // set base bits 15:0
    _sys_gdt[num] |= limit & 0x0000FFFF;               // set limit bits 15:0
}

extern void gdt_flush();
uint32_t gdt_count;  
 
void init_gdt() {
    _sys_gdt = (gdt_entry_t *)(0x30000);
    _sys_gdt_ptr.limit = sizeof(gdt_entry_t) * _sys_gdt_count -1;
    _sys_gdt_ptr.base = 0x30000;

    _set_gdt_gate(0, 0, 0, 0);
    _set_gdt_gate(1, 0, 0x000FFFFF, GDT_TEMPLATE_CODE_PL0);
    _set_gdt_gate(2, 0, 0x000FFFFF, GDT_TEMPLATE_DATA_PL0);
    gdt_count = 3;
//    _set_gdt_gate(3, 0xb8000, 0x0000FFFF, GDT_TEMPLATE_SCREEN); 
    gdt_flush();
}

