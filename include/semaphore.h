#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#define SEMAPHORE_COUNT 100

#include <stdbool.h>
#include <task.h>

typedef struct __semaphore_t {
    int value;
    bool used;
    task_t * front;
} semaphore_t;

void init_semaphore();

#endif
