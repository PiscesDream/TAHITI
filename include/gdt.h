#ifndef _GDT_H_
#define _GDT_H_

#include <stdint.h>

// from turtorial
#define DT_GRAN(x)      ((x) << 0x0F) // Granularity (0 for 1B - 1MB, 1 for 4KB - 4GB)
#define DT_SIZE(x)      ((x) << 0x0E) // Size (0 for 16-bit, 1 for 32)
#define DT_LONG(x)      ((x) << 0x0D) // Long mode
#define DT_SAVL(x)      ((x) << 0x0C) // Available for system use
#define DT_PRES(x)      ((x) << 0x07) // Present
#define DT_PRIV(x)      (((x) &  0x03) << 0x05)   // Set privilege level (0 - 3)
#define DT_DESCTYPE(x)  ((x) << 0x04) // Descriptor type (0 for system, 1 for code/data)

#define DT_TYPE_DATA_R          0x00 // Read-Only
#define DT_TYPE_DATA_RA         0x01 // Read-Only, accessed
#define DT_TYPE_DATA_RW         0x02 // Read/Write
#define DT_TYPE_DATA_RWA        0x03 // Read/Write, accessed
#define DT_TYPE_DATA_RD         0x04 // Read-Only, expand-down
#define DT_TYPE_DATA_RDA        0x05 // Read-Only, expand-down, accessed
#define DT_TYPE_DATA_RWD        0x06 // Read/Write, expand-down
#define DT_TYPE_DATA_RWDA       0x07 // Read/Write, expand-down, accessed
#define DT_TYPE_CODE_X          0x08 // Execute-Only
#define DT_TYPE_CODE_XA         0x09 // Execute-Only, accessed
#define DT_TYPE_CODE_XR         0x0A // Execute/Read
#define DT_TYPE_CODE_XRA        0x0B // Execute/Read, accessed
#define DT_TYPE_CODE_XC         0x0C // Execute-Only, conforming
#define DT_TYPE_CODE_XCA        0x0D // Execute-Only, conforming, accessed
#define DT_TYPE_CODE_XRC        0x0E // Execute/Read, conforming
#define DT_TYPE_CODE_XRCA       0x0F // Execute/Read, conforming, accessed

// some pre-set gdts
// selector = descriptor + TI(1) + RPL(2)
#define GDT_TEMPLATE_CODE_PL0 DT_GRAN(1) | DT_SIZE(1) | DT_LONG(0) | DT_SAVL(0) | \
                              DT_PRES(1) | DT_DESCTYPE(1) | DT_PRIV(0) | \
                              DT_TYPE_CODE_XR
#define GDT_TEMPLATE_DATA_PL0 DT_GRAN(1) | DT_SIZE(1) | DT_LONG(0) | DT_SAVL(0) | \
                              DT_PRES(1) | DT_DESCTYPE(1) | DT_PRIV(0) | \
                              DT_TYPE_DATA_RW
#define GDT_TEMPLATE_SCREEN DT_GRAN(0) | DT_SIZE(0) | DT_LONG(0) | DT_SAVL(0) | \
                            DT_PRES(1) | DT_DESCTYPE(1) | DT_PRIV(0) | \
                            DT_TYPE_DATA_RW
#define GDT_TEMPLATE_CODE_PL3 DT_GRAN(1) | DT_SIZE(1) | DT_LONG(0) | DT_SAVL(0) | \
                              DT_PRES(1) | DT_DESCTYPE(1) | DT_PRIV(3) | \
                              DT_TYPE_CODE_XR
#define GDT_TEMPLATE_DATA_PL3 DT_GRAN(1) | DT_SIZE(1) | DT_LONG(0) | DT_SAVL(0) | \
                              DT_PRES(1) | DT_DESCTYPE(1) | DT_PRIV(3) | \
                              DT_TYPE_DATA_RW
#define GDT_TEMPLATE_TSS DT_GRAN(0) | DT_SIZE(0) | DT_LONG(0) | DT_SAVL(0) | \
                         DT_PRES(1) | DT_DESCTYPE(0) | DT_PRIV(0) | \
                         DT_TYPE_CODE_XA
#define GDT_TEMPLATE_LDT DT_GRAN(0) | DT_SIZE(0) | DT_LONG(0) | DT_SAVL(0) | \
                         DT_PRES(0) | DT_DESCTYPE(0) | DT_PRIV(0) | \
                         DT_TYPE_DATA_RW
#define GDT_TEMPLATE_TASK_CODE DT_GRAN(0) | DT_SIZE(1) | DT_LONG(0) | DT_SAVL(0) | \
                               DT_PRES(1) | DT_DESCTYPE(1) | DT_PRIV(0) | \
                               DT_TYPE_CODE_XR
#define GDT_TEMPLATE_TASK_DATA DT_GRAN(0) | DT_SIZE(1) | DT_LONG(0) | DT_SAVL(0) | \
                               DT_PRES(1) | DT_DESCTYPE(1) | DT_PRIV(0) | \
                               DT_TYPE_DATA_RW

//#define GDT_CODE_PL3 DT_DESCTYPE(1) | DT_PRES(1) | DT_SAVL(0) | \
                     DT_LONG(0)     | DT_SIZE(1) | DT_GRAN(1) | \
                     DT_PRIV(3)     | DT_TYPE_CODE_EXRD
//#define GDT_DATA_PL3 DT_DESCTYPE(1) | DT_PRES(1) | DT_SAVL(0) | \
                     DT_LONG(0)     | DT_SIZE(1) | DT_GRAN(1) | \
                     DT_PRIV(3)     | DT_TYPE_DATA_RDWR

typedef uint64_t gdt_entry_t;
typedef struct {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed)) gdt_ptr_t ;

void init_gdt();
void _set_gdt_gate(uint32_t num, uint32_t base, uint32_t limit, uint16_t flag);

#endif
