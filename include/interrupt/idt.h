#ifndef _IDT_H_
#define _IDT_H_

#include <stddef.h>

typedef struct {
    unsigned short base_low;
    unsigned short segment;        /* Our kernel segment goes here! */
    unsigned char zero;     /* This will ALWAYS be set to 0! */
    unsigned char flags;       /* Set using the above table! */
    unsigned short base_high;
} __attribute__((packed))  idt_entry_t;

typedef struct {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed)) idt_ptr_t;


void idt_init();
void idt_install_handler(int idt, void (*handler)(exception_status_t *r));

#endif 
