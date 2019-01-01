#pragma once
#ifndef TASK_H_
#define TASK_H_

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

#endif //end TASK_H_