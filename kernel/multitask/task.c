#include <stdint.h>
#include <task.h>
#include <string.h>
#include <utils/system.h>
#include <utils/port.h>
#include <heap.h>
#include <debug.h>
#include <screen.h>
#include <gdt.h>
#include <ldt.h>
#include <stddef.h>
#include <syscall.h>
#include <schedule.h>
#include <semaphore.h>

//  volatile task_t *cur_task=0;
//  volatile task_t *task_queue=0;
//  volatile uint32_t pid_counter = 0;
task_t *cur_task=0;
task_t *task_queue=0;
uint32_t pid_counter = 0;
extern uint32_t gdt_count;
extern uint32_t read_eip();

task_t* create_empty_task(uint32_t need_sti) {
    if (need_sti) cli();
    task_t * task = (task_t*)kmalloc(sizeof(task_t)); 
    memset((void *)task, 0, sizeof(task_t));

    task->parent = cur_task;
    task->brothers = cur_task->children;
    cur_task->children = task; 
    cur_task->children_count ++;

    task->pid = pid_counter++;
    task->status = TASK_STATUS_READY;
    task->privilege = 0x0; // fixed

    task->counter = task->priority = 1;
    task->video_page = cur_video_page; // lab 11

    if (task_queue) { //queue not empty
        task_t * tmp = task_queue;
        task->next = tmp->next;
        tmp->next = task;
    }
    else {
        task_queue = task->next = task;
    }

//  kprintf("\nnew task: %x", task);
//  task_t * i = task_queue;
//  kprintf("\n[task: %x next: %x task_queue: %x]-->", i, i->next, task_queue);
//  i = i->next;
//  kprintf("\n[task: %x next: %x task_queue: %x]-->", i, i->next, task_queue);

    // circumlar link list
    if (need_sti) sti();
    return task;
}

task_t *create_task_from_mem(uint32_t base, uint32_t length, uint32_t need_sti) { // stack size included
    // the data and code share the same space 
    if (need_sti) cli();

    task_t* task = create_empty_task(0); 

    task->length = length; 
    task->base = kmalloc(task->length);

    memset((void*)task->base, 0, task->length);
    memcpy((void*)task->base, (void*)base, length);

    _set_gdt_gate(gdt_count++, task->base, task->length, GDT_TEMPLATE_TASK_CODE);
    task->estatus.cs = selector(gdt_count-1, 0b000);
    _set_gdt_gate(gdt_count++, task->base, task->length, GDT_TEMPLATE_TASK_DATA);
    task->estatus.ds = task->estatus.es = task->estatus.fs = task->estatus.gs = task->estatus.ss = \
                       task->estatus.useresp = selector(gdt_count-1, 0b000);
    task->estatus.esp = task->estatus.ebp = length;
    task->estatus.eip = 0;
    task->estatus.eflags = 0x202;

    if (need_sti) sti();
    return task;
}

extern  void move_stack(uint32_t dest_ss, uint32_t dest_esp, uint32_t top); 
void task_switch(exception_status_t *t) {
//0x1eb9
    asm volatile("movl %eax, %eax");
    if (!cur_task) return; // no task or only one task


    task_t* next_task = (task_t*)get_next_task(cur_task);
    if (next_task) {
        cur_task->estatus = *t;
        cur_task->estatus.esp += 0x14;// theoradical 0x14
        *t = next_task->estatus;

//scr_printf(">>old task:\n");
//scr_put_task((task_t*)cur_task);
//scr_printf(">>new task:\n");
//scr_put_task(next_task);

// 2
//extern  void move_stack(uint32_t dest_ss, uint32_t dest_esp, uint32_t top); 
//        move_stack(next_task->estatus.ss, next_task->estatus.esp, cur_task->estatus.esp);
//    scr_printf("copy done.");


//  kprintf("\n");
//  task_t * i = task_queue;
//  do{
//      kprintf("[task: %x status: %x next: %x ]-->", i->pid, i->status, i->next->pid);
//      i = i->next;
//  } while (i != task_queue);

        cur_task = next_task;


  scr_temp_move_to(24, 0);
  kprintf("Current PID: %d      ", cur_task->pid); 
//  scr_printf("  Current ESP: %x", cur_task->estatus.esp); 
  scr_temp_retrieve();

      outb_pause(0x20, 0x20);
      asm volatile("movw %0, %%ax; movw %%ax, %%ss;"
                   "movl %1, %%esp"
                   ::"m"(cur_task->estatus.ss), "m"(cur_task->estatus.esp));
//    asm volatile("pushl %0;pushl %1;"
//                 ::"m"(cur_task->estatus.ss),
//                   "m"(cur_task->estatus.useresp));
      asm volatile("pushl %0;pushl %1; pushl %2;"
                   ::"m"(cur_task->estatus.eflags),
                     "m"(cur_task->estatus.cs),
                     "m"(cur_task->estatus.eip));
      asm volatile("pushl %0;pushl %1; pushl %2; pushl %3;"
                   ::"m"(cur_task->estatus.eax),
                     "m"(cur_task->estatus.ecx), 
                     "m"(cur_task->estatus.edx),
                     "m"(cur_task->estatus.ebx));
      asm volatile("pushl %0;pushl %1; pushl %2; pushl %3;"
                   ::"m"(cur_task->estatus.esp),
                     "m"(cur_task->estatus.ebp), 
                     "m"(cur_task->estatus.esi),
                     "m"(cur_task->estatus.edi));
      asm volatile("pushw %0;pushw %1; pushw %2; pushw %3; pushw %4;"
                   ::"m"(cur_task->estatus.ds),
                     "m"(cur_task->estatus.es), 
                     "m"(cur_task->estatus.fs),
                     "m"(cur_task->estatus.gs),
                     "m"(cur_task->estatus.ss));
      asm volatile("popw %ss;popw %gs;popw %fs;popw %es;popw %ds;popa;sti;iret");
    }
}

