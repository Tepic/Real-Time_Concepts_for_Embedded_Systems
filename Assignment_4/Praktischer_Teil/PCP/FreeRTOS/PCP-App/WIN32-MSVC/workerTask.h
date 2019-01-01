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

typedef struct workerTask WorkerTask_t;

void WorkerTask_vInit(WorkerTask_t* pWorkerTask, uint8_t uTaskNumber, uint8_t nominalPriority, uint32_t uExecutionTime);

#endif //end TASK_H_