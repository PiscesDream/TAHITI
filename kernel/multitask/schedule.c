#include <schedule.h>
#include <task.h>
#include <screen.h>


// FIFO schedule algorithm, no efficient
volatile task_t* get_next_task(volatile task_t * cur_task) {
    task_t * tmp = cur_task->next;
    while (tmp != cur_task && tmp->status!=TASK_STATUS_READY) 
        tmp = tmp->next;
    return tmp;
}


// new way to do it right
volatile task_t* __get_next_task(volatile task_t * cur_task) {
    cur_task->counter--;
    if (cur_task->counter > 0) return cur_task;

    task_t * tmp = (task_t*)cur_task;
    while (tmp->counter <= 0) {
        tmp = tmp->next; // find the first one still not run out of time
        if (tmp == cur_task) break;
    }
    if (tmp == cur_task && tmp->counter == 0) {
        while (1) { // all tasks run out of time
            tmp->counter = tmp->priority;
            tmp = tmp->next;
            if (tmp == cur_task) break;
        }
    }
    tmp = cur_task->next;
    while (tmp->status == TASK_STATUS_DEAD) tmp = tmp->next;
    return tmp;
}