void set_cur_priority(uint32_t pri) {
    cli();
    asm volatile("movw $0x10, %ax; movw %ax, %ds");
    cur_task->priority = pri;
    asm volatile("movw %cs, %ax; movw %ax, %ds");
    sti();
}

// all should be used in interruption
int fork() {
    uint32_t eip, ebp, esp, eflags;
    asm volatile("cli; pushf; popl %2;  movl %%ebp, %0; movl %%esp, %1;"
                 "movl %%ss:0x2c(%%esp), %3;":
            "=r" (ebp), "=r"(esp), "=r"(eflags), "=r"(eip));
    esp += 0x30;

    task_t *parent_task = (task_t*)cur_task;
    task_t *new_task = create_task_from_mem(cur_task->base, cur_task->length, 0);
    if (!new_task) {
        sti(); // fail to fork
        return -1;
    }

    new_task->brothers = cur_task->children;
    cur_task->children = new_task;
    cur_task->children_count++;
    new_task->parent = (task_t*)cur_task;

    new_task->estatus.esp = esp;
    new_task->estatus.ebp = ebp;
    new_task->estatus.eip = eip;
    new_task->estatus.eax = 0; // return value of fork()
    new_task->estatus.eflags = eflags;

//  scr_printf("new task:\n");
//  scr_put_task(new_task);

    sti();
    return new_task->pid; // for parent
}

int share_fork() {
    uint32_t eip, ebp, esp, eflags;
    asm volatile("pushf; popl %2; cli; movl %%ebp, %0; movl %%esp, %1;"
                 "movl %%ss:0x2c(%%esp), %3;":
            "=r" (ebp), "=r"(esp), "=r"(eflags), "=r"(eip));
    esp += 0x30;

    task_t *parent_task = (task_t*)cur_task;
    task_t *new_task = create_empty_task(0);
    if (!new_task) {
        sti(); // fail to fork
        return -1;
    }

    new_task->brothers = cur_task->children;
    cur_task->children = new_task;
    cur_task->children_count++;
    new_task->parent = (task_t*)cur_task;

    // share
    new_task->estatus.cs = cur_task->estatus.cs;
    new_task->estatus.ds = cur_task->estatus.ds;
    new_task->estatus.es = cur_task->estatus.es;
    new_task->estatus.fs = cur_task->estatus.fs;
    new_task->estatus.ss = cur_task->estatus.ss;

    new_task->estatus.esp = esp;
    new_task->estatus.ebp = ebp;
    new_task->estatus.eip = eip;
    new_task->estatus.eax = 0; // return value of fork()
    new_task->estatus.eflags = eflags;

    scr_printf("new share task:\n");
    scr_put_task(new_task);

    sti();
    return new_task->pid; // for parent
}


void exit(int a) {
    cli();
    asm volatile("movw $0x10, %ax; movw %ax, %ds");
    wait();
    cur_task->status = TASK_STATUS_DEAD;
    task_t* tmp = cur_task->parent;
    if (tmp) {
        tmp->children_count--;
//      task_t *p = cur_task->parent->children;
//      if (p == cur_task) { // cur_task is the head of children
//          cur_task->parent->children = cur_task->brothers;
//      }
//      else { // delete it from the link list
//          while (p -> brothers != cur_task) p = p->brothers;
//          p->brothers = p->brothers->brothers;
//      }
    }
    sti();
    for (;;){hlt();} // waiting to be killed by schedule
}

