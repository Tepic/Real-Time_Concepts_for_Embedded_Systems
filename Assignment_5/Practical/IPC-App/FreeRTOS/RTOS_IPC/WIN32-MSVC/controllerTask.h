#pragma once
#ifndef CONTROLLER_TASK_H_
#define CONTROLLER_TASK_H_

/* Data Structure that stores task information needed for the task dispatcher and ICPP */

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

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
	uint8_t uPriorityLevel;
	uint8_t uReleaseTime;

	// In control tasks uControllerID will define if the controller is main (=1) or back-up (=2) one
	uint16_t uControllerID;

	// Period of execution of the tesk
	uint16_t uPeriod;

	// List of queues that the task's instance is connected to
	gll_t* pUsedQueueList;

} ControllerTask_t;

/* Function declarations */

ControllerTask_t* ControllerTask_Create(TaskFunction_t taskHandler, uint8_t uTaskNumber, uint8_t uPriorityLevel, uint16_t uControllerID, uint16_t uPeriod, gll_t* pUsedQueueList);

#endif //end CONTROLLER_TASK_H_
