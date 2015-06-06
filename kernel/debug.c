#include <debug.h>
#include <utils/port.h>
#include <utils/system.h>
#include <screen.h>
#include <stdint.h>
#include <interrupt/idt.h>

extern uint32_t mem_capacity;

void __debug_smile() {
    scr_puts(":)\n");
    hlt();
}

// the following is unit test
void __debug_keyboard() {
    for (;;){
            unsigned char a = inb(0x60);
            outb(0x20, 0x20);
            //print_dec(a);
            //print_char(' ');
        }
} 

// from linux 0.11
#define EXT_MEM_K (*(unsigned short *)0x90002)      // [0x90002]: extended mem after 1M (in kB)

void __debug_diplay_info() {
	mem_capacity = (1<<20) + (EXT_MEM_K<<10); // memory end
    scr_puts("Memory space: ");
    scr_putdec(mem_capacity);
    scr_puts("byte = ");
    scr_putdec(mem_capacity>>20);
    scr_puts("MB\n");
}

#include <task.h>
void debug_handler(exception_status_t *t) {
    task_switch(t);
//  scr_printf("eax: %x, ebx: %x, ecx: %x, edx %x", t->eax, t->ebx, t->ecx, t->edx);
//  t->eax = t->ebx = t->ecx = t->edx =0x123;
//    t->ds = t->fs = selector(3, 0b000);
}

void init_debug() {
//    irq_install_handler(0x80, &syscall_handler);
    idt_install_handler(127, &debug_handler);
}



