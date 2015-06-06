#include <program_header.h>
#include <stdio.h>
#include <semaphore.h>
#include <system.h>
#include <stdlib.h>

void producer();
void consumer();

#define BUFFER_SIZE 5
int s, e, n;
int buffer;

int main() {
    srand(13);
    printf("This is lab 8\n");

    s = sem_apply(1);
    n = sem_apply(0);
    e = sem_apply(BUFFER_SIZE);

    buffer = 0;

    int pid = fork();

    if (pid == 0) 
        producer();
    else 
        consumer();
}

void producer() {
    while (1) {
        uint32_t times = rand()%7;
        while (times--) {
        //    printf("Produced a product !\n");
            sem_wait(e);
            sem_wait(s);

            buffer++;
            printf("%d", buffer);
        //    printf("Buffer length = %d\n", buffer);

            sem_signal(s);
            sem_signal(n);
        }
        switch_task();
    }
}

void consumer() {
    while (1) {
        uint32_t times = rand()%7;
        while (times--) {
            sem_wait(n);
            sem_wait(s);

            buffer--;
            printf("%d", buffer);
        //    printf("Buffer length = %d\n", buffer);

            sem_signal(s);
            sem_signal(e);
        //    printf("Consume a product !\n");
        }
        switch_task();
    }
}
