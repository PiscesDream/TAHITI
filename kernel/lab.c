#include <task.h>
#include <screen.h>
#include <stdint.h>
#include <syscall.h>
#include <utils/system.h>
#include <heap.h>

_syscall0(int, fork);
_syscall1(int, exit, int, exit_code);
_syscall0(int, wait);
_syscall0(int, getpid);
_syscall1(int, putch, int, ch);
_syscall0(int, ls);
_syscall0(int, tree);
_syscall1(int, sem_apply, int, value);
_syscall1(int, sem_free, int, num);
_syscall1(int, sem_wait, int, num);
_syscall1(int, sem_signal, int, num);

//task_t *create_task_from_mem(uint32_t base, uint32_t length) { // stack size included
task_t* create_task_from_lab(void* base, void* endoflab, uint32_t stack) {
//  scr_printf("Lab starts at %x and ends at %x with stack %x is loading...\n", base, endoflab, stack);
//  return create_task_from_mem((uint32_t)base, (uint32_t)endoflab-(uint32_t)base+stack, 1);
}

void lab1_start(){};
void lab1() {
}
void lab1_end() {};


void lab7_test_mm() {
    uint32_t A = kmalloc(0x100); 
    printf("new A = 0x100 bytes space at: %x\n", A);
    uint32_t B = kmalloc(0x100); 
    printf("new B = 0x100 bytes space at: %x\n", B);
    uint32_t C = kmalloc(0x100); 
    printf("new C = 0x100 bytes space at: %x\n", C);
    kfree(A);
    printf("space of A is freed\n");
    kfree(B);
    printf("space of A is freed\n");
    uint32_t D = kmalloc(0x130); 
    printf("new D = 0x130 bytes space at: %x\n", D);
}

void lab7_test_fork0() {
    scr_puts("start fork. \n");
    int res = share_fork();
    int i = 0;
    for (i = 0; i < 200; ++i) 
        if (res) 
            scr_putch('+');
        else 
            scr_putch('-');
}

void lab7_test_fork() {
    scr_puts("start fork. \n");
    int res = syscall_fork();
//    asm volatile("int $127"); // force switch for the first time
    
    if (res != 0) {syscall_wait();asm volatile("int $127"); }

    printf("My fork() result is [%d]\n", res);
    printf("My pid is [%d]\n", syscall_getpid());
    int i = 0;
    for (i = 0; i < 30; ++i) scr_printf(" %d ", i); 
    if (res == 0) {
        task_send_uint32_message(0, 123); 
        syscall_exit(1); 
        for(;;){hlt();};
    }
    scr_printf("My fork() result is [%d]", res);
    scr_printf("Message [%d]", *((uint32_t*)task_take_message()->addr));
}

void lab7_test_message() {
    printf("start fork. \n");
    int res = fork();
    if (res != 0) {
        wait();
        message_t * message = task_take_message();
        printf("Received message :%d from child, whose pid is %d\n",
                *((uint32_t*)message->addr), message->sender);
    }
    else {
        uint32_t message = 1231;
        printf("Sending message :%d from child\n", message);
        task_send_uint32_message(0, message);
        exit(0);
    }
}


void lab7_v1() {  // abandoned, bugs may be
    char str[80] = "21c6e8a33609e6ef546426c2e0121172378b4a20764bd0a4dd9fcd394c14ab62\0";
    int LetterNr = -1;
    int pid = share_fork(); //fork();
    if (pid == 0) {//child
        LetterNr = 0;
        char *p = str;
        while (*p) {
            if ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z'))
                LetterNr++;
            p++;
        }
        exit(0);
    }
    if (pid > 0) { //parent
        wait();
        printf("The number of letter = %d\n", LetterNr);
    }
}

void lab7_v2() {
    // static here because the ds is copied
    static char str[80] = "21c6e8a33609e6ef546426c2e0121172378b4a20764bd0a4dd9fcd394c14ab62\0";
    int LetterNr = -1;
    int pid = syscall_fork();
    if (pid == 0) {//child
        LetterNr = 0;
        char *p = str;
        while (*p) {
            if ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z'))
                LetterNr++;
            p++;
        }
        scr_printf("I am child process, the LetterNr is %d\n", LetterNr);
        task_send_uint32_message(0, LetterNr);
        exit(0);
    }
    if (pid > 0) { //parent
        syscall_wait();
        LetterNr = *((uint32_t*)task_take_message()->addr);
        printf("I am parent process the number of letter = %d\n", LetterNr);
    }
}

#include <fs/fs.h>

void lab9() {
    syscall_ls();
    cd("DIR");
    syscall_ls();
    cat("SUB");
    cd("..");
    syscall_tree();
    exec("LAB9");
}

void lab8() {
    syscall_ls();
    exec("LAB8");
}
