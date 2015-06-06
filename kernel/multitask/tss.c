#include <tss.h>
#include <gdt.h>
#include <string.h>
#include <heap.h>
#include <screen.h>

extern uint32_t gdt_count;

tss_entry_t * create_tss() {
    tss_entry_t *tss = (tss_entry_t *)kmalloc(sizeof(tss_entry_t));
  uint32_t base = (uint32_t)tss;
  uint32_t limit = base + sizeof(tss_entry_t);
    _set_gdt_gate(gdt_count++, base, limit, GDT_TEMPLATE_TSS);
    memset((void*)tss, 0, sizeof(tss_entry_t));

//  scr_puts("\nnew tss at:");
//  scr_puthex((uint32_t)tss);
//  scr_puts("      gdt number:");
//  scr_putdec(gdt_count-1);

    return tss; 
}

static uint16_t cur_tss_ss = -1;
void tss_switch(uint32_t new_tss_ss) {
    if (cur_tss_ss != -1)
        tss_save(cur_tss_ss);
    cur_tss_ss = new_tss_ss;
    tss_load(cur_tss_ss);
}
