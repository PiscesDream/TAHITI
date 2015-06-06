// abandon
#include <paging.h>
#include <stdint.h>
#include <utils/system.h>
#include <string.h>
#include <interrupt/idt.h>
#include <screen.h>
#include <heap.h>
#include <debug.h>

extern uint32_t mem_addr;

// bit set algorithm is modified from http://www.jamesmolloy.co.uk/tutorial_html/6.-Paging.html
static void set_frame(uint32_t frame_addr)
{
    uint32_t frame = frame_addr/0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    frames[idx] |= (0x1 << off);
}

// use bit set to save space
static void clear_frame(uint32_t frame_addr)
{
    uint32_t frame = frame_addr/0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    frames[idx] &= ~(0x1 << off);
}

static uint32_t test_frame(uint32_t frame_addr)
{
    uint32_t frame = frame_addr/0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    return (frames[idx] & (0x1 << off));
}

static uint32_t first_frame()
{
    uint32_t i, j;
    for (i = 0; i < INDEX_FROM_BIT(nframes); i++) {
        if (frames[i] != 0xFFFFFFFF) {// nothing free, exit early.
            for (j = 0; j < 32; j++) {
                uint32_t toTest = 0x1 << j;
                if ( !(frames[i] & toTest) ) return i*4*8+j;
            }
        }
    }
}

void alloc_frame(page_t *page, int is_kernel, int is_writeable) {
    if (page->frame != 0) 
        return; // Frame was already allocated, return straight away.
    else
    {
        uint32_t idx = first_frame(); // idx is now the index of the first free frame.
        if (idx == (uint32_t)-1) 
            panic("No free frames!");
        set_frame(idx*0x1000); // this frame is now ours!
        page->present = 1; // Mark it as present.
        page->rw = (is_writeable)?1:0; // Should the page be writeable?
        page->user = (is_kernel)?0:1; // Should the page be user-mode?
        page->frame = idx;
    }
}

void free_frame(page_t *page) {
    uint32_t frame;
    if (!(frame=page->frame))
        return; // The given page didn't actually have an allocated frame!
    else {
        clear_frame(frame); // Frame is now free again.
        page->frame = 0x0; // Page now doesn't have a frame.
    }
}

void init_paging() {
    mem_addr = 0x200000;
    nframes = mem_capacity / 0x1000;
    frames = (uint32_t*)_malloc(INDEX_FROM_BIT(nframes), 0, 0);
    memset((void *)frames, 0, INDEX_FROM_BIT(nframes));

    kernel_dir = (page_directory_t *)_malloc(sizeof(page_directory_t), 1, 0);
    memset((void *)kernel_dir, 0, sizeof(page_directory_t));
    kernel_dir->phy_addr = (uint32_t)kernel_dir->table_phy_addr;
    //    current_dir = kernel_dir;

    int i = 0;
    while (i < mem_addr) {        
        alloc_frame(get_page(i, 1, kernel_dir), 0, 0); // Kernel code is readable but not writeable from userspace.
        i += 0x1000;
        // will allocate one more during the loop
    }
    mem_addr = 0x100000;


    idt_install_handler(14, page_fault_handler);
    switch_page_directory(kernel_dir);

    current_dir = clone_directory(kernel_dir);
    switch_page_directory(current_dir);
}

void switch_page_directory(page_directory_t *dir) {
    current_dir = dir;
    //asm volatile("mov %0, %%cr3":: "r"(&dir->table_phy_addr));
    asm volatile("mov %0, %%cr3":: "r"(dir->phy_addr));
    uint32_t cr0;
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0":: "r"(cr0));
}

page_t *get_page(uint32_t address, int make, page_directory_t *dir) {
    address /= 0x1000;
    uint32_t table_idx = address / 1024;
    if (dir->tables[table_idx])
        return &dir->tables[table_idx]->pages[address%1024];
    else if(make) {
        uint32_t tmp;
        dir->tables[table_idx] = (page_table_t*)_malloc(sizeof(page_table_t), 1, &tmp);
        memset((void*)dir->tables[table_idx], 0, 0x1000);
        dir->table_phy_addr[table_idx] = tmp | 0x7;      // PRESENT, RW, User
        return &dir->tables[table_idx]->pages[address%1024];
    }
    else
        return 0;
}

void page_fault_handler(exception_status_t* t) {
    uint32_t faulting_addr;
    asm volatile("mov %%cr2, %0" :"=r" (faulting_addr):);
    // Output an error message.
    scr_puts("Page fault! [ ");
    if (!(t->err_code & 0x1)) scr_puts("present ");
    if (t->err_code & 0x2) scr_puts("read-only ");
    if (t->err_code & 0x4) scr_puts("user-mode ");
    if (t->err_code & 0x8) scr_puts("reserved ");
    scr_puts("] at ");
    scr_puthex(faulting_addr);
    scr_puts("\n");
    panic("Page fault");
}


page_directory_t * clone_directory(page_directory_t * src) {
    uint32_t phys;
    page_directory_t *dir = (page_directory_t *)_malloc(sizeof(page_directory_t), 1, &phys);
    memset((void *)dir, 0, sizeof(page_directory_t));

    uint32_t offset = (uint32_t)dir->table_phy_addr - (uint32_t)dir;
    dir->phy_addr = phys + offset;

    int i;
    for (i = 0;i < 1024; ++i) {
        if (!src->tables[i]) continue;
        if (kernel_dir->tables[i] == src->tables[i]) {
            dir->tables[i] = src->tables[i];
            dir->table_phy_addr[i] = src->table_phy_addr[i];
        }
        else {
            uint32_t phys;
            dir->tables[i] = clone_table(src->tables[i], &phys);
            dir->table_phy_addr[i] = phys | 0x07;
        }
    }
    return dir;
}

page_table_t *clone_table(page_table_t *src, uint32_t *phys) {
    page_table_t *table = (page_table_t *)_malloc(sizeof(page_table_t), 1, phys);
    memset((void*)table, 0, sizeof(page_table_t));

    int i;
    for (i = 0; i < 1024; ++i) {
        if (!src->pages[i].frame) continue;
        alloc_frame(&table->pages[i], 0, 0);
        if (src->pages[i].present) table->pages[i].present = 1;
        if (src->pages[i].rw)      table->pages[i].rw = 1;
        if (src->pages[i].user)    table->pages[i].user = 1;
        if (src->pages[i].accessed)table->pages[i].accessed = 1;
        if (src->pages[i].dirty)   table->pages[i].dirty = 1;
        copy_page_physical(src->pages[i].frame*0x1000, table->pages[i].frame*0x1000);
    }
    return table;
}
