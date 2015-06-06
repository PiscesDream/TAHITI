#ifndef _TASK_H_
#define _TASK_H_

//#include <paging.h>
#include <tss.h>
#include <stdint.h>
#include <stddef.h>

#define TASK_STATUS_DEAD 0x1
#define TASK_STATUS_ALIVE 0x2
#define TASK_STATUS_READY 0x3
#define TASK_STATUS_BLOCKED 0x4

typedef struct message {
    uint32_t addr;
    uint32_t size;
    uint32_t sender;
    struct message * next;
} message_t;

typedef struct __task_t {
    struct __task_t *next;
    
    // fork
    uint32_t children_count;
    struct __task_t *children;
    struct __task_t *brothers;
    struct __task_t *parent;

    // basic
    uint32_t status;
    uint32_t pid;
    uint32_t privilege;
    uint32_t base;
    uint32_t length;

    uint32_t priority;
    uint32_t counter;

    // progress status
    exception_status_t estatus; 

    // communicate
    message_t *message_box;
    uint32_t message_count;

    // used in semaphore;
    struct __task_t * semaphore_next;


//    uint16_t tss_gdt_id;
//    uint32_t tss_base;
//    uint32_t tss_slt;
//    tss_entry_t * tss;

//  uint16_t ldt_gdt_id;
//  uint16_t ldt_slt;
//  uint32_t ldt_base; // ldt has 2 entry, 0 for code, 1 for data/stack
//  uint32_t ldt_limit; 
}  task_t; // TCB


//extern uint32_t read_eip();
//void init_multitask();
//void move_stack(void *, uint32_t);
//int getpid();

void init_multitask();
void task_switch(exception_status_t*);
//void task_switch();
void exit(int);
void wait();
int fork();
int share_fork();
int getpid();
void set_cur_priority(uint32_t pri);
int task_send_uint32_message(uint32_t , uint32_t );
message_t * task_take_message();
task_t *create_task_from_mem(uint32_t base, uint32_t length, uint32_t need_sti);

#define DUMMY_EIP 0x000001


task_t * task_queue;
task_t * cur_task;

#endif
