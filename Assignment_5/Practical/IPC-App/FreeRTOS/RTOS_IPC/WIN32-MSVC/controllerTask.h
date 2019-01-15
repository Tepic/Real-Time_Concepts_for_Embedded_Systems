#pragma once
#ifndef CONTROLLER_TASK_H_
#define CONTROLLER_TASK_H_

/* Data Structure that stores task information needed for the task dispatcher and ICPP */

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/* Other includes */
#include "bool_t.h"
#include "gll.h"
#include "queue.h"

#define CONTROLLER_TASK_NONE_PRIORITY 0

/* variable declarations */
typedef struct controllerTask
{
	TaskHandle_t xHandle;
	uint8_t uTaskNumber;
	uint8_t uNominalPriority;
	uint8_t uActivePriority;
	uint8_t uReleaseTime;
	uint16_t uPeriod;
	gll_t* pUsedSemaphoreList;
	gll_t* pBlockedTaskList; // List of blocked tasks on this task
	gll_t* pUsedQueueList;

} ControllerTask_t;

/* Function declarations */

ControllerTask_t* ControllerTask_Create(TaskFunction_t taskHandler, uint8_t uTaskNumber, uint8_t nominalPriority, uint16_t uPeriod, gll_t* pUsedSemaphoreList, gll_t* pUsedQueueList);
void ControllerTask_vDestroy(ControllerTask_t* pControllerTask);
void ControllerTask_vPrint(ControllerTask_t*);
void ControllerTask_vListAddTaskDescendingPriorityOrder(gll_t* pTaskList, ControllerTask_t* pControllerTask);
void ControllerTask_vListPrintPriority(gll_t* pTaskList);
void ControllerTask_vResetActivePriorityToNominalPriority(ControllerTask_t* pControllerTask);

#endif //end TASK_H_
