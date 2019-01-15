#pragma once
#ifndef TASK_H_
#define TASK_H_

/* Data Structure that stores task information needed for the task dispatcher and ICPP */

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/* Other includes */
#include "bool_t.h"
#include "gll.h"
#include "queue.h"

#define WORKER_TASK_NONE_PRIORITY 0

/* variable declarations */
typedef struct workerTask
{
	TaskHandle_t xHandle;
	uint8_t uTaskNumber;
	uint8_t uPriorityLevel;
	uint8_t uReleaseTime;
	uint16_t uStartValue;
	uint16_t uCurrentValue;
	uint16_t uEndValue;
	uint16_t uPeriod;
	gll_t* pUsedQueueList;

} WorkerTask_t;

/* Function declarations */

WorkerTask_t* WorkerTask_Create(TaskFunction_t taskHandler, uint8_t uTaskNumber, uint8_t uPriorityLevel, uint16_t uStartValue, uint16_t uEndValue, uint16_t uPeriod, gll_t* pUsedQueueList);

#endif //end TASK_H_
