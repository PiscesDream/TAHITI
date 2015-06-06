#ifndef _IRQ_H_
#define _IRQ_H_

#include <stddef.h>

void irq_init();
void irq_install_handler(int irq, void (*handler)(exception_status_t *r));

#endif
