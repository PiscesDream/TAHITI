#include <utils/port.h>
#include <string.h>
#include <stdint.h>
#include <screen.h>
#include <utils/system.h>

static int scr_x, scr_y, store_x = 0, store_y = 0;
static int hidemode = 0;
uint16_t * const VEDIO_MEM = (uint16_t *) 0xb8000;
const int ATTR = 0x0f00;

void cls() {
    uint16_t *p = (uint16_t *) VEDIO_MEM;
    int t = 80*25;
    while (t--) *p++=0x0f00;
    scr_x = scr_y = 0;
}

void scroll_up()
{
    uint16_t blank = ' ' | (ATTR << 8);
    int scroll_len = scr_x-SCR_BOTTOM;
    memcpy((void *)VEDIO_MEM, (void*)(VEDIO_MEM + scroll_len*80), (SCR_BOTTOM) * 80 * 2);
    memsetw((void *)(VEDIO_MEM+(SCR_BOTTOM)*80), blank, scroll_len*80);
    scr_x--;
}


void scr_update_cursor(int x, int y) {
    scr_x = x; scr_y = y;
    if (!hidemode) {
        int position = x*80 + y;
        outb(0x0f, 0x3d4);
        outb(position & 0xff, 0x3d5);
        outb(0x0E, 0x3d4);
        outb((position>>8) & 0xFF, 0x3d5);
    }
}
void scr_backup_cursor() {
    store_x = scr_x, store_y = scr_y;
}
void scr_retrieve_cursor() {
    scr_x = store_x, scr_y = store_y;
}
void scr_set_hidemode(int setting) {
    hidemode = setting;
}
void scr_temp_move_to(uint32_t x, uint32_t y) {
    asm volatile("movw %ds, %ax; movw %ax, %fs ;movw $0x10, %ax; movw %ax, %ds;");
    scr_backup_cursor();
    scr_set_hidemode(1);
    scr_update_cursor(x, y);
    asm volatile("movw %fs, %ax;movw %ax, %ds");
}
void scr_temp_retrieve() {
    asm volatile("movw %ds, %ax; movw %ax, %fs ;movw $0x10, %ax; movw %ax, %ds;");
    scr_set_hidemode(0);
    scr_retrieve_cursor();
    asm volatile("movw %fs, %ax;movw %ax, %ds");
}

void scr_putch(char c) {
    asm volatile("movw %ds, %ax; movw %ax, %fs ;movw $0x10, %ax; movw %ax, %ds;");

    if (c == 0x08)
        scr_y--;
    else if (c == 0x09) 
        scr_y = (scr_y+8) & ~7;
    else if (c == '\r')
        scr_y = 0;
    else if (c == '\n')
        scr_y = 0, scr_x++;
    else if (c >= ' ') 
        *(VEDIO_MEM+scr_x*80+scr_y) = ATTR | c, scr_y++;

    if (scr_y >= 80) scr_y = 0, scr_x++;
    if (scr_x == SCR_BOTTOM+1) scroll_up();
    scr_update_cursor(scr_x, scr_y);

    asm volatile("movw %fs, %ax;movw %ax, %ds");
}

void scr_puts(const char * s) {
    while (*s) scr_putch(*s++);
}
void scr_puts_space_stop(const char * s) {
    while (*s && *s != ' ') scr_putch(*s++);
}

void _scr_putnum(uint32_t a, int base, char * alpha) {
    uint32_t x = a, acc = 1;
    while (x>=base) 
        acc *= base, x/=base; 
    while (acc)
        scr_putch(alpha[a/acc]), a%=acc, acc/=base;
}

void scr_putdec(uint32_t a) {
    _scr_putnum(a, 10, "0123456789");
}

void scr_puthex(uint32_t a) {
    scr_puts("0x");
    _scr_putnum(a, 16, "0123456789ABCDEF");
}

void scr_putbin(uint32_t a) {
    scr_puts("0b");
    _scr_putnum(a, 16, "01");
}

#include <task.h>
void scr_put_task(task_t * a) {
    kprintf("eax: %x, ebx: %x, ecx: %x, edx: %x\n", a->estatus.eax,a->estatus.ebx,a->estatus.ecx,a->estatus.edx);
    kprintf("cs: %x, ds: %x, es: %x, fs: %x\n", a->estatus.cs,a->estatus.ds,a->estatus.es,a->estatus.fs);
    kprintf("ss: %x, esp: %x, ebp: %x\n", a->estatus.ss,a->estatus.esp,a->estatus.ebp);
    kprintf("eip: %x pid: %d, eflags: %x\n", a->estatus.eip, a->pid, a->estatus.eflags);
    kprintf("phy_base: %x\n", a->base);
//  scr_puts("Task: ");
//  scr_puthex((uint32_t)a);
//  scr_puts("\nid: ");
//  scr_puthex(a->id);
//  scr_puts("\nesp: ");
//  scr_puthex(a->esp);
//  scr_puts("\nebp: ");
//  scr_puthex(a->ebp);
//  scr_puts("\neip: ");
//  scr_puthex(a->eip);
//  scr_puts("\n");
}


int scr_printf(const char *fmt, ...)
{
    uint32_t parameter;
    void * ap = (void*) &fmt;
    ap += sizeof(fmt);

    while (*fmt)
    {
        if (*fmt != '%')
            scr_putch(*fmt);
        else
        {
            fmt++;
            //parameter = *(uint32_t*)ap;
            asm volatile("movl %1, %%eax;"
                         "movl %%ss:(%%eax), %%eax;"
                         "movl %%eax, %0;"
                         :"=m"(parameter)
                         :"m"(ap));
            ap += 4;
            if (*fmt == 'c')
                scr_putch(parameter);
            else if (*fmt == 'x')
                scr_puthex(parameter);
            else if (*fmt == 'b')
                scr_putbin(parameter);
            else if (*fmt == 'd') {
                if ((int)(parameter) < 0) {
                    scr_putch('-');
                    scr_putdec(-(int)parameter);
                }
                else 
                    scr_putdec(parameter);
            }
            else if (*fmt == 's')
                scr_puts((char*)parameter);
            else if (*fmt == 'S')
                scr_puts_space_stop((char*)parameter);

        }
        fmt++;
    }
    ap = NULL;
}

// no use




void putch_for_syscall(exception_status_t * t) {
    char c = (char)(t->ebx);
    if (c == 0x08)
        scr_y--;
    else if (c == 0x09) 
        scr_y = (scr_y+8) & ~7;
    else if (c == '\r')
        scr_y = 0;
    else if (c == '\n')
        scr_y = 0, scr_x++;
    else if (c >= ' ') 
        *(VEDIO_MEM+scr_x*80+scr_y) = ATTR | c, scr_y++;
    if (scr_y >= 80) scr_y = 0, scr_x++;
    if (scr_x == SCR_BOTTOM+1) scroll_up();
    scr_update_cursor(scr_x, scr_y);
}

int puts_for_syscall(exception_status_t *t) {
    char * s = cur_task->base + t->ebx;
    scr_puts(s);
}
