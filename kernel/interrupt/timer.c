#include <utils/port.h>
#include <stddef.h>
#include <screen.h>
#include <interrupt/irq.h>
#include <stdint.h>

extern void task_switch(exception_status_t*);
static uint32_t timer_ticks = 0;
static uint32_t frequency = 18;

static char spin_string[]="|/-\\";
static int spin_count=0;

void spin() {
    spin_count = (spin_count+1)%4;
    scr_temp_move_to(24, 79);
    scr_putch(spin_string[spin_count]);
    scr_temp_retrieve();
}

void timer_handler(exception_status_t *r) {
    timer_ticks++;
    if (timer_ticks % frequency == 0) {
        spin();
        task_switch(r);
    }
}

void set_time_frequency(int fre) {
    frequency = fre;
    int divisor = 1193180 / frequency;
    outb_pause(0x36, 0x43);            
    outb_pause(divisor & 0xFF, 0x40); 
    outb_pause(divisor >> 8, 0x40);  
}
