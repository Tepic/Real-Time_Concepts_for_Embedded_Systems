#pragma once
#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

/* Standard includes. */
#include <conio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "semphr.h"

/* Other includes */
#include "bool_t.h"
#include "print.h"

#define SEMAPHORE_AQUIRED_NONE -1
#define SEMAPHORE_PRIORITY_CEILING_NONE -1

typedef struct semaphore Semaphore_t;

void PIP_vSemaphoreTake(SemaphoreHandle_t);
void PIP_vSemaphoreGive(SemaphoreHandle_t);
void Semaphore_vInit(Semaphore_t* pWorkerTask, uint8_t priorityCeiling, const uint8_t id);
static bool_t isSemaphoreHolded();

#endif //end SEMAPHORE_H_