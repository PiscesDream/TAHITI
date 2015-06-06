#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include <task.h>
volatile task_t* get_next_task(volatile task_t * cur_task);

#endif 
