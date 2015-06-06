#include <syscall.h>
#include <stddef.h>
#include <screen.h>
#include <interrupt/idt.h>
#include <task.h>
#include <fs/fs.h>

extern int ls_for_syscall(exception_status_t * t);
extern void putch_for_syscall(exception_status_t * t);
extern int exit_for_syscall(exception_status_t * t);
extern int wait_for_syscall(exception_status_t * t);
extern int fork_for_syscall(exception_status_t * t);
extern int share_fork_for_syscall(exception_status_t * t);
extern int getpid_for_syscall(exception_status_t * t);
extern int get_key_for_syscall(exception_status_t * t);
extern int tree_for_syscall(exception_status_t * t);
extern int sem_apply_for_syscall(exception_status_t *t);
extern int sem_free_for_syscall(exception_status_t *t);
extern int sem_wait_for_syscall(exception_status_t *t);
extern int sem_signal_for_syscall(exception_status_t *t);

static void syscall_handler(exception_status_t *t);
static void *syscalls[64] = {
    0,
    0,
    0,
    0,
    &fork_for_syscall,
    &share_fork_for_syscall,
    &exit_for_syscall,
    &wait_for_syscall,
    &getpid_for_syscall,
    &putch_for_syscall,
    &ls_for_syscall,
    &get_key_for_syscall,
    &tree_for_syscall,
    &sem_apply_for_syscall,
    &sem_free_for_syscall,    
    &sem_wait_for_syscall,
    &sem_signal_for_syscall
};
static uint32_t syscalls_count = 64;

void init_syscall() {
//    irq_install_handler(0x80, &syscall_handler);
    idt_install_handler(0x80, &syscall_handler);
}

void syscall_handler(exception_status_t *t) {
    if (t->eax >= syscalls_count) return;

    int (*service)(exception_status_t *) = syscalls[t->eax];
    t->eax = service(t);
}


//  _syscall1(int, scr_puts, const char*, str);
//  _syscall1(int, scr_putdec, uint32_t, t);
//  _syscall1(int, scr_puthex, uint32_t, t);
// syscalls should be defined in the user program



