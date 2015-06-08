#include <utils/system.h>
#include <debug.h>
#include <screen.h>
#include <gdt.h>
#include <interrupt/idt.h>
#include <interrupt/irq.h>
#include <interrupt/timer.h>
//#include <paging.h>
#include <task.h>
#include <syscall.h>
#include <fs/fs.h>
#include <tss.h>
#include <lab.h>
#include <heap.h>
#include <interrupt/keyboard.h>

static uint8_t preset = 1;
uint32_t mem_capacity = 0;
//page_directory_t* kernel_dir;
//uint32_t initial_esp;
extern void switch_to_user_mode();

int main() {
    if (preset) {
        preset = 0; 
        init_heap();
        init_gdt();
    }
    cli();
    idt_init();
    irq_init();
    init_keyboard();
    __debug_diplay_info();
    set_time_frequency(20);
//    init_paging();
    init_syscall();
    init_debug();
    sti();

    cls();
    init_multitask();
    init_fs();
    printf("System starts. \n");
    asm volatile("int $127"); // force switch for the first time

    shell();

    for (;;){
    }
}             
