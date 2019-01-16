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

	// In sensor tasks uStartValue will be used to start the counter
	// In control tasks uStartValue will define if the controller is main (=1) or back-up (=2) one
	uint16_t uStartValue;

	// Current value of the sensor counter
	uint16_t uCurrentValue;

	// Ceilling value of the counter at which the counter wraps around
	uint16_t uEndValue;

	uint16_t uPeriod;

	// List of queues that the task's instance is connected to
	gll_t* pUsedQueueList;

} WorkerTask_t;

/* Function declarations */

WorkerTask_t* WorkerTask_Create(TaskFunction_t taskHandler, uint8_t uTaskNumber, uint8_t uPriorityLevel, uint16_t uStartValue, uint16_t uEndValue, uint16_t uPeriod, gll_t* pUsedQueueList);

#endif //end TASK_H_
