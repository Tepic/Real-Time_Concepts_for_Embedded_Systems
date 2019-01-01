#pragma once

#ifndef SEMAPHORE_TASK_LIST_H_
#define SEMAPHORE_TASK_LIST_H_

#include "semaphore.h"

/* Variable declarations */
typedef struct semaphoreList SemaphoreList_t;

/* Function declarations */
int8_t WorkerTaskList_vCreate(SemaphoreList_t* pSemaphoreList, uint8_t uLenght);
uint8_t WorkerTaskList_vGetLength(SemaphoreList_t* pSemaphoreList);
WorkerTask_t* WorkerTaskList_vGetElement(SemaphoreList_t* pSemaphoreList, uint8_t uIndex);
void WorkerTaskList_vDestroy(SemaphoreList_t* pSemaphoreList);

#endif // SEMAPHORE_TASK_LIST_H_