#pragma once
#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

/* Standard includes. */
#include <conio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "semphr.h"

/* Other includes */
#include "workerTask.h"
#include "workerTaskList.h"
#include "bool_t.h"
#include "print.h"

#define SEMAPHORE_AQUIRED_BY_NONE -1
#define SEMAPHORE_PRIORITY_CEILING_NONE -1

/* Variable declarations */
typedef struct semaphore Semaphore_t;

/* Function declarations */
void PIP_vSemaphoreTake(Semaphore_t* pSemaphore, WorkerTask_t* pTaskToAquireResource, WorkerTaskList_t* pTaskList);
void PIP_vSemaphoreGive(Semaphore_t*);
void Semaphore_vInit(Semaphore_t* pSemaphoreHandle, uint8_t priorityCeiling, uint8_t id);
uint8_t Semaphore_vGetId(Semaphore_t* pSemaphoreHandle);
static bool_t isSemaphoreAcquired(Semaphore_t*);


#endif //end SEMAPHORE_H_