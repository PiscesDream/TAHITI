#include <interrupt/keyboard.h>
#include <utils/port.h>
#include <screen.h>
#include <stddef.h>
#include <interrupt/irq.h>
#include <interrupt/kdbus.h>
#include <constants.h>
#include <stdbool.h>

uint8_t kb_buffer[KB_BUFFER_MAX];
int kb_buffer_len, kb_buffer_head, kb_buffer_tail;

void reset_kb_buffer() {
    kb_buffer[0] = '\0';
    kb_buffer_head = kb_buffer_len = 0; 
    kb_buffer_tail = -1;
}

uint8_t pop_kb_buffer() {
    if (kb_buffer_len == 0) return 0xff;
    kb_buffer_len--;
    kb_buffer_tail++;
    if (kb_buffer_tail >= KB_BUFFER_MAX) kb_buffer_tail = 0;
    return kb_buffer[kb_buffer_tail];
}

void push_kb_buffer(uint8_t scancode) {
    if (kb_buffer_len >= KB_BUFFER_MAX) return;
    kb_buffer_len++;
    kb_buffer[kb_buffer_head] = scancode;
    kb_buffer_head++;
    if (kb_buffer_head >= KB_BUFFER_MAX) kb_buffer_head = 0;
}

void keyboard_handler(exception_status_t *r);
void init_keyboard() {
    reset_kb_buffer();
    irq_install_handler(1, keyboard_handler);
}


static bool __alt = false;
static bool __ctrl = false;
void keyboard_handler(exception_status_t *r) {
    unsigned char scancode = inb(0x60);
    /* If the top bit of the byte we read from the keyboard is
    *  set, that means that a key has just been released */
    if (scancode & 0x80) {
        // release a key
        if (scancode-0x80 == KEY_ALT) __alt = false;
        if (scancode-0x80 == KEY_CTRL) __ctrl = false;
    }
    else {
        if (scancode == KEY_ALT) __alt = true;
        if (scancode == KEY_CTRL) __ctrl = true;

        if (__ctrl && scancode == KEY_F1) switch_video_page(1);
        if (__ctrl && scancode == KEY_F2) switch_video_page(2);
        if (__ctrl && scancode == KEY_F3) switch_video_page(3);


        push_kb_buffer(scancode);

//scr_putch(kbdus[scancode]);

//      uint8_t c =pop_kb_buffer();
//      kprintf("get: %x(%c), pop: %x(%c)", scancode, kbdus[scancode], c, kbdus[c]);
//      scr_putch(kbdus[pop_kb_buffer()]);
    }
}

#include <stddef.h>

int get_key_for_syscall(exception_status_t * t) {
    if (cur_task->video_page != cur_video_page) 
        return 0xff;
    else 
        return pop_kb_buffer(); 
}
