#include <interrupt/idt.h>
#include <string.h>
#include <stddef.h>
#include <screen.h>
#include <utils/system.h>

extern void _set_idt();

/// idt
#define idt_entry_count 0x200
idt_entry_t * _sys_idt;
idt_ptr_t _sys_idt_ptr;
uint32_t default_estatus = 0x60000;

void _set_idt_gate(unsigned char num, void * base, unsigned short segment, unsigned char flags)
{
    _sys_idt[num].segment = segment;
    _sys_idt[num].zero = 0;
    _sys_idt[num].flags = flags;
    _sys_idt[num].base_low = (unsigned long)base & 0xFFFF;
    _sys_idt[num].base_high = ((unsigned long)base >> 16) & 0xFFFF;
}

extern void isr_0();
extern void isr_1();
extern void isr_2();
extern void isr_3();
extern void isr_4();
extern void isr_5();
extern void isr_6();
extern void isr_7();
extern void isr_8();
extern void isr_9();
extern void isr_10(); 
extern void isr_11();
extern void isr_12();
extern void isr_13();
extern void isr_14();
extern void isr_15();
extern void isr_16();
extern void isr_17();
extern void isr_18();
extern void isr_19();
extern void isr_20();
extern void isr_21();
extern void isr_22();
extern void isr_23();
extern void isr_24();
extern void isr_25();
extern void isr_26();
extern void isr_27();
extern void isr_28();
extern void isr_29();
extern void isr_30();
extern void isr_31();

extern void isr_127();
extern void isr_128();


void idt_init()
{
    _sys_idt = (idt_entry_t *)(0x31000);
    _sys_idt_ptr.base = 0x31000; 
    _sys_idt_ptr.limit = sizeof(idt_entry_t)*idt_entry_count-1;
    memset((void*)(0x31000), 0, _sys_idt_ptr.limit+1);

    _set_idt_gate(0, (void *)isr_0, 0x08, 0x8E);
    _set_idt_gate(1, (void *)isr_1, 0x08, 0x8E);
    _set_idt_gate(2, (void *)isr_2, 0x08, 0x8E);
    _set_idt_gate(3, (void *)isr_3, 0x08, 0x8E);
    _set_idt_gate(4, (void *)isr_4, 0x08, 0x8E);
    _set_idt_gate(5, (void *)isr_5, 0x08, 0x8E);
    _set_idt_gate(6, (void *)isr_6, 0x08, 0x8E);
    _set_idt_gate(7, (void *)isr_7, 0x08, 0x8E);
    _set_idt_gate(8, (void *)isr_8, 0x08, 0x8E);
    _set_idt_gate(9, (void *)isr_9, 0x08, 0x8E);
    _set_idt_gate(10, (void *)isr_10, 0x08, 0x8E);
    _set_idt_gate(11, (void *)isr_11, 0x08, 0x8E);
    _set_idt_gate(12, (void *)isr_12, 0x08, 0x8E);
    _set_idt_gate(13, (void *)isr_13, 0x08, 0x8E);
    _set_idt_gate(14, (void *)isr_14, 0x08, 0x8E);
    _set_idt_gate(15, (void *)isr_15, 0x08, 0x8E);
    _set_idt_gate(16, (void *)isr_16, 0x08, 0x8E);
    _set_idt_gate(17, (void *)isr_17, 0x08, 0x8E);
    _set_idt_gate(18, (void *)isr_18, 0x08, 0x8E);
    _set_idt_gate(19, (void *)isr_19, 0x08, 0x8E);
    _set_idt_gate(20, (void *)isr_20, 0x08, 0x8E);
    _set_idt_gate(21, (void *)isr_21, 0x08, 0x8E);
    _set_idt_gate(22, (void *)isr_22, 0x08, 0x8E);
    _set_idt_gate(23, (void *)isr_23, 0x08, 0x8E);
    _set_idt_gate(24, (void *)isr_24, 0x08, 0x8E);
    _set_idt_gate(25, (void *)isr_25, 0x08, 0x8E);
    _set_idt_gate(26, (void *)isr_26, 0x08, 0x8E);
    _set_idt_gate(27, (void *)isr_27, 0x08, 0x8E);
    _set_idt_gate(28, (void *)isr_28, 0x08, 0x8E);
    _set_idt_gate(29, (void *)isr_29, 0x08, 0x8E);
    _set_idt_gate(30, (void *)isr_30, 0x08, 0x8E);
    _set_idt_gate(31, (void *)isr_31, 0x08, 0x8E);

    // 0xEE means the DPL = 3
    _set_idt_gate(127, (void *)isr_127, 0x08, 0x8E); //debug used
    _set_idt_gate(128, (void *)isr_128, 0x08, 0x8E); //syscall
    _set_idt();
}

char *exception_msg[] = {
"Division By Zero Exception",
"Debug Exception",
"Non Maskable Interrupt Exception",
"Breakpoint Exception",
"Into Detected Overflow Exception",
"Out of Bounds Exception",
"Invalid Opcode Exception",
"No Coprocessor Exception",
"Double Fault Exception",
"Coprocessor Segment Overrun Exception",
"Bad TSS Exception",
"Segment Not Present Exception",
"Stack Fault Exception",
"General Protection Fault Exception",
"Page Fault Exception",
"Unknown Interrupt Exception",
"Coprocessor Fault Exception",
"Alignment Check Exception (486+)",
"Machine Check Exception (Pentium/586+)",
"Reserved Exceptions"};

void *idt_routines[256] = {0};

void idt_install_handler(int idt, void (*handler)(exception_status_t *r)) {
    idt_routines[idt] = handler;
}
#include <task.h>
extern task_t * cur_task;
extern void copy_estatus(uint32_t, uint32_t);
extern void copy_estatus_back(uint32_t, uint32_t);
void _fault_handler(exception_status_t *status) {
//    if (status->int_no >= 32 && status->int_no != 0x80) return;

    // from gs:status -> ds:estatus
//    scr_printf("*estatus = %x\n", (uint32_t)(status));

    //copy_estatus(sizeof(exception_status_t), (uint32_t)status);
    uint32_t source_status = cur_task->base+(uint32_t)status;
    memcpy((void*)default_estatus, (void*)(source_status), sizeof(exception_status_t));
    status = (exception_status_t*)default_estatus;
   
    if (status->int_no < 32) {
        scr_puts("Exception occurs!\n");
        if (status->int_no <= 19) 
            scr_puts(exception_msg[status->int_no]);
        else 
            scr_puts(exception_msg[19]);
        scr_putch('\n');
        return;
    }

    void (*handler)(exception_status_t *);
    handler = idt_routines[status->int_no];
    if (handler) {
        handler((exception_status_t*)default_estatus);
    //    if (status->int_no == 0x80) // need copy back
    //        copy_estatus_back(sizeof(exception_status_t), (uint32_t)status);
        memcpy((void*)(source_status), (void*)default_estatus, sizeof(exception_status_t));
    }
    else {
        scr_puthex(status->int_no);
        panic(" INT MISS : There is no int handler\nPANIC\n");
    }
}


