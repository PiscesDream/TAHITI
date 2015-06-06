#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

int sem_apply(int);
int sem_free(int sem_num);
void sem_signal(int sem_num);
void sem_wait(int sem_num);

#endif