void wait() {
    cli();
    asm volatile("movw %ds, %ax; movw %ax, %fs; movw $0x10, %ax; movw %ax, %ds");
    while (cur_task->children_count) { 
        asm volatile("movw %fs, %ax; movw %ax, %ds");
        sti();
        hlt();
        cli();
        asm volatile("movw %ds, %ax; movw %ax, %fs; movw $0x10, %ax; movw %ax, %ds");
    }
    asm volatile("movw %fs, %ax; movw %ax, %ds");
    sti();
}

void init_multitask() {
    cli();

    task_queue = 0;
    cur_task = 0;
    pid_counter = 0;

//    asm volatile("pushf; popl %eax; or $0x3000, %eax; pushl %eax; popf;");
    cur_task = task_queue = (task_t*)create_empty_task(0);
scr_printf("create 0 task at %x\n", (uint32_t)cur_task);
    cur_task->length = 0x90000; 
    cur_task->base = 0x00000;
    
    // clean semaphore
    init_semaphore();

    sti();
}

int getpid() {
    asm volatile("movw %ds, %ax; movw %ax, %fs; movw $0x10, %ax; movw %ax, %ds");
    asm volatile("movw %%fs, %%bx; movw %%bx, %%ds"::"a"(cur_task->pid));
}

int task_send_uint32_message(uint32_t pid, uint32_t content) {
    cli();
    asm volatile("movw %ds, %ax; movw %ax, %fs; movw $0x10, %ax; movw %ax, %ds");

    task_t *tmp = task_queue->next;
    while (tmp && tmp != task_queue && tmp->pid != pid) tmp = tmp->next;
    if (tmp && tmp->pid == pid) {
        // create a message package
        message_t *msg = (message_t*)kmalloc(sizeof(message_t));
        msg->addr = kmalloc(sizeof(uint32_t));
        *((uint32_t*)msg->addr) = content;
        msg->size = sizeof(uint32_t);
        msg->sender = cur_task->pid; 

        message_t *msg_tmp = tmp->message_box;
        if (!msg_tmp) 
            // empty message box
            tmp->message_box = msg;
        else {
            // find the end of queue
            while (msg_tmp->next != 0) msg_tmp = msg_tmp->next;
            msg_tmp->next = msg;
        }
        tmp->message_count++;
    }

    asm volatile("movw %fs, %ax; movw %ax, %ds");
    sti();
}


message_t * task_take_message() {
    cli();
    asm volatile("movw %ds, %ax; movw %ax, %fs; movw $0x10, %ax; movw %ax, %ds");

    message_t * msg;
    if (cur_task->message_count != 0) {
        cur_task->message_count--;
        msg = cur_task->message_box;
        cur_task->message_box = cur_task->message_box->next;
    }

    asm volatile("movw %fs, %ax; movw %ax, %ds");
    sti();
    return msg;
}













int fork_for_syscall(exception_status_t * t) {
    task_t *parent_task = (task_t*)cur_task;
    task_t *new_task = create_task_from_mem(cur_task->base, cur_task->length, 0);
    if (!new_task) 
        return -1;

    new_task->estatus.ds = cur_task->estatus.ds;

    new_task->brothers = cur_task->children;
    cur_task->children = new_task;
    cur_task->children_count++;
    new_task->parent = (task_t*)cur_task;
    
    new_task->estatus.eip = t->eip;
    new_task->estatus.eflags = t->eflags;
    new_task->estatus.edi = t->edi;
    new_task->estatus.esi = t->esi;
    new_task->estatus.ebp = t->ebp;
    new_task->estatus.esp = t->esp+0x14;
    new_task->estatus.eax = 0; // return value of child fork()
    new_task->estatus.ebx = t->ebx;
    new_task->estatus.ecx = t->ecx;
    new_task->estatus.edx = t->edx;

//  kprintf("new task:\n");
//  scr_put_task(new_task);

    return new_task->pid; // for parent
}

int exit_for_syscall(exception_status_t *t) {
    if (cur_task->children_count == 0) { // no need to wait
        cur_task->status = TASK_STATUS_DEAD;  
        task_t* tmp = cur_task->parent;
        if (tmp) tmp->children_count--;
    }
    return (cur_task->status == TASK_STATUS_DEAD);
}

int getpid_for_syscall(exception_status_t * t) {
    return cur_task->pid;
}

int share_fork_for_syscall(exception_status_t * t) {
}

int wait_for_syscall(exception_status_t * t) {
    if (cur_task->children_count > 0) {
        cur_task->status = TASK_STATUS_BLOCKED;
    }
}
