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
WorkerTask_t* WorkerTask_Create(TaskFunction_t taskHandler, uint8_t uTaskNumber, uint8_t nominalPriority, uint32_t uExecutionTime);
void  WorkerTask_vDestroy(WorkerTask_t* pWorkerTask);
uint8_t WorkerTask_vGetTaskNumber(WorkerTask_t* pWorkerTask);
void WorkerTask_vSetActivePriority(WorkerTask_t* pWorkerTask, uint8_t uActivePriority);
void WorkerTask_vResetActivePriority(WorkerTask_t* pWorkerTask);
uint8_t WorkerTask_uGetActivePriority(WorkerTask_t* pWorkerTask);
uint8_t WorkerTask_uGetNominalPriority(WorkerTask_t* pWorkerTask);
TaskHandle_t WorkerTask_vGetHandle(WorkerTask_t* pWorkerTask);
void WorkerTask_vPrint(WorkerTask_t*);
uint8_t WorkerTask_vSizeOf();

#endif //end TASK_H_