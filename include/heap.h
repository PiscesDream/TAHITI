#ifndef _HEAP_H_
#define _HEAP_H_

#include <stdint.h>

extern uint32_t mem_addr;
//uint32_t _malloc(uint32_t size, int align, uint32_t *phys); with paging
void init_heap();
uint32_t kmalloc(uint32_t size);
void kfree(uint32_t size);

typedef struct mm_node {
    struct mm_node *last, *next;
    uint32_t size;
} mm_node_t;

#endif
