#include <semaphore.h>
#include <stddef.h>
#include <stdbool.h>
#include <task.h>
#include <utils/system.h>
#include <screen.h>

semaphore_t sem[SEMAPHORE_COUNT];

void init_semaphore() {
    int i;
    for (i = 0; i < SEMAPHORE_COUNT; ++i) {
        sem[i].used = false;
        sem[i].value = 0;
        sem[i].front = 0;
    }
}

int sem_apply_for_syscall(exception_status_t *t) {
    int i = 0;
    for (i = 0; i < SEMAPHORE_COUNT; ++i) {
        if (sem[i].used == false) {
            sem[i].used = true; 
            sem[i].value = t->ebx; // first argument
            sem[i].front = 0;
//            kprintf("sem[%d] is %d\n", i, sem[i].value); 
            return i;
        }
    }
    return -1;
}


int sem_free_for_syscall(exception_status_t *t) {
    sem[t->ebx].used = false;
}


int sem_wait_for_syscall(exception_status_t *t) {
    int i = t->ebx;
    if (sem[i].used == false) panic("Using an unassigned semaphore !");
    sem[i].value--;

//    kprintf("sem[%d] is %d\n", i, sem[i].value); 

    if (sem[i].value < 0) {
//        kprintf("blocking task %d\n", cur_task->pid); 
//        kprintf("-"); 

        task_t * tmp = sem[i].front;
        if (!tmp) 
            sem[i].front = cur_task; 
        else {
            while (tmp->semaphore_next) 
                tmp = tmp->semaphore_next; // find the queue end
            tmp->semaphore_next = cur_task;
        }
        cur_task->semaphore_next = 0;

        cur_task->status = TASK_STATUS_BLOCKED;
    }
    if (cur_task->status == TASK_STATUS_BLOCKED) return false;
    return true;
}

int sem_signal_for_syscall(exception_status_t *t) {
    int i = t->ebx;
    if (sem[i].used == false) panic("Using an unassigned semaphore !");
    sem[i].value ++;

//    kprintf("sem[%d] is %d\n", i, sem[i].value); 

    if (sem[i].value <= 0) {
        task_t * tmp = sem[i].front;
        tmp->status = TASK_STATUS_READY;
        sem[i].front = tmp->semaphore_next;
        tmp->semaphore_next = 0;

//        kprintf("+"); 
//        kprintf("releasing task %d\n", tmp->pid); 
//kprintf("[task: %x status: %x next: %x ]-->", tmp->pid, tmp->status, tmp->next->pid);
    }
}


