#include <system.h>

extern int syscall_fork();
extern int syscall_exit(int);
extern int syscall_wait();
extern int syscall_getpid();

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
