#pragma once
#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

/* Standard includes. */
#include <conio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/* Other includes */
#include "bool_t.h"

typedef struct workerTask
{
	TaskHandle_t xHandle;
	bool_t isMissbehaved;
	uint32_t uExecutionTime;
	uint8_t uTaskNumber;

} WorkerTask_t;

void WorkerTask_vInit(WorkerTask_t* pWorkerTask, uint8_t uTaskNumber, bool_t isMissbehaved, uint32_t uExecutionTime);

#endif //end SEMAPHORE_H_