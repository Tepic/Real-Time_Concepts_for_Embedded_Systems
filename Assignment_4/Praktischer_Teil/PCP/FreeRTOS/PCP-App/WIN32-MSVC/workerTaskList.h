#pragma once
#ifndef WORKER_TASK_LIST_H_
#define WORKER_TASK_LIST_H_

#include "workerTask.h"

/* Variable declarations */
typedef struct workerTaskList WorkerTaskList_t;

/* Function declarations */
int8_t WorkerTaskList_vCreate(WorkerTaskList_t* pTaskList, uint8_t uLenght);
uint8_t WorkerTaskList_vGetLength(WorkerTaskList_t* pTaskList);
WorkerTask_t* WorkerTaskList_vGetElement(WorkerTaskList_t* pTaskList, uint8_t uIndex);
void WorkerTaskList_vDestroy(WorkerTaskList_t* pTaskList);

#endif // WORKER_TASK_LIST_H_