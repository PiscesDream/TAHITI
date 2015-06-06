#ifndef _LDT_H_
#define _LDT_H_

#include <stdint.h>
#include <gdt.h>

typedef uint64_t ldt_entry_t;
void set_ldt_gate(ldt_entry_t*, uint32_t, uint32_t, uint32_t);
#define LDT_TEMPLATE_CODE    DT_GRAN(0) | DT_SIZE(1) | DT_LONG(0) | DT_SAVL(0) | \
                              DT_PRES(1) | DT_DESCTYPE(1) | DT_PRIV(0) | \
                              DT_TYPE_CODE_XR
#define LDT_TEMPLATE_DATA    DT_GRAN(0) | DT_SIZE(1) | DT_LONG(0) | DT_SAVL(0) | \
                              DT_PRES(1) | DT_DESCTYPE(1) | DT_PRIV(0) | \
                              DT_TYPE_DATA_RW



#endif
