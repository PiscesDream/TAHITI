#include <heap.h>
#include <stdint.h>
#include <screen.h>
#include <utils/system.h>

uint32_t mem_addr = 0x100000;  // linear usage of kernel memory
uint32_t mem_addr_end = 0x400000;  
mm_node_t *mm_node_cur, *mm_node_first;
uint32_t mm_minsize = sizeof(mm_node_t);

// linear allocation
uint32_t __kmalloc(uint32_t size) {
    if (size < mm_minsize) size = mm_minsize;
    uint32_t tmp = mem_addr;
    mem_addr += size; 
    return tmp;
}
void __kfree(uint32_t addr) {}

uint32_t kmalloc(uint32_t size) {
//  mm_node_t * mm_node_print = mm_node_first;
//  printf("node %x, last %x, next %x, space %x", 
//          mm_node_print, mm_node_print->last, mm_node_print->next, mm_node_print->size);
//  mm_node_print = mm_node_print->next;
//  while (mm_node_print != mm_node_first) {
//      printf("|||  node %x, last %x, next %x, space %x", 
//              mm_node_print, mm_node_print->last, mm_node_print->next, mm_node_print->size);
//      mm_node_print = mm_node_print->next;
//  }
//  printf("\n");

    if (size < mm_minsize) size = mm_minsize;

    uint32_t addr;
//    mm_node_t * mm_node_ptr = mm_node_cur; // this is the circulating one
    mm_node_t * mm_node_ptr = mm_node_first; // this will the first bit algorithm
    
    if (mm_node_ptr->size < size) {
        if (mm_node_ptr->next == mm_node_ptr) // single hole
            panic("Memory Full!");
        else {
            mm_node_ptr = mm_node_ptr->next; 
            while (mm_node_ptr != mm_node_first && mm_node_ptr->size < size) mm_node_ptr = mm_node_ptr->next;
            if (mm_node_ptr == mm_node_first) panic("Memory Full !");
        }
    }

    addr = (uint32_t)mm_node_ptr + sizeof(mm_node_t); // allocate
    mm_node_t *mm_node_new = (mm_node_t*)(addr+size);
    mm_node_new->size = mm_node_ptr->size - size - sizeof(mm_node_t);
    mm_node_ptr->size = 0;

    mm_node_new->last = mm_node_ptr;
    mm_node_new->next = mm_node_ptr->next;
    mm_node_new->next->last = mm_node_new;
    mm_node_new->last->next = mm_node_new;

    mm_node_cur = mm_node_new;
    return addr;
}

void kfree(uint32_t addr) {
    mm_node_t * mm_node_ptr = (mm_node_t*)(addr - sizeof(mm_node_t)); // get the blank info

    mm_node_ptr->size =(uint32_t)mm_node_ptr->next - (uint32_t)mm_node_ptr - sizeof(mm_node_t);
    if (mm_node_ptr->next->size > 0) { // the next node is available, merge it!
        mm_node_ptr->size += mm_node_ptr->next->size + sizeof(mm_node_t);

        mm_node_ptr->next = mm_node_ptr->next->next;
        mm_node_ptr->next->last = mm_node_ptr;
//        printf("merge next");
    }

    if (mm_node_ptr->last->size > 0) { // the last node is available, be merged !
        mm_node_ptr->last->size += mm_node_ptr->size + sizeof(mm_node_t);

        mm_node_ptr->last->next = mm_node_ptr->next;
        mm_node_ptr->last->next->last = mm_node_ptr->last;
//        printf("merge last");
    }
}

void init_heap() {
    mm_node_cur = mm_node_first = (mm_node_t*)mem_addr;
    mm_node_first->last = mm_node_first; 
    mm_node_first->next = mm_node_first; 
    mm_node_first->size = mem_addr_end - mem_addr - sizeof(mm_node_t);
}




//
// with paging
//  uint32_t _malloc(uint32_t size, int align, uint32_t *phys) {
//      if (align && (mem_addr & 0xFFFFF000)) // check align
//          mem_addr = (mem_addr & 0xFFFFF000) + 0x1000;
//      if (phys) 
//          *phys = mem_addr;
//      uint32_t tmp = mem_addr;
//      mem_addr += size;

//  //  scr_puts("Allocation: ");
//  //  scr_puthex(tmp);
//  //  scr_putch('\n');

//  //  scr_puts("Mem addr: ");
//  //  scr_puthex(mem_addr);
//  //  scr_putch('\n');

//      return tmp;
//  }

