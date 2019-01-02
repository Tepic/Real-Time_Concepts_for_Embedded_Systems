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
#include "gll.h"

#define WORKER_TASK_NONE_PRIORITY 0


/* variable declarations */
typedef struct workerTask
{
	TaskHandle_t xHandle;
	uint8_t uTaskNumber;
	uint8_t uNominalPriority;
	uint8_t uActivePriority;
	uint8_t uPriorityWhenItAcquiredResource;
	uint8_t uReleaseTime;
	uint8_t uPeriod;
	gll_t* pAcquiredResourceList;
	gll_t* pUsedSemaphoreList;

} WorkerTask_t;

/* Function declarations */

WorkerTask_t* WorkerTask_Create(TaskFunction_t taskHandler, uint8_t uTaskNumber, uint8_t nominalPriority, uint8_t uReleaseTime, uint8_t uPeriod, gll_t* pUsedSemaphoreList);
void  WorkerTask_vDestroy(WorkerTask_t* pWorkerTask);
void WorkerTask_vPrint(WorkerTask_t*);
void WorkerTask_vListAddTaskDescendingPriorityOrder(gll_t* pTaskList, WorkerTask_t* pWorkerTask);
void WorkerTask_vListPrintPriority(gll_t* pTaskList);
void WorkerTask_vResetActivePriorityToNominalPriority(WorkerTask_t* pWorkerTask);

#endif //end TASK_H_
