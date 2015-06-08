#ifndef _LAB_H_
#define _LAB_H_

#include <task.h>
#include <stdint.h>

task_t* create_task_from_lab(void* base, void* endoftest, uint32_t stack);
#define DECL_LAB(labname) void labname##_start();void labname##_end();

void lab7_test_fork0();
void lab7_test_message();
void lab7_test_fork();
void lab7_test_mm();
void lab7_v1();
void lab7_v2();

void lab8();
void lab9();
void lab10();
void shell();



#endif
