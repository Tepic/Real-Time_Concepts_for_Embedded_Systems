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
#include "bool_t.h"
#include "print.h"
#include "gll.h"

#define SEMAPHORE_AQUIRED_BY_NONE 0
#define SEMAPHORE_PRIORITY_CEILING_NONE 0

/* Variable declarations */
typedef struct semaphore Semaphore_t;

/* Function declarations */
int8_t PIP_SemaphoreTake(Semaphore_t* pSemaphore, WorkerTask_t* pTaskToAquireResource, gll_t* pTaskList);
int8_t PIP_vSemaphoreGive(Semaphore_t* pSemaphoreHandle, WorkerTask_t* pTaskToReleaseResource);
Semaphore_t* Semaphore_Create(uint8_t priorityCeiling, const uint8_t* id);
void  Semaphore_vDestroy(Semaphore_t* pSemaphore);
uint8_t Semaphore_GetId(Semaphore_t* pSemaphoreHandle);
void Semaphore_vPrint(Semaphore_t*);
static bool_t isSemaphoreAcquired(Semaphore_t*);
static void transmitTasksActivePriority(Semaphore_t* pSemaphore, WorkerTask_t* pBlockedTask, gll_t* pTaskList);


#endif //end SEMAPHORE_H_