#include <interrupt/irq.h>
#include <utils/system.h>
#include <stddef.h>
#include <screen.h>
#include <utils/port.h>

extern void irq_0();
extern void irq_1();
extern void irq_2();
extern void irq_3();
extern void irq_4();
extern void irq_5();
extern void irq_6();
extern void irq_7();
extern void irq_8();
extern void irq_9();
extern void irq_10();
extern void irq_11();
extern void irq_12();
extern void irq_13();
extern void irq_14();
extern void irq_15();

void *irq_routines[16] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

void irq_install_handler(int irq, void (*handler)(exception_status_t *r)) {
    irq_routines[irq] = handler;
}

void irq_uninstall_handler(int irq) {
    irq_routines[irq] = 0;
}

extern void _set_idt_gate(unsigned char num, void * base, unsigned short segment, unsigned char flags);
extern void timer_handler();  
extern void keyboard_handler();

extern void set_time_frequency(int);

void irq_init()
{
    // remap IRQ
    // IRQ-0~IRQ-15 map to INT-32~INT-47
    outb_pause(0x11, 0x20);
    outb_pause(0x11, 0xA0);
    outb_pause(0x20, 0x21);
    outb_pause(0x28, 0xA1);
    outb_pause(0x04, 0x21);
    outb_pause(0x02, 0xA1);
    outb_pause(0x01, 0x21);
    outb_pause(0x01, 0xA1);
    outb_pause(0x0, 0x21);
    outb_pause(0x0, 0xA1);

    _set_idt_gate(32, (void *)irq_0, 0x08, 0x8E);
    _set_idt_gate(33, (void *)irq_1, 0x08, 0x8E);
    _set_idt_gate(34, (void *)irq_2, 0x08, 0x8E);
    _set_idt_gate(35, (void *)irq_3, 0x08, 0x8E);
    _set_idt_gate(36, (void *)irq_4, 0x08, 0x8E);
    _set_idt_gate(37, (void *)irq_5, 0x08, 0x8E);
    _set_idt_gate(38, (void *)irq_6, 0x08, 0x8E);
    _set_idt_gate(39, (void *)irq_7, 0x08, 0x8E);
    _set_idt_gate(40, (void *)irq_8, 0x08, 0x8E);
    _set_idt_gate(41, (void *)irq_9, 0x08, 0x8E);
    _set_idt_gate(42, (void *)irq_10, 0x08, 0x8E);
    _set_idt_gate(43, (void *)irq_11, 0x08, 0x8E);
    _set_idt_gate(44, (void *)irq_12, 0x08, 0x8E);
    _set_idt_gate(45, (void *)irq_13, 0x08, 0x8E);
    _set_idt_gate(46, (void *)irq_14, 0x08, 0x8E);
    _set_idt_gate(47, (void *)irq_15, 0x08, 0x8E);

    irq_install_handler(0, timer_handler);
    set_time_frequency(18); // 18hz
}

#include <task.h>
#include <string.h>
extern task_t * cur_task;
extern uint32_t default_estatus;
extern void copy_estatus(uint32_t, uint32_t);
void _irq_handler(exception_status_t *status)
{
    //copy_estatus(sizeof(exception_status_t), (uint32_t)status);
    memcpy((void*)default_estatus, (void*)(cur_task->base+(uint32_t)status), sizeof(exception_status_t));
    status = (exception_status_t*)default_estatus;
 
    void (*handler)(exception_status_t *);

    handler = irq_routines[status->int_no - 32];
    if (handler) 
        handler((exception_status_t*)default_estatus);
    
    if (status->int_no >= 40)
        outb_pause(0x20, 0xA0);
    outb_pause(0x20, 0x20);
}
		


//  0	Division By Zero Exception	No
//  1	Debug Exception	No
//  2	Non Maskable Interrupt Exception	No
//  3	Breakpoint Exception	No
//  4	Into Detected Overflow Exception	No
//  5	Out of Bounds Exception	No
//  6	Invalid Opcode Exception	No
//  7	No Coprocessor Exception	No
//  8	Double Fault Exception	Yes
//  9	Coprocessor Segment Overrun Exception	No
//  10	Bad TSS Exception	Yes
//  11	Segment Not Present Exception	Yes
//  12	Stack Fault Exception	Yes
//  13	General Protection Fault Exception	Yes
//  14	Page Fault Exception	Yes
//  15	Unknown Interrupt Exception	No
//  16	Coprocessor Fault Exception	No
//  17	Alignment Check Exception (486+)	No
//  18	Machine Check Exception (Pentium/586+)	No
//  19 to 31	Reserved Exceptions	No
