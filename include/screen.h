#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <task.h>

void cls();
//void scr_update_cursor(int row, int col);
void scr_puts(const char *);
void scr_putch(char);
void scr_putdec(uint32_t);
void scr_puthex(uint32_t);
void scr_putbin(uint32_t);
void scr_put_task(task_t*);
//void scr_backup_cursor();
//void scr_retrieve_cursor();
//void scr_set_hidemode(int setting);
int scr_printf(const char *fmt, ...);
int user_printf(const char *fmt, ...);
#define printf user_printf
#define kprintf scr_printf



void scr_temp_move_to(uint32_t, uint32_t);
void scr_temp_retrieve();

#define SCR_TOP 0 
#define SCR_BOTTOM 22
#define SCR_LEFT 0 
#define SCR_RIGHT 79
#define SCR_HEIGHT (SCR_BOTTOM-SCR_TOP+1)
#define SCR_WIDTH (SCR_RIGHT-SCR_LEFT+1)

#endif
