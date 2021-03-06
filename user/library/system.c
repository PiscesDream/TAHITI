#include <system.h>
#include <syscall.h>

int fork() {
    return syscall_fork();
}

int exit(int exit_code) {
    syscall_wait();
    syscall_exit(exit_code);
    for (;;) {}
}

int wait() {
    return syscall_wait();
}

int getpid() {
    return syscall_getpid();
}

void switch_task() {
    asm volatile("int $127");
}

void print_path() {
    syscall_print_path();
}
