#include <syscall.h>
#include <semaphore.h>
#include <system.h>

int sem_apply(int value) {
    return syscall_sem_apply(value);
}

int sem_free(int sem_num) {
    return syscall_sem_free(sem_num);
}

void sem_wait(int sem_num) {
    int r = syscall_sem_wait(sem_num);
    if (r == 0) {
        switch_task();
    }
}


void sem_signal(int sem_num) {
    syscall_sem_signal(sem_num);
}
