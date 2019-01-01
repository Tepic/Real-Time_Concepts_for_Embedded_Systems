#pragma once
#ifndef TASK_H_
#define TASK_H_

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/* Other includes */
#include "bool_t.h"

/* variable declarations */
typedef struct workerTask WorkerTask_t;

/* Function declarations */
void WorkerTask_vInit(WorkerTask_t* pWorkerTask, uint8_t uTaskNumber, uint8_t nominalPriority, uint32_t uExecutionTime);
uint8_t WorkerTask_vGetTaskNumber(WorkerTask_t* pWorkerTask);

#endif //end TASK_H_