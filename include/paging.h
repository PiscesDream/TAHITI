#ifndef _PAGING_H_
#define _PAGING_H_

#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint32_t present  :1;
    uint32_t rw       :1;
    uint32_t user     :1;
    uint32_t accessed :1;
    uint32_t dirty    :1;
    uint32_t unused   :7;
    uint32_t frame    :20;  // shifted right 12 bits
} page_t;

typedef struct {
    page_t pages[1024];
} page_table_t;

typedef struct {
    page_table_t *tables[1024];
    uint32_t table_phy_addr[1024];
    uint32_t phy_addr;  // used when copy/switch
} page_directory_t;

void init_paging();
void switch_page_directory(page_directory_t *);
page_t * get_page(uint32_t, int, page_directory_t *);
void page_fault_handler(exception_status_t* t);

uint32_t *frames;
uint32_t nframes;

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

extern uint32_t mem_addr;
extern uint32_t mem_capacity;
extern page_directory_t * kernel_dir;
page_directory_t* current_dir;


page_table_t *clone_table(page_table_t *src, uint32_t *phys);
extern void copy_page_physical(uint32_t, uint32_t);
page_directory_t * clone_directory(page_directory_t *src);

#endif
